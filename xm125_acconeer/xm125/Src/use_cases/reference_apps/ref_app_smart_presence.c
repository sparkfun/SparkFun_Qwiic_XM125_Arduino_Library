// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_detector_presence.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"


typedef enum
{
	SMART_PRESENCE_PRESET_CONFIG_NONE = 0,
	SMART_PRESENCE_PRESET_CONFIG_SHORT_RANGE,
	SMART_PRESENCE_PRESET_CONFIG_MEDIUM_RANGE,
	SMART_PRESENCE_PRESET_CONFIG_LONG_RANGE,
	SMART_PRESENCE_PRESET_CONFIG_CEILING,
	SMART_PRESENCE_PRESET_CONFIG_LOW_POWER_WAKEUP,
} smart_presence_preset_config_t;

typedef enum
{
	SMART_PRESENCE_MODE_WAKEUP = 0,
	SMART_PRESENCE_MODE_NOMINAL,
} smart_presence_mode_t;


#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

#define DEFAULT_PRESET_CONFIG SMART_PRESENCE_PRESET_CONFIG_MEDIUM_RANGE

/**
 * @brief Smart presence reference application results container for each zone
 */
typedef struct
{
	/**
	 * The upper zone limit in m
	 */
	float zone_limit;
	/**
	 * Detection result
	 */
	bool zone_detection;
	/**
	 * Slow motion presence detection result
	 */
	bool inter_zone_detection;
	/**
	 * Fast motion presence detection result
	 */
	bool intra_zone_detection;
} acc_smart_presence_zone_result_t;

typedef struct
{
	uint32_t                       num_zones;
	uint32_t                       num_zones_for_wakeup;
	acc_detector_presence_config_t *presence_config;
} acc_ref_app_smart_presence_wakeup_config_t;

typedef struct
{
	uint32_t                       num_zones;
	acc_detector_presence_config_t *presence_config;
} acc_ref_app_smart_presence_zone_config_t;

typedef struct
{
	bool                                       use_wakeup_mode;
	bool                                       show_all_detected_zones;
	acc_ref_app_smart_presence_wakeup_config_t wakeup_config;
	acc_ref_app_smart_presence_zone_config_t   nominal_config;
} acc_ref_app_smart_presence_config_t;


/**
 * @brief Smart presence reference application results container
 */
typedef struct
{
	/**
	 * true if presence was detected in any zone, false otherwise
	 */
	bool presence_detected;
	/**
	 * The zone for maximum presence score if presence detected, -1 when no presence
	 */
	int32_t max_presence_zone;
	/**
	 * A measure of the amount of fast motion detected
	 */
	float intra_presence_score;
	/**
	 * A measure of the amount of slow motion detected
	 */
	float inter_presence_score;
	/**
	 * The index of the zone with maximum slow motion presence score, -1 when no presence
	 */
	int32_t max_inter_zone;
	/**
	 * The index of the zone with maximum fast motion presence score, -1 when no presence
	 */
	int32_t max_intra_zone;
	/**
	 * The configuration used, see smart_presence_mode_t
	 */
	uint32_t used_config;
	/**
	 * True if data was collected during switch delay.
	 */
	bool switch_delay;
	/**
	 * Array of wakeup detections for each zone, is NULL if use_wakeup_mode is set
	 */
	uint32_t *wakeup_detections;
	/**
	 * Array of presence results for each zone
	 */
	acc_smart_presence_zone_result_t *zone_results;
} acc_smart_presence_result_t;

typedef struct
{
	acc_detector_presence_handle_t   *detector_handle;
	acc_detector_presence_metadata_t metadata;
	uint32_t                         buffer_size;
} presence_detector_context_t;

typedef struct
{
	acc_ref_app_smart_presence_config_t *app_config;
	acc_detector_presence_config_t      *current_presence_config;
	uint32_t                            current_num_zones;
	smart_presence_mode_t               current_mode;
	presence_detector_context_t         wakeup_context;
	presence_detector_context_t         nominal_context;
	presence_detector_context_t         *current_context;
	acc_sensor_t                        *sensor;
	void                                *buffer;
	acc_cal_result_t                    sensor_cal_result;
	float                               *zone_limits;
	float                               *distances;
	uint32_t                            num_distances;
	uint32_t                            delay_count;
	uint32_t                            max_zone_time_n;
	uint32_t                            max_switch_delay_n;
	uint32_t                            *wakeup_detections;
	acc_smart_presence_zone_result_t    *zone_results;
} app_context_t;


static bool swap_config(app_context_t *context, smart_presence_mode_t new_mode);


static uint32_t get_max_buffer_size(app_context_t *context);


static uint16_t get_max_num_points(app_context_t *context);


static uint32_t get_max_num_zones(app_context_t *context);


static bool initialize_application_resources(app_context_t *context);


static void cleanup(app_context_t *context);


