// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdint.h>
#include <stdio.h>

#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_processing.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"
#include "acc_version.h"

#include "ref_app_breathing.h"

typedef enum
{
	BREATHING_PRESET_NONE,
	BREATHING_PRESET_SITTING,
	BREATHING_PRESET_INFANT,
} breathing_preset_t;

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

#define DEFAULT_PRESET_CONFIG BREATHING_PRESET_SITTING


static void cleanup(ref_app_breathing_handle_t *handle, ref_app_breathing_config_t *config, acc_sensor_t *sensor, void *buffer);


static void set_config(ref_app_breathing_config_t *config, breathing_preset_t preset);


static bool sensor_calibration(acc_sensor_t *sensor, acc_cal_result_t *sensor_cal_result, void *buffer, uint32_t buffer_size);


static bool measure(acc_sensor_t *sensor, void *buffer, uint32_t buffer_size);


static bool enter_hibernate(acc_sensor_t *sensor);


static bool exit_hibernate(acc_sensor_t *sensor);


static void print_app_state(ref_app_breathing_result_t *result);


static void print_result(ref_app_breathing_result_t *result, ref_app_breathing_app_state_t prev_app_state);


static bool handle_indications(ref_app_breathing_handle_t *handle, ref_app_breathing_config_t *config, acc_sensor_t *sensor,
                               acc_cal_result_t *sensor_cal_result, void *buffer, uint32_t buffer_size,
                               acc_detector_presence_result_t *presence_result);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	ref_app_breathing_config_t    *config = NULL;
	ref_app_breathing_handle_t    *handle = NULL;
	acc_sensor_t                  *sensor = NULL;
	acc_cal_result_t              sensor_cal_result;
	void                          *buffer        = NULL;
	uint32_t                      buffer_size    = 0U;
	ref_app_breathing_app_state_t prev_app_state = (ref_app_breathing_app_state_t)0U;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	config = ref_app_breathing_config_create();

	if (config == NULL)
	{
		printf("Failed to create config\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	set_config(config, DEFAULT_PRESET_CONFIG);

	handle = ref_app_breathing_create(config);

	if (handle == NULL)
	{
		printf("Failed to create handle\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!ref_app_breathing_get_buffer_size(handle, &buffer_size))
	{
		printf("ref_app_breathing_get_buffer_size() failed\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	buffer = acc_integration_mem_alloc(buffer_size);
	if (buffer == NULL)
	{
		printf("Failed to allocate buffer\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	float    frame_rate    = acc_detector_presence_config_frame_rate_get(config->presence_config);
	uint32_t sleep_time_ms = (uint32_t)(1000.0f / frame_rate);
	acc_integration_set_periodic_wakeup(sleep_time_ms);

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!sensor_calibration(sensor, &sensor_cal_result, buffer, buffer_size))
	{
		printf("Sensor calibration failed\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!ref_app_breathing_prepare(handle, config, sensor, &sensor_cal_result, buffer, buffer_size))
	{
		printf("ref_app_breathing_prepare() failed\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	ref_app_breathing_result_t result = { 0 };

	if (!enter_hibernate(sensor))
	{
		printf("enter_hibernate failed\n");
		cleanup(handle, config, sensor, buffer);
		return EXIT_FAILURE;
	}

	while (true)
	{
		if (!measure(sensor, buffer, buffer_size))
		{
			cleanup(handle, config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!ref_app_breathing_process(handle, buffer, &result))
		{
			printf("ref_app_breathing_process() failed\n");
			cleanup(handle, config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!handle_indications(handle, config, sensor, &sensor_cal_result, buffer, buffer_size, &result.presence_result))
		{
			cleanup(handle, config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!result.presence_result.processing_result.calibration_needed)
		{
			print_result(&result, prev_app_state);
			prev_app_state = result.app_state;
		}

		acc_integration_sleep_until_periodic_wakeup();
	}

	cleanup(handle, config, sensor, buffer);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(ref_app_breathing_handle_t *handle, ref_app_breathing_config_t *config, acc_sensor_t *sensor, void *buffer)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (config != NULL)
	{
		ref_app_breathing_config_destroy(config);
	}

	if (sensor != NULL)
	{
		acc_sensor_destroy(sensor);
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}

	ref_app_breathing_destroy(handle);
}


static void set_config(ref_app_breathing_config_t *config, breathing_preset_t preset)
{
	acc_detector_presence_config_t *presence_config = config->presence_config;

	switch (preset)
	{
		case BREATHING_PRESET_NONE:
			// Add custom configuration of the reference application here
			break;
		case BREATHING_PRESET_SITTING:
			acc_detector_presence_config_end_set(presence_config, 1.5f);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 6.0f);
			break;
		case BREATHING_PRESET_INFANT:
			acc_detector_presence_config_end_set(presence_config, 1.0f);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 4.0f);
			break;
	}

	acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);
}


static bool sensor_calibration(acc_sensor_t *sensor, acc_cal_result_t *sensor_cal_result, void *buffer, uint32_t buffer_size)
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
			status = acc_sensor_calibrate(sensor, &cal_complete, sensor_cal_result, buffer, buffer_size);

			if (status && !cal_complete)
			{
				status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
			}
		} while (status && !cal_complete);
	}

	if (status)
	{
		/* Reset sensor after calibration by disabling/enabling it */
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);
	}
	else
	{
		printf("acc_sensor_calibrate() failed\n");
		acc_sensor_status(sensor);
	}

	return status;
}


static bool measure(acc_sensor_t *sensor, void *buffer, uint32_t buffer_size)
{
	if (!exit_hibernate(sensor))
	{
		printf("exit_hibernate failed\n");
		return EXIT_FAILURE;
	}

	if (!acc_sensor_measure(sensor))
	{
		printf("acc_sensor_measure failed\n");
		acc_sensor_status(sensor);
		return false;
	}

	if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
	{
		printf("Sensor interrupt timeout\n");
		acc_sensor_status(sensor);
		return false;
	}

	if (!acc_sensor_read(sensor, buffer, buffer_size))
	{
		printf("acc_sensor_read() failed\n");
		acc_sensor_status(sensor);
		return false;
	}

	if (!enter_hibernate(sensor))
	{
		printf("enter_hibernate failed\n");
		return EXIT_FAILURE;
	}

	return true;
}


static bool enter_hibernate(acc_sensor_t *sensor)
{
	bool status = true;

	if (!acc_sensor_hibernate_on(sensor))
	{
		printf("acc_sensor_hibernate_on failed\n");
		acc_sensor_status(sensor);
		status = false;
	}

	acc_hal_integration_sensor_disable(SENSOR_ID);
	return status;
}


static bool exit_hibernate(acc_sensor_t *sensor)
{
	bool status = true;

	acc_hal_integration_sensor_enable(SENSOR_ID);
	if (!acc_sensor_hibernate_off(sensor))
	{
		printf("acc_sensor_hibernate_off failed\n");
		acc_sensor_status(sensor);
		status = false;
	}

	return status;
}


static void print_app_state(ref_app_breathing_result_t *result)
{
	switch (result->app_state)
	{
		case REF_APP_BREATHING_APP_STATE_INIT:
			break;
		case REF_APP_BREATHING_APP_STATE_NO_PRESENCE:
			printf("NO_PRESENCE_DETECTED\n");
			break;
		case REF_APP_BREATHING_APP_STATE_INTRA_PRESENCE:
			printf("INTRA_PRESENCE_DETECTED\n");
			break;
		case REF_APP_BREATHING_APP_STATE_DETERMINE_DISTANCE:
			printf("DETERMINE_DISTANCE_ESTIMATE\n");
			break;
		case REF_APP_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE:
			printf("ESTIMATE_BREATHING_RATE\n");
			break;
		default:
			break;
	}
}


static void print_result(ref_app_breathing_result_t *result, ref_app_breathing_app_state_t prev_app_state)
{
	if (prev_app_state != result->app_state)
	{
		print_app_state(result);
	}

	if (result->result_ready)
	{
		printf("Breaths: %" PRIu16 " bpm\n", (uint16_t)result->breathing_rate);
	}
}


static bool handle_indications(ref_app_breathing_handle_t *handle, ref_app_breathing_config_t *config, acc_sensor_t *sensor,
                               acc_cal_result_t *sensor_cal_result, void *buffer, uint32_t buffer_size,
                               acc_detector_presence_result_t *presence_result)
{
	if (presence_result->processing_result.data_saturated)
	{
		printf("Data saturated. The detector result is not reliable.\n");
	}

	if (presence_result->processing_result.frame_delayed)
	{
		printf("Frame delayed. Could not read data fast enough.\n");
		printf("Try lowering the frame rate or call 'acc_sensor_read' more frequently.\n");
	}

	if (presence_result->processing_result.calibration_needed)
	{
		printf("Sensor recalibration needed ... \n");

		if (!exit_hibernate(sensor))
		{
			printf("exit_hibernate failed\n");
			return EXIT_FAILURE;
		}

		if (!sensor_calibration(sensor, sensor_cal_result, buffer, buffer_size))
		{
			printf("Sensor calibration failed\n");
			return false;
		}

		printf("Sensor recalibration done!\n");

		// Before measuring again, the sensor needs to be prepared through the detector
		if (!ref_app_breathing_prepare(handle, config, sensor, sensor_cal_result, buffer, buffer_size))
		{
			printf("acc_detector_presence_prepare() failed\n");
			return false;
		}

		if (!enter_hibernate(sensor))
		{
			printf("enter_hibernate failed\n");
			return EXIT_FAILURE;
		}
	}

	return true;
}
