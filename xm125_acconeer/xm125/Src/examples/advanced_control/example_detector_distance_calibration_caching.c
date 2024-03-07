// Copyright (c) Acconeer AB, 2024
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_definitions_a121.h"
#include "acc_detector_distance.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_integration_log.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"
#include "acc_version.h"


/** \example example_detector_distance_calibration_caching.c
 * @brief This is an example on how the sensor and detector calibration can be cached
 * @n
 * This example executes as follows:
 *   - Retrieve and register HAL struct
 *   - Create and initialize distance detector resources
 *   - Create and calibrate sensor
 *   - Calibrate detector
 *   - Loop:
 *     - Prepare, measure, read, process data using the detector
 *     - Check 'calibration_needed' indication
 *     - If needed, either use cached calibration or perform new calibration and store
 *   - Destroy resources
 */


#define SENSOR_ID (1U)
// 2 seconds should be enough even for long ranges and high signal quality
#define SENSOR_TIMEOUT_MS (2000U)


/**
 * A sensor calibration and the dynamic part of a detector calibration are valid at the
 * temperature they were done +- 15 degrees.
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

// Variables for temperature independent caching
static uint8_t  *detector_cal_result_static;
static uint32_t detector_cal_result_static_size = 0;

// Variables for temperature dependent caching
static acc_cal_result_t                  sensor_cal_results[MAX_CACHE_COUNT];
static acc_detector_cal_result_dynamic_t detector_cal_results_dynamic[MAX_CACHE_COUNT];
static int16_t                           cal_temps[MAX_CACHE_COUNT];
static uint16_t                          curr_cache_count   = 0;
static uint16_t                          cache_index_in_use = 0;


typedef struct
{
	acc_sensor_t                   *sensor;
	acc_detector_distance_config_t *config;
	acc_detector_distance_handle_t *handle;
	void                           *buffer;
	uint32_t                       buffer_size;
} distance_detector_resources_t;


// General control functions
static void set_config(acc_detector_distance_config_t *detector_config);


static bool initialize_detector_resources(distance_detector_resources_t *resources);


static bool detector_get_next(distance_detector_resources_t  *resources,
                              acc_detector_distance_result_t *result);


static void cleanup(distance_detector_resources_t *resources);


// Calibration and caching functions
static bool sensor_and_detector_calibration(distance_detector_resources_t *resources, uint16_t cache_index);


static bool sensor_calibration(distance_detector_resources_t *resources, uint16_t cache_index);


static bool detector_calibration_full(distance_detector_resources_t *resources, uint16_t cache_index);


static bool detector_calibration_update(distance_detector_resources_t *resources, uint16_t cache_index);


static bool calibration_caching(distance_detector_resources_t *resources, int16_t temp);


// Caching helper functions
static bool find_cache_index(int16_t temp, uint16_t *cache_index);


static bool get_next_empty_cache_index(uint16_t *cache_index);


static bool add_cache(uint16_t cache_index, int16_t temp);


// Main function
int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	distance_detector_resources_t resources = { 0 };

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	resources.config = acc_detector_distance_config_create();
	if (resources.config == NULL)
	{
		printf("Config creation failed\n");
		return EXIT_FAILURE;
	}

	set_config(resources.config);

	if (!initialize_detector_resources(&resources))
	{
		printf("Initializing detector resources failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	acc_detector_distance_config_log(resources.handle, resources.config);

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	resources.sensor = acc_sensor_create(SENSOR_ID);
	if (resources.sensor == NULL)
	{
		printf("Sensor creation failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	// Do a sensor calibration and a full detector calibration and store first in cache.
	if (!sensor_and_detector_calibration(&resources, 0))
	{
		printf("Sensor or detector calibration failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	uint32_t update_count = 5U;

	for (uint32_t i = 0U; i < update_count; i++)
	{
		acc_detector_distance_result_t result = { 0 };

		if (!detector_get_next(&resources, &result))
		{
			printf("Could not get next result\n");
			cleanup(&resources);
			return EXIT_FAILURE;
		}

		if (result.calibration_needed)
		{
			printf("New calibrations needed due to temperature change\n");

			if (!calibration_caching(&resources, result.temperature))
			{
				printf("Calibration caching failed\n");
				cleanup(&resources);
				return EXIT_FAILURE;
			}
		}
		else
		{
			printf("Distance result retrieved\n");
		}
	}

	cleanup(&resources);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


// General control functions
static void set_config(acc_detector_distance_config_t *detector_config)
{
	// Balanced detector config
	acc_detector_distance_config_start_set(detector_config, 0.25f);
	acc_detector_distance_config_end_set(detector_config, 3.0f);
	acc_detector_distance_config_max_step_length_set(detector_config, 0U);
	acc_detector_distance_config_max_profile_set(detector_config, ACC_CONFIG_PROFILE_5);
	acc_detector_distance_config_reflector_shape_set(detector_config, ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_GENERIC);
	acc_detector_distance_config_peak_sorting_set(detector_config, ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST);
	acc_detector_distance_config_threshold_method_set(detector_config, ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_CFAR);
	acc_detector_distance_config_num_frames_recorded_threshold_set(detector_config, 100U);
	acc_detector_distance_config_threshold_sensitivity_set(detector_config, 0.5f);
	acc_detector_distance_config_signal_quality_set(detector_config, 15.0f);
	acc_detector_distance_config_close_range_leakage_cancellation_set(detector_config, false);
}


static bool initialize_detector_resources(distance_detector_resources_t *resources)
{
	resources->handle = acc_detector_distance_create(resources->config);
	if (resources->handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		return false;
	}

	if (!acc_detector_distance_get_sizes(resources->handle, &(resources->buffer_size), &detector_cal_result_static_size))
	{
		printf("acc_detector_distance_get_sizes() failed\n");
		return false;
	}

	resources->buffer = acc_integration_mem_alloc(resources->buffer_size);
	if (resources->buffer == NULL)
	{
		printf("sensor buffer allocation failed\n");
		return false;
	}

	detector_cal_result_static = acc_integration_mem_alloc(detector_cal_result_static_size);
	if (detector_cal_result_static == NULL)
	{
		printf("static cal result allocation failed\n");
		return false;
	}

	return true;
}


static bool detector_get_next(distance_detector_resources_t  *resources,
                              acc_detector_distance_result_t *result)
{
	bool result_available = false;

	do
	{
		if (!acc_detector_distance_prepare(resources->handle, resources->config, resources->sensor, &sensor_cal_results[cache_index_in_use],
		                                   resources->buffer,
		                                   resources->buffer_size))
		{
			printf("acc_detector_distance_prepare() failed\n");
			return false;
		}

		if (!acc_sensor_measure(resources->sensor))
		{
			printf("acc_sensor_measure() failed\n");
			return false;
		}

		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("Sensor interrupt timeout\n");
			return false;
		}

		if (!acc_sensor_read(resources->sensor, resources->buffer, resources->buffer_size))
		{
			printf("acc_sensor_read() failed\n");
			return false;
		}

		if (!acc_detector_distance_process(resources->handle, resources->buffer, detector_cal_result_static,
		                                   &detector_cal_results_dynamic[cache_index_in_use],
		                                   &result_available, result))
		{
			printf("acc_detector_distance_process() failed\n");
			return false;
		}
	} while (!result_available);

	return true;
}


static void cleanup(distance_detector_resources_t *resources)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (resources->sensor != NULL)
	{
		acc_sensor_destroy(resources->sensor);
	}

	if (detector_cal_result_static != NULL)
	{
		acc_integration_mem_free(detector_cal_result_static);
	}

	if (resources->buffer != NULL)
	{
		acc_integration_mem_free(resources->buffer);
	}

	if (resources->handle != NULL)
	{
		acc_detector_distance_destroy(resources->handle);
	}

	if (resources->config != NULL)
	{
		acc_detector_distance_config_destroy(resources->config);
	}
}


// Calibration and caching functions
static bool sensor_and_detector_calibration(distance_detector_resources_t *resources, uint16_t cache_index)
{
	bool status = sensor_calibration(resources, cache_index);

	if (status)
	{
		status = detector_calibration_full(resources, cache_index);
	}

	if (status)
	{
		acc_cal_info_t cal_info;
		acc_sensor_get_cal_info(&sensor_cal_results[cache_index], &cal_info);
		add_cache(cache_index, cal_info.temperature);
		cache_index_in_use = cache_index;
	}

	return status;
}


static bool sensor_calibration(distance_detector_resources_t *resources, uint16_t cache_index)
{
	bool           status              = false;
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
			status = acc_sensor_calibrate(resources->sensor, &cal_complete, &sensor_cal_results[cache_index], resources->buffer,
			                              resources->buffer_size);

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
	}

	return status;
}


static bool detector_calibration_full(distance_detector_resources_t *resources, uint16_t cache_index)
{
	bool status       = false;
	bool cal_complete = false;

	do
	{
		status = acc_detector_distance_calibrate(resources->sensor,
		                                         resources->handle,
		                                         &sensor_cal_results[cache_index],
		                                         resources->buffer,
		                                         resources->buffer_size,
		                                         detector_cal_result_static,
		                                         detector_cal_result_static_size,
		                                         &detector_cal_results_dynamic[cache_index],
		                                         &cal_complete);

		if (status && !cal_complete)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status && !cal_complete);

	return status;
}


static bool detector_calibration_update(distance_detector_resources_t *resources, uint16_t cache_index)
{
	bool status       = false;
	bool cal_complete = false;

	do
	{
		status = acc_detector_distance_update_calibration(resources->sensor,
		                                                  resources->handle,
		                                                  &sensor_cal_results[cache_index],
		                                                  resources->buffer,
		                                                  resources->buffer_size,
		                                                  &detector_cal_results_dynamic[cache_index],
		                                                  &cal_complete);

		if (status && !cal_complete)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status && !cal_complete);

	return status;
}


static bool calibration_caching(distance_detector_resources_t *resources, int16_t temp)
{
	bool     status      = true;
	uint16_t cache_index = 0;
	bool     use_cache   = find_cache_index(temp, &cache_index);

	// If no cached calibration can be used, a new calibration should be made
	// and the result stored at an empty cache_index
	if (!use_cache)
	{
		printf("New sensor calibration and detector calibration update is performed\n");

		if (!get_next_empty_cache_index(&cache_index))
		{
			printf("No empty cache_index to store calibration result\n");
			return false;
		}

		status = sensor_calibration(resources, cache_index);

		if (status)
		{
			status = detector_calibration_update(resources, cache_index);
		}

		if (status)
		{
			acc_cal_info_t cal_info;
			acc_sensor_get_cal_info(&sensor_cal_results[cache_index], &cal_info);
			add_cache(cache_index, cal_info.temperature);
			cache_index_in_use = cache_index;
		}
	}
	else
	{
		cache_index_in_use = cache_index;
		printf("Using cached calibration for %u degrees Celsius\n", cal_temps[cache_index_in_use]);
	}

	return status;
}


// Caching helper functions
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
