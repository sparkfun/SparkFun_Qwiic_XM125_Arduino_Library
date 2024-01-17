// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_detector_distance.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_integration_log.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"


/** \example example_detector_distance_low_power_off.c
 * @brief This is an example on how to disable the sensor and put the system in a low power state between measurements
 * @n
 * The example executes as follows:
 *   - Create a distance configuration
 *   - Create a sensor instance
 *   - Enable sensor
 *   - Create a detector instance
 *   - Calibrate the sensor
 *   - Calibrate the detector
 *    - Disable sensor
 *   - Loop forever:
 *     - Enable sensor
 *     - Prepare the detector
 *     - Perform a sensor measurement and read out the data
 *     - Disable sensor
 *     - Process the measurement and get detector result
 *     - Put the system in deep sleep for a specified amount of time
 *   - Destroy the configuration
 *   - Destroy the detector instance
 *   - Destroy the sensor instance
 */

#define SENSOR_ID           (1U)
#define SENSOR_TIMEOUT_MS   (1000U)
#define DEFAULT_UPDATE_RATE (1.0f)


static void print_result(acc_detector_distance_result_t *result);


static void cleanup(acc_detector_distance_handle_t *distance_handle,
                    acc_detector_distance_config_t *distance_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer,
                    void                           *calibration_buffer);


static void set_config(acc_detector_distance_config_t *distance_config);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_detector_distance_config_t *distance_config        = NULL;
	acc_detector_distance_handle_t *distance_handle        = NULL;
	acc_sensor_t                   *sensor                 = NULL;
	void                           *buffer                 = NULL;
	void                           *calibration_buffer     = NULL;
	uint32_t                       buffer_size             = 0U;
	uint32_t                       calibration_buffer_size = 0U;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	distance_config = acc_detector_distance_config_create();
	if (distance_config == NULL)
	{
		printf("acc_detector_distance_config_create() failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	set_config(distance_config);

	uint32_t sleep_time_ms = (uint32_t)(1000.0f / DEFAULT_UPDATE_RATE);

	acc_integration_set_periodic_wakeup(sleep_time_ms);

	distance_handle = acc_detector_distance_create(distance_config);
	if (distance_handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	if (!acc_detector_distance_get_buffer_sizes(distance_handle, &buffer_size, &calibration_buffer_size))
	{
		printf("acc_detector_distance_get_buffer_size() failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	buffer = acc_integration_mem_alloc(buffer_size);
	if (buffer == NULL)
	{
		printf("buffer allocation failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	calibration_buffer = acc_integration_mem_alloc(calibration_buffer_size);
	if (calibration_buffer == NULL)
	{
		printf("buffer allocation failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	// Calibrate sensor
	bool             status;
	bool             cal_complete = false;
	acc_cal_result_t sensor_cal_result;

	do
	{
		status = acc_sensor_calibrate(sensor, &cal_complete, &sensor_cal_result, buffer, buffer_size);

		if (status && !cal_complete)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status && !cal_complete);

	if (!status)
	{
		printf("acc_sensor_calibrate() failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	/* Reset sensor after calibration by disabling/enabling it */
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	// Calibrate detector
	bool done = false;

	do
	{
		status = acc_detector_distance_calibrate(sensor,
		                                         distance_handle,
		                                         &sensor_cal_result,
		                                         buffer,
		                                         buffer_size,
		                                         calibration_buffer,
		                                         calibration_buffer_size,
		                                         &done);
	} while (status && !done && acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS));

	if (!status)
	{
		printf("acc_detector_distance_calibrate() failed\n");
		cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_disable(SENSOR_ID);

	while (true)
	{
		acc_detector_distance_result_t result;

		acc_hal_integration_sensor_enable(SENSOR_ID);

		bool result_available = false;

		do
		{
			if (!acc_detector_distance_prepare(distance_handle, distance_config, sensor, &sensor_cal_result, buffer,
			                                   buffer_size))
			{
				printf("acc_detector_distance_prepare() failed\n");
				cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
				return EXIT_FAILURE;
			}

			if (!acc_sensor_measure(sensor))
			{
				printf("acc_sensor_measure failed\n");
				cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
				return EXIT_FAILURE;
			}

			if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
			{
				printf("Sensor interrupt timeout\n");
				cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
				return EXIT_FAILURE;
			}

			if (!acc_sensor_read(sensor, buffer, buffer_size))
			{
				printf("acc_sensor_read failed\n");
				cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
				return EXIT_FAILURE;
			}

			if (!acc_detector_distance_process(distance_handle, buffer, calibration_buffer, &result_available, &result))
			{
				printf("acc_detector_distance_process failed\n");
				cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);
				return EXIT_FAILURE;
			}
		} while (!result_available);

		acc_hal_integration_sensor_disable(SENSOR_ID);

		print_result(&result);

		acc_integration_sleep_until_periodic_wakeup();
	}

	cleanup(distance_handle, distance_config, sensor, buffer, calibration_buffer);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(acc_detector_distance_handle_t *distance_handle,
                    acc_detector_distance_config_t *distance_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer,
                    void                           *calibration_buffer)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (distance_config != NULL)
	{
		acc_detector_distance_config_destroy(distance_config);
	}

	if (distance_handle != NULL)
	{
		acc_detector_distance_destroy(distance_handle);
	}

	if (sensor != NULL)
	{
		acc_sensor_destroy(sensor);
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}

	if (calibration_buffer != NULL)
	{
		acc_integration_mem_free(calibration_buffer);
	}
}


static void set_config(acc_detector_distance_config_t *distance_config)
{
	// Add configuration of the detector here

	acc_detector_distance_config_start_set(distance_config, 0.25f);
	acc_detector_distance_config_end_set(distance_config, 3.0f);
}


static void print_result(acc_detector_distance_result_t *result)
{
	printf("%d detected distances", result->num_distances);
	if (result->num_distances > 0)
	{
		printf(": ");

		for (uint8_t i = 0; i < result->num_distances; i++)
		{
			printf("%" PRIfloat "m ", ACC_LOG_FLOAT_TO_INTEGER(result->distances[i]));
		}
	}

	printf("\n");
}
