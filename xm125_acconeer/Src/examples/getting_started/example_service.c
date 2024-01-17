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


/** \example example_service.c
 * @brief This is an example on how the Service API can be used
 * @n
 * The example executes as follows:
 *   - Create a configuration
 *   - Create a processing instance using the previously created configuration
 *   - Create a sensor instance
 *   - Prepare a sensor
 *   - Perform a sensor measurement and read out the data
 *   - Process the measurement
 *   - Check 'calibration_needed' indication
 *   - Destroy the sensor instance
 *   - Destroy the processing instance
 *   - Destroy the configuration
 */

#define SENSOR_ID          (1U)
#define SENSOR_TIMEOUT_MS  (1000U)
#define MAX_DATA_ENTRY_LEN (15U) // "-32000+-32000i" + zero termination

static void set_config(acc_config_t *config);


static bool do_sensor_calibration_and_prepare(acc_sensor_t *sensor, acc_config_t *config, void *buffer, uint32_t buffer_size);


static void print_data(acc_int16_complex_t *data, uint16_t data_length);


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

	if (!do_sensor_calibration_and_prepare(sensor, config, buffer, buffer_size))
	{
		printf("do_sensor_calibration_and_prepare() failed\n");
		acc_sensor_status(sensor);
		cleanup(config, processing, sensor, buffer);
		return EXIT_FAILURE;
	}

	const uint32_t update_count = 5U;

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
			printf("The current calibration is not valid for the current temperature.\n");
			printf("The sensor needs to be re-calibrated.\n");

			if (!do_sensor_calibration_and_prepare(sensor, config, buffer, buffer_size))
			{
				printf("do_sensor_calibration_and_prepare() failed\n");
				acc_sensor_status(sensor);
				cleanup(config, processing, sensor, buffer);
				return EXIT_FAILURE;
			}

			printf("The sensor was successfully re-calibrated.\n");
		}
		else
		{
			print_data(proc_result.frame, proc_meta.frame_data_length);
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


static bool do_sensor_calibration_and_prepare(acc_sensor_t *sensor, acc_config_t *config, void *buffer, uint32_t buffer_size)
{
	bool             status       = false;
	bool             cal_complete = false;
	acc_cal_result_t cal_result;
	const uint16_t   calibration_retries = 1U;

	// Random disturbances may cause the calibration to fail. At failure, retry at least once.
	for (uint16_t i = 0; !status && (i <= calibration_retries); i++)
	{
		// Reset sensor before calibration by disabling/enabling it
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);

		do
		{
			status = acc_sensor_calibrate(sensor, &cal_complete, &cal_result, buffer, buffer_size);

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

		status = acc_sensor_prepare(sensor, config, &cal_result, buffer, buffer_size);
	}

	return status;
}


static void print_data(acc_int16_complex_t *data, uint16_t data_length)
{
	printf("Processed data:\n");
	char buffer[MAX_DATA_ENTRY_LEN];

	for (uint16_t i = 0; i < data_length; i++)
	{
		if ((i > 0) && ((i % 8) == 0))
		{
			printf("\n");
		}

		snprintf(buffer, sizeof(buffer), "%" PRIi16 "+%" PRIi16 "i", data[i].real, data[i].imag);

		printf("%14s ", buffer);
	}

	printf("\n");
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