static bool initialize_sensor(app_context_t *context);


static bool sensor_prepare(app_context_t *context);


static bool sensor_calibration(app_context_t *context);


static bool detector_get_next(app_context_t                  *context,
                              acc_detector_presence_result_t *detector_result);


static bool enter_hibernate(acc_sensor_t *sensor);


static bool exit_hibernate(acc_sensor_t *sensor);


static void set_config(acc_ref_app_smart_presence_config_t *app_config, smart_presence_preset_config_t preset);


static int32_t process_inter_intra(app_context_t                  *context,
                                   acc_detector_presence_result_t *presence_result,
                                   acc_smart_presence_result_t    *result,
                                   bool                           inter);


static bool process_detector_result(app_context_t                  *context,
                                    acc_detector_presence_result_t *presence_result,
                                    acc_smart_presence_result_t    *result,
                                    bool                           *swap_needed,
                                    smart_presence_mode_t          *new_mode);


static bool determine_config_swap(app_context_t               *context,
                                  acc_smart_presence_result_t *result,
                                  smart_presence_mode_t       *new_mode);


static void print_result(app_context_t *context, acc_smart_presence_result_t *result);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	app_context_t                       context    = { 0 };
	acc_ref_app_smart_presence_config_t app_config = { 0 };

	context.app_config = &app_config;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	context.app_config->nominal_config.presence_config = acc_detector_presence_config_create();
	if (context.app_config->nominal_config.presence_config == NULL)
	{
		printf("acc_detector_presence_config_create() failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	context.app_config->wakeup_config.presence_config = acc_detector_presence_config_create();
	if (context.app_config->wakeup_config.presence_config == NULL)
	{
		printf("acc_detector_presence_config_create() failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	set_config(context.app_config, DEFAULT_PRESET_CONFIG);

	if (!initialize_application_resources(&context))
	{
		printf("Initializing detector context failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	/* Turn the sensor on */
	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	if (!initialize_sensor(&context))
	{
		printf("Sensor initializaiton failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	if (!sensor_calibration(&context))
	{
		printf("Sensor calibration failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	if (!sensor_prepare(&context))
	{
		printf("Sensor prepare failed failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	if (!enter_hibernate(context.sensor))
	{
		printf("enter_hibernate failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	while (true)
	{
		acc_detector_presence_result_t detector_result;
		acc_smart_presence_result_t    result      = { 0 };
		bool                           swap_needed = false;
		smart_presence_mode_t          new_mode;

		if (!detector_get_next(&context, &detector_result))
		{
			printf("Could not get next result\n");
			cleanup(&context);
			return EXIT_FAILURE;
		}

		if (!process_detector_result(&context, &detector_result, &result, &swap_needed, &new_mode))
		{
			printf("Failed to process presence result\n");
			cleanup(&context);
			return EXIT_FAILURE;
		}

		print_result(&context, &result);

		if (detector_result.processing_result.data_saturated)
		{
			printf("Data saturated. The detector result is not reliable.\n");
		}

		if (detector_result.processing_result.frame_delayed)
		{
			printf("Frame delayed. Could not read data fast enough.\n");
			printf("Try lowering the frame rate or call 'acc_sensor_read' more frequently.\n");
		}

		if (detector_result.processing_result.calibration_needed)
		{
			printf("Sensor recalibration needed ... \n");

			if (!exit_hibernate(context.sensor))
			{
				printf("exit_hibernate failed\n");
				return EXIT_FAILURE;
			}

			if (!sensor_calibration(&context))
			{
				printf("Sensor calibration failed\n");
				cleanup(&context);
				return EXIT_FAILURE;
			}

			printf("Sensor recalibration done!\n");

			// If we will swap config, prepare will be handled there
			if (!swap_needed)
			{
				if (!sensor_prepare(&context))
				{
					printf("Sensor prepare failed failed\n");
					cleanup(&context);
					return EXIT_FAILURE;
				}

				if (!enter_hibernate(context.sensor))
				{
					printf("enter_hibernate failed\n");
					return EXIT_FAILURE;
				}
			}
		}

		// Swap of config needs to be done after printing the result
		if (swap_needed)
		{
			swap_config(&context, new_mode);

			// If we have just calibrated, we're already out of hibernate
			if (!detector_result.processing_result.calibration_needed)
			{
				if (!exit_hibernate(context.sensor))
				{
					printf("exit_hibernate failed\n");
					return EXIT_FAILURE;
				}
			}

			// Before measuring again, the sensor needs to be prepared through the detector
			if (!sensor_prepare(&context))
			{
				printf("Sensor prepare failed failed\n");
				cleanup(&context);
				return EXIT_FAILURE;
			}

			if (!enter_hibernate(context.sensor))
			{
				printf("enter_hibernate failed\n");
				return EXIT_FAILURE;
			}
		}

		acc_integration_sleep_until_periodic_wakeup();
	}

	cleanup(&context);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static bool swap_config(app_context_t *context, smart_presence_mode_t new_mode)
{
	bool status = true;

	switch (new_mode)
	{
		case SMART_PRESENCE_MODE_WAKEUP:
			context->current_context         = &context->wakeup_context;
			context->current_presence_config = context->app_config->wakeup_config.presence_config;
			context->current_num_zones       = context->app_config->wakeup_config.num_zones;
			break;

		case SMART_PRESENCE_MODE_NOMINAL:
			context->current_context         = &context->nominal_context;
			context->current_presence_config = context->app_config->nominal_config.presence_config;
			context->current_num_zones       = context->app_config->nominal_config.num_zones;
			break;

		default:
			status = false;
			break;
	}

	if (status)
	{
		uint32_t sleep_time_ms = (uint32_t)(1000.0f / acc_detector_presence_config_frame_rate_get(context->current_presence_config));
		acc_integration_set_periodic_wakeup(sleep_time_ms);

		context->current_mode = new_mode;

		float start_m       = context->current_context->metadata.start_m;
		float step_length_m = context->current_context->metadata.step_length_m;

		context->num_distances = context->current_context->metadata.num_points;

		// Create zones limits based on chosen range and number of zones
		for (uint32_t distance = 0; distance < context->num_distances; distance++)
		{
			context->distances[distance] = start_m + (distance * step_length_m);
			if (context->current_num_zones == context->num_distances)
			{
				context->zone_limits[distance] = context->distances[distance];
			}
		}

		if (context->current_num_zones < context->num_distances)
		{
			float zone_step_length = (context->distances[context->num_distances - 1] - start_m) /
			                         context->current_num_zones;

			for (uint32_t zone = 0; zone < context->current_num_zones; zone++)
			{
				context->zone_limits[zone] = start_m + ((float)(zone + 1) * zone_step_length);
			}
		}
	}

	return status;
}


static uint32_t get_max_buffer_size(app_context_t *context)
{
	uint32_t max_buffer_size = context->nominal_context.buffer_size;

	if (context->app_config->use_wakeup_mode)
	{
		if (context->wakeup_context.buffer_size > max_buffer_size)
		{
			max_buffer_size = context->wakeup_context.buffer_size;
		}
	}

	return max_buffer_size;
}


static uint16_t get_max_num_points(app_context_t *context)
{
	uint16_t max_num_points = context->nominal_context.metadata.num_points;

	if (context->app_config->use_wakeup_mode)
	{
		if (context->wakeup_context.metadata.num_points > max_num_points)
		{
			max_num_points = context->wakeup_context.metadata.num_points;
		}
	}

	return max_num_points;
}


static uint32_t get_max_num_zones(app_context_t *context)
{
	uint32_t max_num_zones = context->app_config->nominal_config.num_zones;

	if (context->app_config->use_wakeup_mode)
	{
		if (context->app_config->wakeup_config.num_zones > max_num_zones)
		{
			max_num_zones = context->app_config->wakeup_config.num_zones;
		}
	}

	return max_num_zones;
}


static bool initialize_wakeup_resources(app_context_t *context)
{
	if (context->app_config->use_wakeup_mode)
	{
		context->wakeup_context.detector_handle = acc_detector_presence_create(context->app_config->wakeup_config.presence_config,
		                                                                       &context->wakeup_context.metadata);
		if (context->wakeup_context.detector_handle == NULL)
		{
			printf("acc_detector_presence_create() failed\n");
			return false;
		}

		if (!acc_detector_presence_get_buffer_size(context->wakeup_context.detector_handle, &context->wakeup_context.buffer_size))
		{
			printf("acc_detector_presence_get_buffer_size() failed\n");
			return false;
		}
	}
	else
	{
		context->wakeup_context.detector_handle = NULL;
		context->wakeup_context.buffer_size     = 0;
	}

	return true;
}


static bool initialize_application_resources(app_context_t *context)
{
	context->nominal_context.detector_handle = acc_detector_presence_create(context->app_config->nominal_config.presence_config,
	                                                                        &context->nominal_context.metadata);
	if (context->nominal_context.detector_handle == NULL)
	{
		printf("acc_detector_presence_create() failed\n");
		return false;
	}

	if (!acc_detector_presence_get_buffer_size(context->nominal_context.detector_handle, &context->nominal_context.buffer_size))
	{
		printf("acc_detector_presence_get_buffer_size() failed\n");
		return false;
	}

	if (!initialize_wakeup_resources(context))
	{
		printf("Failed to initialize resources for wakeup\n");
		return false;
	}

	uint32_t max_buffer_size = get_max_buffer_size(context);
	uint16_t max_num_points  = get_max_num_points(context);
	uint32_t max_num_zones   = get_max_num_zones(context);

	context->buffer = acc_integration_mem_alloc(max_buffer_size);
	if (context->buffer == NULL)
	{
		printf("sensor buffer allocation failed\n");
		return false;
	}

	context->distances = acc_integration_mem_alloc(sizeof(*context->distances) * max_num_points);
	if (context->distances == NULL)
	{
		printf("distances allocation failed\n");
		return false;
	}

	context->zone_limits = acc_integration_mem_alloc(sizeof(*context->zone_limits) * max_num_zones);
	if (context->zone_limits == NULL)
	{
		printf("zone limits allocation failed\n");
		return false;
	}

	context->zone_results = acc_integration_mem_alloc(sizeof(acc_smart_presence_zone_result_t) *
	                                                  max_num_zones);
	if (context->zone_results == NULL)
	{
		printf("zone result allocation failed\n");
		return false;
	}

	if (context->app_config->use_wakeup_mode)
	{
		context->wakeup_detections = acc_integration_mem_calloc(1U, sizeof(*context->wakeup_detections) *
		                                                        context->app_config->wakeup_config.num_zones);

		if (context->wakeup_detections == NULL)
		{
			printf("wake up detection allocation failed\n");
			return false;
		}

		float frame_rate = acc_detector_presence_config_frame_rate_get(context->app_config->wakeup_config.presence_config);
		context->max_zone_time_n = (uint32_t)((2.0f * frame_rate) + 0.5f);

		swap_config(context, SMART_PRESENCE_MODE_WAKEUP);
	}
	else
	{
		context->wakeup_detections = NULL;
		context->max_zone_time_n   = 0;

		swap_config(context, SMART_PRESENCE_MODE_NOMINAL);
	}

	float inter_frame_deviation_time_const =
		acc_detector_presence_config_inter_frame_deviation_time_const_get(context->app_config->nominal_config.presence_config);
	float inter_output_time_const =
		acc_detector_presence_config_inter_output_time_const_get(context->app_config->nominal_config.presence_config);
	float intra_frame_time_const =
		acc_detector_presence_config_intra_frame_time_const_get(context->app_config->nominal_config.presence_config);
	float intra_output_time_const =
		acc_detector_presence_config_intra_output_time_const_get(context->app_config->nominal_config.presence_config);
	float frame_rate = acc_detector_presence_config_frame_rate_get(context->app_config->nominal_config.presence_config);

	float max_switch_delay_n = inter_frame_deviation_time_const + inter_output_time_const;

	if (max_switch_delay_n < (intra_frame_time_const + intra_output_time_const))
	{
		max_switch_delay_n = intra_frame_time_const + intra_output_time_const;
	}

	context->max_switch_delay_n = (uint32_t)(max_switch_delay_n * frame_rate);

	return true;
}


static void cleanup(app_context_t *context)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (context->app_config->nominal_config.presence_config != NULL)
	{
		acc_detector_presence_config_destroy(context->app_config->nominal_config.presence_config);
	}

	if (context->app_config->wakeup_config.presence_config != NULL)
	{
		acc_detector_presence_config_destroy(context->app_config->wakeup_config.presence_config);
	}

	if (context->nominal_context.detector_handle != NULL)
	{
		acc_detector_presence_destroy(context->nominal_context.detector_handle);
	}

	if (context->wakeup_context.detector_handle != NULL)
	{
		acc_detector_presence_destroy(context->wakeup_context.detector_handle);
	}

	if (context->sensor != NULL)
	{
		acc_sensor_destroy(context->sensor);
	}

	if (context->buffer != NULL)
	{
		acc_integration_mem_free(context->buffer);
	}

	if (context->zone_limits != NULL)
	{
		acc_integration_mem_free(context->zone_limits);
	}

	if (context->distances != NULL)
	{
		acc_integration_mem_free(context->distances);
	}

	if (context->zone_results != NULL)
	{
		acc_integration_mem_free(context->zone_results);
	}

	if (context->wakeup_detections != NULL)
	{
		acc_integration_mem_free(context->wakeup_detections);
	}
}


static bool initialize_sensor(app_context_t *context)
{
	context->sensor = acc_sensor_create(SENSOR_ID);
	if (context->sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		return false;
	}

	return true;
}


static bool sensor_prepare(app_context_t *context)
{
	bool status = true;

	if (!acc_detector_presence_prepare(context->current_context->detector_handle,
	                                   context->current_presence_config,
	                                   context->sensor,
	                                   &context->sensor_cal_result,
	                                   context->buffer,
	                                   context->current_context->buffer_size))
	{
		printf("acc_detector_presence_prepare() failed\n");
		status = false;
	}

	return status;
}


static bool sensor_calibration(app_context_t *context)
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
			status = acc_sensor_calibrate(context->sensor, &cal_complete, &context->sensor_cal_result, context->buffer,
			                              context->current_context->buffer_size);

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
		acc_sensor_status(context->sensor);
	}

	return status;
}


static bool detector_get_next(app_context_t                  *context,
                              acc_detector_presence_result_t *detector_result)
{
	if (!exit_hibernate(context->sensor))
	{
		printf("exit_hibernate failed\n");
		return EXIT_FAILURE;
	}

	if (!acc_sensor_measure(context->sensor))
	{
		printf("acc_sensor_measure failed\n");
		return EXIT_FAILURE;
	}

	if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
	{
		printf("Sensor interrupt timeout\n");
		return EXIT_FAILURE;
	}

	if (!acc_sensor_read(context->sensor, context->buffer, context->current_context->buffer_size))
	{
		printf("acc_sensor_read failed\n");
		return EXIT_FAILURE;
	}

	if (!enter_hibernate(context->sensor))
	{
		printf("enter_hibernate failed\n");
		return EXIT_FAILURE;
	}

	if (!acc_detector_presence_process(context->current_context->detector_handle, context->buffer, detector_result))
	{
		printf("acc_detector_presence_process failed\n");
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


static void set_config(acc_ref_app_smart_presence_config_t *app_config, smart_presence_preset_config_t preset)
{
	switch (preset)
	{
		case SMART_PRESENCE_PRESET_CONFIG_NONE:
			// Add custom configuration of the reference application here
			break;

		case SMART_PRESENCE_PRESET_CONFIG_SHORT_RANGE:
			app_config->show_all_detected_zones = false;
			app_config->use_wakeup_mode         = true;

			// Wakeup config
			app_config->wakeup_config.num_zones            = 1;
			app_config->wakeup_config.num_zones_for_wakeup = 1;
			acc_detector_presence_config_start_set(app_config->wakeup_config.presence_config, 0.5f);
			acc_detector_presence_config_end_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_auto_step_length_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->wakeup_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->wakeup_config.presence_config, 16);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->wakeup_config.presence_config, 16);
			acc_detector_presence_config_frame_rate_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->wakeup_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->wakeup_config.presence_config, 1.5f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->wakeup_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->wakeup_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->wakeup_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->wakeup_config.presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->wakeup_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->wakeup_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->wakeup_config.presence_config, false);

			// Nominal config
			app_config->nominal_config.num_zones = 5;
			acc_detector_presence_config_start_set(app_config->nominal_config.presence_config, 0.06f);
			acc_detector_presence_config_end_set(app_config->nominal_config.presence_config, 1.0f);
			acc_detector_presence_config_auto_step_length_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->nominal_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->nominal_config.presence_config, 16);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->nominal_config.presence_config, 16);
			acc_detector_presence_config_frame_rate_set(app_config->nominal_config.presence_config, 10.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->nominal_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->nominal_config.presence_config, 1.4f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->nominal_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->nominal_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->nominal_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->nominal_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->nominal_config.presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->nominal_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->nominal_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->nominal_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->nominal_config.presence_config, false);

			break;

		case SMART_PRESENCE_PRESET_CONFIG_MEDIUM_RANGE:
			app_config->show_all_detected_zones = false;
			app_config->use_wakeup_mode         = true;

			// Wakeup config
			app_config->wakeup_config.num_zones            = 1;
			app_config->wakeup_config.num_zones_for_wakeup = 1;
			acc_detector_presence_config_start_set(app_config->wakeup_config.presence_config, 1.5f);
			acc_detector_presence_config_end_set(app_config->wakeup_config.presence_config, 2.5f);
			acc_detector_presence_config_auto_step_length_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->wakeup_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->wakeup_config.presence_config, 32);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->wakeup_config.presence_config, 16);
			acc_detector_presence_config_frame_rate_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->wakeup_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->wakeup_config.presence_config, 1.5f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->wakeup_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->wakeup_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->wakeup_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->wakeup_config.presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->wakeup_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->wakeup_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->wakeup_config.presence_config, false);

			// Nominal config
			app_config->nominal_config.num_zones = 7;
			acc_detector_presence_config_start_set(app_config->nominal_config.presence_config, 0.3f);
			acc_detector_presence_config_end_set(app_config->nominal_config.presence_config, 2.5f);
			acc_detector_presence_config_auto_step_length_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->nominal_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->nominal_config.presence_config, 32);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->nominal_config.presence_config, 16);
			acc_detector_presence_config_frame_rate_set(app_config->nominal_config.presence_config, 12.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->nominal_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->nominal_config.presence_config, 1.3f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->nominal_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->nominal_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->nominal_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->nominal_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->nominal_config.presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->nominal_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->nominal_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->nominal_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->nominal_config.presence_config, false);

			break;

		case SMART_PRESENCE_PRESET_CONFIG_LONG_RANGE:
			app_config->show_all_detected_zones = false;
			app_config->use_wakeup_mode         = true;

			// Wakeup config
			app_config->wakeup_config.num_zones            = 5;
			app_config->wakeup_config.num_zones_for_wakeup = 2;
			acc_detector_presence_config_start_set(app_config->wakeup_config.presence_config, 3.0f);
			acc_detector_presence_config_end_set(app_config->wakeup_config.presence_config, 5.0f);
			acc_detector_presence_config_auto_step_length_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->wakeup_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->wakeup_config.presence_config, 48);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->wakeup_config.presence_config, 32);
			acc_detector_presence_config_frame_rate_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->wakeup_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->wakeup_config.presence_config, 1.4f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->wakeup_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->wakeup_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->wakeup_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->wakeup_config.presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->wakeup_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->wakeup_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->wakeup_config.presence_config, false);

			// Nominal config
			app_config->nominal_config.num_zones = 5;
			acc_detector_presence_config_start_set(app_config->nominal_config.presence_config, 5.0f);
			acc_detector_presence_config_end_set(app_config->nominal_config.presence_config, 7.5f);
			acc_detector_presence_config_auto_step_length_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->nominal_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->nominal_config.presence_config, 128);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->nominal_config.presence_config, 32);
			acc_detector_presence_config_frame_rate_set(app_config->nominal_config.presence_config, 12.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->nominal_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->nominal_config.presence_config, 1.2f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->nominal_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->nominal_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->nominal_config.presence_config, 0.8f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->nominal_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->nominal_config.presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->nominal_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->nominal_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->nominal_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->nominal_config.presence_config, false);

			break;

		case SMART_PRESENCE_PRESET_CONFIG_CEILING:
			app_config->show_all_detected_zones = false;
			app_config->use_wakeup_mode         = true;

			// Wakeup config
			app_config->wakeup_config.num_zones            = 1;
			app_config->wakeup_config.num_zones_for_wakeup = 1;
			acc_detector_presence_config_start_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_end_set(app_config->wakeup_config.presence_config, 3.5f);
			acc_detector_presence_config_auto_step_length_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->wakeup_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->wakeup_config.presence_config, 32);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->wakeup_config.presence_config, 24);
			acc_detector_presence_config_frame_rate_set(app_config->wakeup_config.presence_config, 4.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->wakeup_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->wakeup_config.presence_config, 1.5f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->wakeup_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->wakeup_config.presence_config, 0.25f);
			acc_detector_presence_config_inter_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->wakeup_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->wakeup_config.presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->wakeup_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->wakeup_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->wakeup_config.presence_config, false);

			// Nominal config
			app_config->nominal_config.num_zones = 1;
			acc_detector_presence_config_start_set(app_config->nominal_config.presence_config, 1.5f);
			acc_detector_presence_config_end_set(app_config->nominal_config.presence_config, 3.5f);
			acc_detector_presence_config_auto_step_length_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->nominal_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->nominal_config.presence_config, 18);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->nominal_config.presence_config, 18);
			acc_detector_presence_config_frame_rate_set(app_config->nominal_config.presence_config, 12.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->nominal_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->nominal_config.presence_config, 1.2f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->nominal_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->nominal_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->nominal_config.presence_config, 0.8f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->nominal_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->nominal_config.presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->nominal_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->nominal_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->nominal_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->nominal_config.presence_config, true);

			break;

		case SMART_PRESENCE_PRESET_CONFIG_LOW_POWER_WAKEUP:
			app_config->show_all_detected_zones = false;
			app_config->use_wakeup_mode         = true;

			// Wakeup config
			app_config->wakeup_config.num_zones            = 1;
			app_config->wakeup_config.num_zones_for_wakeup = 1;
			acc_detector_presence_config_start_set(app_config->wakeup_config.presence_config, 0.725f);
			acc_detector_presence_config_end_set(app_config->wakeup_config.presence_config, 1.505f);
			acc_detector_presence_config_auto_step_length_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->wakeup_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->wakeup_config.presence_config, 8);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->wakeup_config.presence_config, 16);
			acc_detector_presence_config_frame_rate_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->wakeup_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->wakeup_config.presence_config, 1.5f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->wakeup_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->wakeup_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->wakeup_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->wakeup_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->wakeup_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->wakeup_config.presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->wakeup_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->wakeup_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->wakeup_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->wakeup_config.presence_config, false);

			// Nominal config
			app_config->nominal_config.num_zones = 1;
			acc_detector_presence_config_start_set(app_config->nominal_config.presence_config, 0.725f);
			acc_detector_presence_config_end_set(app_config->nominal_config.presence_config, 1.505f);
			acc_detector_presence_config_auto_step_length_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_auto_profile_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_frame_idle_state_set(app_config->nominal_config.presence_config,
			                                                        ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(app_config->nominal_config.presence_config, 8);
			acc_detector_presence_config_sweeps_per_frame_set(app_config->nominal_config.presence_config, 16);
			acc_detector_presence_config_frame_rate_set(app_config->nominal_config.presence_config, 5.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(app_config->nominal_config.presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(app_config->nominal_config.presence_config, 1.4f);
			acc_detector_presence_config_intra_frame_time_const_set(app_config->nominal_config.presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(app_config->nominal_config.presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(app_config->nominal_config.presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(app_config->nominal_config.presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(app_config->nominal_config.presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(app_config->nominal_config.presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(app_config->nominal_config.presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(app_config->nominal_config.presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(app_config->nominal_config.presence_config, 3);
			acc_detector_presence_config_inter_phase_boost_set(app_config->nominal_config.presence_config, false);

			break;
	}

	acc_detector_presence_config_frame_rate_app_driven_set(app_config->wakeup_config.presence_config, true);
	acc_detector_presence_config_frame_rate_app_driven_set(app_config->nominal_config.presence_config, true);
}


static int32_t process_inter_intra(app_context_t                  *context,
                                   acc_detector_presence_result_t *presence_result,
                                   acc_smart_presence_result_t    *result,
                                   bool                           inter)
{
	static int32_t max_inter_zone = -1;
	static int32_t max_intra_zone = -1;

	bool    detection;
	float   threshold;
	float   score;
	float   *depthwise_scores;
	int32_t *max_presence_zone;

	if (inter)
	{
		detection = acc_detector_presence_config_inter_detection_get(context->current_presence_config);
		threshold = acc_detector_presence_config_inter_detection_threshold_get(context->current_presence_config);
		score     = presence_result->inter_presence_score;

		depthwise_scores  = presence_result->depthwise_inter_presence_scores;
		max_presence_zone = &max_inter_zone;
	}
	else
	{
		detection = acc_detector_presence_config_intra_detection_get(context->current_presence_config);
		threshold = acc_detector_presence_config_intra_detection_threshold_get(context->current_presence_config);
		score     = presence_result->intra_presence_score;

		depthwise_scores  = presence_result->depthwise_intra_presence_scores;
		max_presence_zone = &max_intra_zone;
	}

	/* Update zone detections for each detection type */
	if (detection)
	{
		if (score > threshold)
		{
			/* Get presence detection result for each zone */
			bool     any_zone_detected = false;
			bool     zone_detected     = false;
			uint32_t limit_idx         = 0;

			for (uint32_t i = 0; i < context->num_distances; i++)
			{
				if (context->distances[i] > context->zone_limits[limit_idx])
				{
					limit_idx++;
					zone_detected = false;
				}

				if (limit_idx >= context->current_num_zones)
				{
					break;
				}

				if (!zone_detected)
				{
					zone_detected = (depthwise_scores[i] > threshold);
					if (inter)
					{
						result->zone_results[limit_idx].inter_zone_detection = zone_detected;
					}
					else
					{
						result->zone_results[limit_idx].intra_zone_detection = zone_detected;
					}

					if (zone_detected)
					{
						any_zone_detected = true;
					}
				}
			}

			/* If no presence is detected in the dephwise array we use the latest zone with detection */
			if (!any_zone_detected)
			{
				if (inter)
				{
					result->zone_results[*max_presence_zone].inter_zone_detection = true;
				}
				else
				{
					result->zone_results[*max_presence_zone].intra_zone_detection = true;
				}
			}
			else
			{
				/* Get the zone with maximum presence score */
				float    max_score    = 0.0f;
				float    max_distance = 0.0f;
				uint32_t max_zone     = 0;

				for (uint32_t i = 0; i < context->num_distances; i++)
				{
					if (depthwise_scores[i] > max_score)
					{
						max_score    = depthwise_scores[i];
						max_distance = context->distances[i];
					}
				}

				while (max_zone < (context->current_num_zones - 1) && max_distance > context->zone_limits[max_zone])
				{
					max_zone++;
				}

				*max_presence_zone = max_zone;
			}
		}
		else
		{
			*max_presence_zone = -1;
		}
	}

	return *max_presence_zone;
}


static bool process_detector_result(app_context_t                  *context,
                                    acc_detector_presence_result_t *presence_result,
                                    acc_smart_presence_result_t    *result,
                                    bool                           *swap_needed,
                                    smart_presence_mode_t          *new_mode)
{
	bool    status         = true;
	int32_t max_inter_zone = -1;
	int32_t max_intra_zone = -1;

	/* We need to clear the zone results array for every iteration */
	memset(context->zone_results, 0, sizeof(acc_smart_presence_zone_result_t) * context->current_num_zones);

	result->zone_results      = context->zone_results;
	result->max_presence_zone = -1;

	if (presence_result->presence_detected)
	{
		/* Update zone detections for each detection type */
		max_inter_zone = process_inter_intra(context, presence_result, result, true);
		max_intra_zone = process_inter_intra(context, presence_result, result, false);

		/* max intra zone is prioritized due to faster reaction time */
		if (max_intra_zone != -1)
		{
			result->max_presence_zone = max_intra_zone;
		}
		else
		{
			result->max_presence_zone = max_inter_zone;
		}

		for (uint32_t i = 0; i < context->current_num_zones; i++)
		{
			if (result->zone_results[i].intra_zone_detection ||
			    result->zone_results[i].inter_zone_detection)
			{
				result->zone_results[i].zone_detection = true;
			}
		}
	}

	for (uint32_t i = 0; i < context->current_num_zones; i++)
	{
		result->zone_results[i].zone_limit = context->zone_limits[i];
	}

	result->presence_detected    = presence_result->presence_detected;
	result->inter_presence_score = presence_result->inter_presence_score;
	result->intra_presence_score = presence_result->intra_presence_score;
	result->max_inter_zone       = max_inter_zone;
	result->max_intra_zone       = max_intra_zone;
	result->used_config          = context->current_mode;

	if (context->app_config->use_wakeup_mode)
	{
		*swap_needed = determine_config_swap(context, result, new_mode);
	}

	// delay_count and wakeup_detections is updated in determine_config_swap
	result->switch_delay      = true ? context->delay_count > 0 : false;
	result->wakeup_detections = context->wakeup_detections;

	return status;
}


static bool determine_config_swap(app_context_t               *context,
                                  acc_smart_presence_result_t *result,
                                  smart_presence_mode_t       *new_mode)
{
	bool swap_config = false;

//
	if (context->delay_count == 0)
	{
		if (context->current_mode == SMART_PRESENCE_MODE_WAKEUP && result->presence_detected)
		{
			uint32_t num_detections = 0;

			for (uint32_t i = 0; i < context->app_config->wakeup_config.num_zones; i++)
			{
				if (result->zone_results[i].zone_detection)
				{
					context->wakeup_detections[i] = context->max_zone_time_n;
				}

				if (context->wakeup_detections[i] > 0)
				{
					num_detections++;
					context->wakeup_detections[i] -= 1;
				}
			}

			if (num_detections >= context->app_config->wakeup_config.num_zones_for_wakeup)
			{
				*new_mode   = SMART_PRESENCE_MODE_NOMINAL;
				swap_config = true;

				context->delay_count++;
			}
		}
		else if (context->current_mode == SMART_PRESENCE_MODE_NOMINAL && !result->presence_detected)
		{
			*new_mode   = SMART_PRESENCE_MODE_WAKEUP;
			swap_config = true;
		}
	}
	else
	{
		if (context->delay_count == 1)
		{
			memset(context->wakeup_detections, 0, (context->app_config->wakeup_config.num_zones * sizeof(*context->wakeup_detections)));
		}

		context->delay_count++;
		if (context->delay_count >= (context->max_switch_delay_n + 1) || result->presence_detected)
		{
			context->delay_count = 0;
		}
	}

	return swap_config;
}


static void print_result(app_context_t *context, acc_smart_presence_result_t *result)
{
	/* Format of output will look like this for four zones:
	 *
	 *  show_all_detected_zones enabled
	 *    - intra presence in the first zone
	 *    - inter presence in the second
	 *    - both inter and intra in the third
	 *    - Wakeup mode
	 *
	 *    --M:0---------------------------
	 *    <INTER> |  #  |     |  #  |     |
	 *    <INTRA> |     |  #  |  #  |     |
	 *    ---------------------------------
	 *
	 *  show_all_detected_zones disabled
	 *    - Highest presence detected in the second zone
	 *    - Nominal mode
	 *
	 *    --M:1----------------------------
	 *    <MAX>   |     |  #  |     |     |
	 *    ---------------------------------
	 */
	printf(" --M:%" PRIu32 "-----", result->used_config);
	for (uint16_t i = 0; i < context->current_num_zones; i++)
	{
		printf("------");
	}

	printf("\n");

	if (context->app_config->show_all_detected_zones)
	{
		printf(" <INTER> |");
		for (uint16_t i = 0; i < context->current_num_zones; i++)
		{
			if (result->zone_results[i].inter_zone_detection)
			{
				printf("  #  |");
			}
			else
			{
				printf("     |");
			}
		}

		printf("\n");
		printf(" <INTRA> |");
		for (uint16_t i = 0; i < context->current_num_zones; i++)
		{
			if (result->zone_results[i].intra_zone_detection)
			{
				printf("  #  |");
			}
			else
			{
				printf("     |");
			}
		}

		printf("\n");
	}
	else
	{
		printf(" <MAX>   |");
		for (uint16_t i = 0; i < context->current_num_zones; i++)
		{
			if (i == result->max_presence_zone)
			{
				printf("  #  |");
			}
			else
			{
				printf("     |");
			}
		}

		printf("\n");
	}

	printf(" ---------");
	for (uint16_t i = 0; i < context->current_num_zones; i++)
	{
		printf("------");
	}

	printf("\n");
}
