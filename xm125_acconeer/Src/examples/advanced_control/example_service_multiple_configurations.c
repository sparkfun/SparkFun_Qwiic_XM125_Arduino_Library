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


/** \example example_service_multiple_configurations.c
 * @brief This is an example on how the Service API can be used with multiple configurations
 * @n
 * The example executes as follows:
 *   - Create a first configuration
 *   - Create a second configuration
 *   - Create a processing instance using the first configuration
 *   - Create a second processing instance using the second configuration
 *   - Create a sensor instance
 *   - Prepare a sensor for the first configuration
 *   - Perform a sensor measurement and read out the data
 *   - Process the measurement with the first processing instance
 *   - Prepare a sensor for the second configuration
 *   - Perform a sensor measurement and read out the data
 *   - Process the measurement with the second processing instance
 *   - Destroy the sensor instance
 *   - Destroy the first processing instance
 *   - Destroy the second processing instance
 *   - Destroy the first configuration
 *   - Destroy the second configuration
 */

#define SENSOR_ID          (1U)
#define SENSOR_TIMEOUT_MS  (1000U)
#define MAX_DATA_ENTRY_LEN (15U) // "-32000+-32000i" + zero termination
#define NBR_CONFIGS        (2U)


typedef struct example_config
{
	int16_t  start_point;
	uint16_t num_points;
	uint16_t sweeps_per_frame;
} example_config_t;


static void print_data(acc_int16_complex_t *data, uint16_t data_length, uint32_t cfg);


static void cleanup(acc_config_t **config, acc_processing_t **processing,
                    acc_sensor_t *sensor, void *buffer, uint32_t nbr_configs);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	acc_config_t              *config[NBR_CONFIGS]     = { NULL };
	acc_processing_t          *processing[NBR_CONFIGS] = { NULL };
	acc_sensor_t              *sensor                  = NULL;
	void                      *buffer                  = NULL;
	uint32_t                  buffer_size              = 0;
	acc_processing_metadata_t proc_meta[NBR_CONFIGS];
	acc_processing_result_t   proc_result;

	example_config_t example_configurations[] =
	{
		{
			.start_point      = 100,
			.num_points       = 100,
			.sweeps_per_frame = 1,
		},
		{
			.start_point      = 120,
			.num_points       = 120,
			.sweeps_per_frame = 3,
		}
	};

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	for (uint32_t cfg = 0U; cfg < NBR_CONFIGS; cfg++)
	{
		config[cfg] = acc_config_create();
		if (config[cfg] == NULL)
		{
			printf("acc_config_create() failed\n");
			cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
			return EXIT_FAILURE;
		}

		acc_config_start_point_set(config[cfg], example_configurations[cfg].start_point);
		acc_config_num_points_set(config[cfg], example_configurations[cfg].num_points);
		acc_config_sweeps_per_frame_set(config[cfg], example_configurations[cfg].sweeps_per_frame);

		processing[cfg] = acc_processing_create(config[cfg], &proc_meta[cfg]);
		if (processing[cfg] == NULL)
		{
			printf("acc_processing_create() failed\n");
			cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
			return EXIT_FAILURE;
		}

		uint32_t current_buffer_size = 0;

		if (!acc_rss_get_buffer_size(config[cfg], &current_buffer_size))
		{
			printf("acc_rss_get_buffer_size() failed\n");
			cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
			return EXIT_FAILURE;
		}

		if (buffer_size < current_buffer_size)
		{
			buffer_size = current_buffer_size;
		}
	}

	buffer = acc_integration_mem_alloc(buffer_size);
	if (buffer == NULL)
	{
		printf("buffer allocation failed\n");
		cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
		return EXIT_FAILURE;
	}

	bool             status;
	bool             cal_complete = false;
	acc_cal_result_t cal_result;

	do
	{
		status = acc_sensor_calibrate(sensor, &cal_complete, &cal_result, buffer, buffer_size);

		if (status && !cal_complete)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status && !cal_complete);

	if (!status)
	{
		printf("acc_sensor_calibrate() failed\n");
		acc_sensor_status(sensor);
		cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
		return EXIT_FAILURE;
	}

	// Reset sensor after calibration by disabling it
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	for (uint32_t i = 0U; i < 5U; i++)
	{
		for (uint32_t cfg = 0U; cfg < NBR_CONFIGS; cfg++)
		{
			if (!acc_sensor_prepare(sensor, config[cfg], &cal_result, buffer, buffer_size))
			{
				printf("acc_sensor_prepare() failed\n");
				acc_sensor_status(sensor);
				cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
				return EXIT_FAILURE;
			}

			if (!acc_sensor_measure(sensor))
			{
				printf("acc_sensor_measure failed\n");
				acc_sensor_status(sensor);
				cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
				return EXIT_FAILURE;
			}

			if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
			{
				printf("Sensor interrupt timeout\n");
				acc_sensor_status(sensor);
				cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
				return EXIT_FAILURE;
			}

			if (!acc_sensor_read(sensor, buffer, buffer_size))
			{
				printf("acc_sensor_read failed\n");
				acc_sensor_status(sensor);
				cleanup(config, processing, sensor, buffer, NBR_CONFIGS);
				return EXIT_FAILURE;
			}

			acc_processing_execute(processing[cfg], buffer, &proc_result);

			print_data(proc_result.frame, proc_meta[cfg].frame_data_length, cfg);
		}
	}

	cleanup(config, processing, sensor, buffer, NBR_CONFIGS);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(acc_config_t **config, acc_processing_t **processing,
                    acc_sensor_t *sensor, void *buffer, uint32_t nbr_configs)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (sensor != NULL)
	{
		acc_sensor_destroy(sensor);
	}

	for (uint32_t cfg = 0U; cfg < nbr_configs; cfg++)
	{
		if (processing[cfg] != NULL)
		{
			acc_processing_destroy(processing[cfg]);
		}

		if (config[cfg] != NULL)
		{
			acc_config_destroy(config[cfg]);
		}
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}
}


static void print_data(acc_int16_complex_t *data, uint16_t data_length, uint32_t cfg)
{
	printf("Processed data for config %" PRIu32 ":\n", cfg);
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
