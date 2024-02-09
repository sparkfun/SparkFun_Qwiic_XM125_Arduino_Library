// Copyright (c) Acconeer AB, 2020-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "acc_config.h"
#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_processing.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"


/** \example example_service_calibration_caching.c
 * @brief This is an example on how the sensor calibration can be cached
 * @n
 * The example executes as follows:
 *   - Retrieve and register HAL stuct
 *   - Create config, processing, and sensor instances
 *   - Calibrate and prepare sensor
 *   - Measure, read and process radar data
 *   - Check 'calibration_needed' indication
 *   - If needed either use cached calibration or perform new calibration and store
 *   - Destroy sensor, processing, and config instances
 */

#define SENSOR_ID          (1U)
#define SENSOR_TIMEOUT_MS  (1000U)
#define MAX_DATA_ENTRY_LEN (15U) // "-32000+-32000i" + zero termination

/**
 * A sensor calibration is valid at the temperature it was done +- 15 degrees
 *
 * If the temperature isn't controlled during caching, which is the case in this example,
 * the maximum amount of caches needs to be calculated from a temperature difference of 16.
 *
 * For example
 * - A calibration is done at 25 degrees, which means it is valid between 10 and 40 degrees
 * - The temperature changes to 41 degrees
 * - A new calibration needs to be done at 41 degrees since it is above the valid range for the previous calibration
 * - The new calibration is then valid between 26 and 56 degrees
 *
 * However, if the temperature is controlled, for example in a factory, the maximum amount
 * of caches can be calculated from a temperature difference of 30.
 *
 * For example
 * - A calibration is done at 25 degrees, which means it is valid between 10 and 40 degrees
 * - The temperature is manually changed to 55 degrees
 * - A new calibration can be done which is valid between 40 and 70 degrees
 *
 * The maximum temperature variation that the application will operate in is assumed to be
 * -40 to 85 degrees in this example.
 */
#define MAX_CAL_TEMP_DIFF  (16)
#define MAX_TEMP_VARIATION (125)
#define MAX_CACHE_COUNT    ((MAX_TEMP_VARIATION / MAX_CAL_TEMP_DIFF) + 1)

// Note that each 'acc_cal_result_t' struct should be suitably aligned for any built-in type
static acc_cal_result_t cal_results[MAX_CACHE_COUNT];
static int16_t          cal_temps[MAX_CACHE_COUNT];
static uint16_t         curr_cache_count = 0;

static void set_config(acc_config_t *config);


static bool find_cache_index(int16_t temp, uint16_t *cache_index);


static bool get_next_empty_cache_index(uint16_t *cache_index);


static bool add_cache(uint16_t cache_index, int16_t temp);


static bool calibration_caching_and_prepare(acc_sensor_t *sensor, acc_config_t *config,
                                            void *buffer, uint32_t buffer_size,
                                            bool temp_known, int16_t temp);


