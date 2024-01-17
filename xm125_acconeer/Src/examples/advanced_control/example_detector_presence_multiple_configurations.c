// Copyright (c) Acconeer AB, 2022-2023
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
#include "acc_detector_presence.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"

/** \example example_detector_presence_multiple_configurations.c
 * @brief This is an example on how the Detector Presence API can be used
 * @n
 * The example executes as follows:
 *   - Create a first presence configuration
 *   - Create a second presence configuration
 *   - Create a sensor instance
 *   - Calibrate the sensor
 *   - Loop forever:
 *     - Loop configurations (i):
 *       - Create a detector instance with the i:th configuration
 *       - Prepare the detector with the i:th configuration
 *       - Loop nbr iterations per configuration:
 *         - Perform a sensor measurement and read out the data
 *         - Process the measurement and get detector result
 *       - Destroy the detector instance
 *   - Destroy the configuration
 *   - Destroy the sensor instance
 */

#define SENSOR_ID                 (1U)
#define SENSOR_TIMEOUT_MS         (1000U)
#define NBR_CONFIGS               (2U)
#define NBR_ITERATIONS_PER_CONFIG (20U)


typedef struct example_config
{
	char  *name;
	float start;
	float end;
	float frame_rate;
	bool  reset_filters_on_prepare;
} example_config_t;


static void print_result(acc_detector_presence_result_t result, const char *config_name);


static void cleanup(acc_detector_presence_handle_t **presence_handle,
                    acc_detector_presence_config_t **presence_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer,
                    uint32_t                       nbr_configs);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_detector_presence_config_t   *presence_config[NBR_CONFIGS] = { NULL };
	acc_detector_presence_handle_t   *presence_handle[NBR_CONFIGS] = { NULL };
	acc_detector_presence_metadata_t metadata[NBR_CONFIGS];
	acc_sensor_t                     *sensor     = NULL;
	void                             *buffer     = NULL;
	uint32_t                         buffer_size = 0U;

	example_config_t example_configurations[] =
	{
		{
			.name       = "zone1",
			.start      = 1.0f,
			.end        = 2.0f,
			.frame_rate = 5.0f,
			/**
			 * Detector filters and states are reset on prepare (reconfiguration) to avoid risk of false detections
			 * when switching back to a zone.
			 */
			.reset_filters_on_prepare = true,
		},
		{
			.name       = "zone2",
			.start      = 0.2f,
			.end        = 1.0f,
			.frame_rate = 10.0f,
			/**
			 * Detector filters and states are reset on prepare (reconfiguration) to avoid risk of false detections
			 * when switching back to a zone.
			 */
			.reset_filters_on_prepare = true,
		},
	};

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	for (uint32_t cfg = 0U; cfg < NBR_CONFIGS; cfg++)
	{
		presence_config[cfg] = acc_detector_presence_config_create();
		if (presence_config[cfg] == NULL)
		{
			printf("acc_detector_presence_config_create() failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
			return EXIT_FAILURE;
		}

		acc_detector_presence_config_start_set(presence_config[cfg], example_configurations[cfg].start);
		acc_detector_presence_config_end_set(presence_config[cfg], example_configurations[cfg].end);
		acc_detector_presence_config_frame_rate_set(presence_config[cfg], example_configurations[cfg].frame_rate);
		acc_detector_presence_config_reset_filters_on_prepare_set(presence_config[cfg], example_configurations[cfg].reset_filters_on_prepare);

		uint32_t current_buffer_size = 0;

		presence_handle[cfg] = acc_detector_presence_create(presence_config[cfg], &metadata[cfg]);
		if (presence_handle[cfg] == NULL)
		{
			printf("acc_detector_presence_create() failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
			return EXIT_FAILURE;
		}

		if (!acc_detector_presence_get_buffer_size(presence_handle[cfg], &current_buffer_size))
		{
			printf("acc_detector_presence_get_buffer_size() failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
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
		cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
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
		cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
		return EXIT_FAILURE;
	}

	// Reset sensor after calibration by disabling it
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	while (true)
	{
		for (uint32_t cfg = 0U; cfg < NBR_CONFIGS; cfg++)
		{
			acc_detector_presence_result_t result;

			if (!acc_detector_presence_prepare(presence_handle[cfg], presence_config[cfg], sensor,
			                                   &cal_result, buffer, buffer_size))
			{
				printf("acc_detector_presence_prepare() failed\n");
				cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
				return EXIT_FAILURE;
			}

			for (uint32_t i = 0U; i < NBR_ITERATIONS_PER_CONFIG; i++)
			{
				if (!acc_sensor_measure(sensor))
				{
					printf("acc_sensor_measure failed\n");
					cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
					return EXIT_FAILURE;
				}

				if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
				{
					printf("Sensor interrupt timeout\n");
					cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
					return EXIT_FAILURE;
				}

				if (!acc_sensor_read(sensor, buffer, buffer_size))
				{
					printf("acc_sensor_read failed\n");
					cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
					return EXIT_FAILURE;
				}

				if (!acc_detector_presence_process(presence_handle[cfg], buffer, &result))
				{
					printf("acc_detector_presence_process failed\n");
					cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);
					return EXIT_FAILURE;
				}

				/* The depthwise inter and intra score arrays that are part of the 'result',
				 * points to memory inside the 'buffer'. So they need to be handled before
				 * the 'buffer' is used again (for example in @ref acc_sensor_read or
				 * @ref acc_detector_presence_process).
				 */
				print_result(result, example_configurations[cfg].name);
			}
		}
	}

	cleanup(presence_handle, presence_config, sensor, buffer, NBR_CONFIGS);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(acc_detector_presence_handle_t **presence_handle,
                    acc_detector_presence_config_t **presence_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer,
                    uint32_t                       nbr_configs)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	for (uint32_t cfg = 0U; cfg < nbr_configs; cfg++)
	{
		if (presence_config[cfg] != NULL)
		{
			acc_detector_presence_config_destroy(presence_config[cfg]);
			presence_config[cfg] = NULL;
		}

		if (presence_handle[cfg] != NULL)
		{
			acc_detector_presence_destroy(presence_handle[cfg]);
			presence_handle[cfg] = NULL;
		}
	}

	if (sensor != NULL)
	{
		acc_sensor_destroy(sensor);
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}
}


static void print_result(acc_detector_presence_result_t result, const char *config_name)
{
	if (result.presence_detected)
	{
		printf("Motion\n");
	}
	else
	{
		printf("No motion\n");
	}

	// Score and distance are multiplied by 1000 to avoid printing floats
	printf("%s - Intra presence score: %d, Inter presence score: %d, Distance (mm): %d\n",
	       config_name,
	       (int)(result.intra_presence_score * 1000.0f),
	       (int)(result.inter_presence_score * 1000.0f),
	       (int)(result.presence_distance * 1000.0f));

	printf("%s - Depthwise Intra Presence Scores: \t", config_name);
	for (uint16_t i = 0; i < result.depthwise_presence_scores_length; i++)
	{
		// Score is multiplied by 1000 to avoid printing floats
		printf("%5i", (int)(result.depthwise_intra_presence_scores[i] * 1000));
	}

	printf("\n");
	printf("%s - Depthwise Inter Presence Scores: \t", config_name);
	for (uint16_t i = 0; i < result.depthwise_presence_scores_length; i++)
	{
		// Score is multiplied by 1000 to avoid printing floats
		printf("%5i", (int)(result.depthwise_inter_presence_scores[i] * 1000));
	}

	printf("\n");
}
