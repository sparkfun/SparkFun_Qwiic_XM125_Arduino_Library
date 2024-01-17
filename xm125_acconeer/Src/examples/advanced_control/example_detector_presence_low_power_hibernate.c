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


/** \example example_detector_presence_low_power_hibernate.c
 * @brief This is an example on how to put the sensor in hibernation and the system in a low power state between measurements
 * @n
 * The example executes as follows:
 *   - Create a presence configuration
 *   - Create a sensor instance
 *   - Enable sensor
 *   - Create a detector instance
 *   - Calibrate the sensor
 *   - Prepare the detector
 *   - Enter hibernation
 *   - Loop forever:
 *     - Exit hibernation
 *     - Perform a sensor measurement and read out the data
 *     - Enter hibernation
 *     - Process the measurement and get detector result
 *     - Put the system in deep sleep for a specified amount of time
 *   - Destroy the configuration
 *   - Destroy the detector instance
 *   - Destroy the sensor instance
 */


typedef enum
{
	PRESENCE_PRESET_CONFIG_NONE = 0,
	PRESENCE_PRESET_CONFIG_SHORT_RANGE,
	PRESENCE_PRESET_CONFIG_MEDIUM_RANGE,
	PRESENCE_PRESET_CONFIG_LONG_RANGE,
	PRESENCE_PRESET_CONFIG_LOW_POWER_WAKEUP,
} presence_preset_config_t;


#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

#define DEFAULT_PRESET_CONFIG PRESENCE_PRESET_CONFIG_MEDIUM_RANGE


static void print_result(acc_detector_presence_result_t result);


static void cleanup(acc_detector_presence_handle_t *presence_handle,
                    acc_detector_presence_config_t *presence_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer);


static void set_config(acc_detector_presence_config_t *presence_config, presence_preset_config_t preset);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_detector_presence_config_t   *presence_config = NULL;
	acc_detector_presence_handle_t   *presence_handle = NULL;
	acc_detector_presence_metadata_t metadata;
	acc_sensor_t                     *sensor     = NULL;
	void                             *buffer     = NULL;
	uint32_t                         buffer_size = 0U;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	presence_config = acc_detector_presence_config_create();
	if (presence_config == NULL)
	{
		printf("acc_detector_presence_config_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	set_config(presence_config, DEFAULT_PRESET_CONFIG);

	uint32_t sleep_time_ms = (uint32_t)(1000.0f / acc_detector_presence_config_frame_rate_get(presence_config));

	acc_integration_set_periodic_wakeup(sleep_time_ms);

	presence_handle = acc_detector_presence_create(presence_config, &metadata);
	if (presence_handle == NULL)
	{
		printf("acc_detector_presence_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!acc_detector_presence_get_buffer_size(presence_handle, &buffer_size))
	{
		printf("acc_detector_presence_get_buffer_size() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	buffer = acc_integration_mem_alloc(buffer_size);
	if (buffer == NULL)
	{
		printf("buffer allocation failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
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
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	// Reset sensor after calibration by disabling it
	acc_hal_integration_sensor_disable(SENSOR_ID);

	acc_hal_integration_sensor_enable(SENSOR_ID);

	if (!acc_detector_presence_prepare(presence_handle, presence_config, sensor, &cal_result, buffer, buffer_size))
	{
		printf("acc_detector_presence_prepare() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!acc_sensor_hibernate_on(sensor))
	{
		printf("acc_sensor_hibernate_on failed\n");
		acc_sensor_status(sensor);
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_disable(SENSOR_ID);

	while (true)
	{
		acc_detector_presence_result_t result;

		acc_hal_integration_sensor_enable(SENSOR_ID);
		if (!acc_sensor_hibernate_off(sensor))
		{
			printf("acc_sensor_hibernate_off failed\n");
			acc_sensor_status(sensor);
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_sensor_measure(sensor))
		{
			printf("acc_sensor_measure failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("Sensor interrupt timeout\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_sensor_read(sensor, buffer, buffer_size))
		{
			printf("acc_sensor_read failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_sensor_hibernate_on(sensor))
		{
			printf("acc_sensor_hibernate_on failed\n");
			acc_sensor_status(sensor);
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		acc_hal_integration_sensor_disable(SENSOR_ID);

		if (!acc_detector_presence_process(presence_handle, buffer, &result))
		{
			printf("acc_detector_presence_process failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		print_result(result);

		acc_integration_sleep_until_periodic_wakeup();
	}

	cleanup(presence_handle, presence_config, sensor, buffer);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(acc_detector_presence_handle_t *presence_handle,
                    acc_detector_presence_config_t *presence_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (presence_config != NULL)
	{
		acc_detector_presence_config_destroy(presence_config);
	}

	if (presence_handle != NULL)
	{
		acc_detector_presence_destroy(presence_handle);
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


static void set_config(acc_detector_presence_config_t *presence_config, presence_preset_config_t preset)
{
	switch (preset)
	{
		case PRESENCE_PRESET_CONFIG_NONE:
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);
			// Add configuration of the detector here
			break;

		case PRESENCE_PRESET_CONFIG_SHORT_RANGE:
			acc_detector_presence_config_start_set(presence_config, 0.06f);
			acc_detector_presence_config_end_set(presence_config, 1.0f);
			acc_detector_presence_config_auto_step_length_set(presence_config, true);
			acc_detector_presence_config_auto_profile_set(presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(presence_config, 16);
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 16);
			acc_detector_presence_config_frame_rate_set(presence_config, 10.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.4f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(presence_config, false);

			break;

		case PRESENCE_PRESET_CONFIG_MEDIUM_RANGE:
			acc_detector_presence_config_start_set(presence_config, 0.3f);
			acc_detector_presence_config_end_set(presence_config, 2.5f);
			acc_detector_presence_config_auto_step_length_set(presence_config, true);
			acc_detector_presence_config_auto_profile_set(presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(presence_config, 32);
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 16);
			acc_detector_presence_config_frame_rate_set(presence_config, 12.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.3f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(presence_config, false);

			break;

		case PRESENCE_PRESET_CONFIG_LONG_RANGE:
			acc_detector_presence_config_start_set(presence_config, 5.0f);
			acc_detector_presence_config_end_set(presence_config, 7.5f);
			acc_detector_presence_config_auto_step_length_set(presence_config, true);
			acc_detector_presence_config_auto_profile_set(presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(presence_config, 128);
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 32);
			acc_detector_presence_config_frame_rate_set(presence_config, 12.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.2f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 0.8f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(presence_config, false);

			break;

		case PRESENCE_PRESET_CONFIG_LOW_POWER_WAKEUP:
			acc_detector_presence_config_start_set(presence_config, 0.725f);
			acc_detector_presence_config_end_set(presence_config, 1.505f);
			acc_detector_presence_config_auto_step_length_set(presence_config, true);
			acc_detector_presence_config_auto_profile_set(presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(presence_config, 8);
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 16);
			acc_detector_presence_config_frame_rate_set(presence_config, 1.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.5f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(presence_config, false);

			break;
	}
}


static void print_result(acc_detector_presence_result_t result)
{
	if (result.presence_detected)
	{
		printf("Motion\n");
	}
	else
	{
		printf("No motion\n");
	}

	// Score and distance is multiplied by 1000 to avoid printing floats
	printf("Intra presence score: %d, Inter presence score: %d, Distance (mm): %d\n",
	       (int)(result.intra_presence_score * 1000.0f),
	       (int)(result.inter_presence_score * 1000.0f),
	       (int)(result.presence_distance * 1000.0f));
}