static void cleanup(acc_config_t *config, acc_processing_t *processing,
                    acc_sensor_t *sensor, void *buffer);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_config_t              *config     = NULL;
	acc_processing_t          *processing = NULL;
	acc_sensor_t              *sensor     = NULL;
	void                      *buffer     = NULL;
	uint32_t                  buffer_size = 0;
	acc_processing_metadata_t proc_meta;
	acc_processing_result_t   proc_result;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	config = acc_config_create();
	if (config == NULL)
	{
		printf("acc_config_create() failed\n");
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	set_config(config);

	// Print the configuration
	acc_config_log(config);

	processing = acc_processing_create(config, &proc_meta);
	if (processing == NULL)
	{
		printf("acc_processing_create() failed\n");
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!acc_rss_get_buffer_size(config, &buffer_size))
	{
		printf("acc_rss_get_buffer_size() failed\n");
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	buffer = acc_integration_mem_alloc(buffer_size);
	if (buffer == NULL)
	{
		printf("buffer allocation failed\n");
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!calibration_caching_and_prepare(sensor, config, buffer, buffer_size, false, 0))
	{
		printf("calibration_caching_and_prepare() failed\n");
		acc_sensor_status(sensor);
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	uint32_t update_count = 5U;

	for (uint32_t i = 0U; i < update_count; i++)
	{
		if (!acc_sensor_measure(sensor))
		{
			printf("acc_sensor_measure failed\n");
			acc_sensor_status(sensor);
			cleanup(config, processing, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("Sensor interrupt timeout\n");
			acc_sensor_status(sensor);
			cleanup(config, processing, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_sensor_read(sensor, buffer, buffer_size))
		{
			printf("acc_sensor_read failed\n");
			acc_sensor_status(sensor);
			cleanup(config, processing, sensor, buffer);
			return EXIT_FAILURE;
		}

		acc_processing_execute(processing, buffer, &proc_result);

		if (proc_result.calibration_needed)
		{
			printf("New calibration needed due to temperature change\n");

			if (!calibration_caching_and_prepare(sensor, config, buffer, buffer_size, true, proc_result.temperature))
			{
				printf("calibration_caching_and_prepare() failed\n");
				acc_sensor_status(sensor);
				cleanup(config, processing, sensor, buffer);
				return EXIT_FAILURE;
			}
		}
		else
		{
			printf("IQ data retrieved\n");
		}
	}

	cleanup(config, processing, sensor, buffer);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void set_config(acc_config_t *config)
{
	// Add configuration of the sensor here

	acc_config_start_point_set(config, 80);
	acc_config_num_points_set(config, 160);
}


static bool find_cache_index(int16_t temp, uint16_t *cache_index)
{
	bool     cache_found   = false;
	uint16_t min_temp_diff = UINT16_MAX;

	// If caches have overlapping temperature ranges, the cache
	// with center temperature closest to 'temp' will be chosen
	for (uint16_t index = 0; index < curr_cache_count; index++)
	{
		uint16_t temp_diff = abs(cal_temps[index] - temp);

		if (temp_diff < MAX_CAL_TEMP_DIFF && temp_diff < min_temp_diff)
		{
			min_temp_diff = temp_diff;
			*cache_index  = index;
			cache_found   = true;
		}
	}

	return cache_found;
}


static bool get_next_empty_cache_index(uint16_t *cache_index)
{
	*cache_index = curr_cache_count;

	return curr_cache_count < MAX_CACHE_COUNT;
}


static bool add_cache(uint16_t cache_index, int16_t temp)
{
	bool status = false;

	if ((cache_index == curr_cache_count) && (cache_index < MAX_CACHE_COUNT))
	{
		cal_temps[cache_index] = temp;
		curr_cache_count++;
		status = true;
	}

	return status;
}


static bool calibration_caching_and_prepare(acc_sensor_t *sensor, acc_config_t *config,
                                            void *buffer, uint32_t buffer_size,
                                            bool temp_known, int16_t temp)
{
	bool     status      = true;
	bool     use_cache   = false;
	uint16_t cache_index = 0;

	// If temperature is known, there might be a cached calibration result that can be used
	if (temp_known)
	{
		if (find_cache_index(temp, &cache_index))
		{
			use_cache = true;
		}
	}

	// If no cached calibration can be used, a new calibration should be made
	// and the result stored at an empty cache_index
	if (!use_cache)
	{
		printf("New calibration is performed\n");

		if (!get_next_empty_cache_index(&cache_index))
		{
			printf("No empty cache_index to store calibration result\n");
			return false;
		}

		status = false;
		bool           cal_complete        = false;
		const uint16_t calibration_retries = 1U;

		// Random disturbances may cause the calibration to fail. At failure, retry at least once.
		for (uint16_t i = 0; !status && (i <= calibration_retries); i++)
		{
			// Reset sensor before calibration by disabling/enabling it
			acc_hal_integration_sensor_disable(SENSOR_ID);
			acc_hal_integration_sensor_enable(SENSOR_ID);

			do
			{
				status = acc_sensor_calibrate(sensor, &cal_complete, &cal_results[cache_index], buffer, buffer_size);

				if (status && !cal_complete)
				{
					status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
				}
			} while (status && !cal_complete);
		}

		if (status)
		{
			// Reset sensor after calibration by disabling/enabling it
			acc_hal_integration_sensor_disable(SENSOR_ID);
			acc_hal_integration_sensor_enable(SENSOR_ID);

			acc_cal_info_t cal_info;
			acc_sensor_get_cal_info(&cal_results[cache_index], &cal_info);
			add_cache(cache_index, cal_info.temperature);
		}
	}
	else
	{
		printf("Using cached calibration for %u degrees Celsius\n", cal_temps[cache_index]);
	}

	if (status)
	{
		status = acc_sensor_prepare(sensor, config, &cal_results[cache_index], buffer, buffer_size);
	}

	return status;
}


static void cleanup(acc_config_t *config, acc_processing_t *processing,
                    acc_sensor_t *sensor, void *buffer)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (sensor != NULL)
	{
		acc_sensor_destroy(sensor);
	}

	if (processing != NULL)
	{
		acc_processing_destroy(processing);
	}

	if (config != NULL)
	{
		acc_config_destroy(config);
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}
}
