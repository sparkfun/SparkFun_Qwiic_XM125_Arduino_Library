// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <math.h>
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


#define SENSOR_ID           1U
#define SENSOR_TIMEOUT_MS   1000U
#define DEFAULT_UPDATE_RATE 1.0f

// Settings for a small tank
#define SMALL_TANK_MEDIAN_FILTER_LENGTH             5U
#define SMALL_TANK_NUM_MEDIANS_TO_AVERAGE           5U
#define SMALL_TANK_RANGE_START_M                    0.03f
#define SMALL_TANK_RANGE_END_M                      0.5f
#define SMALL_TANK_MAX_STEP_LENGTH                  2U
#define SMALL_TANK_MAX_PROFILE                      ACC_CONFIG_PROFILE_2
#define SMALL_TANK_NUM_FRAMES_REC                   50U
#define SMALL_TANK_PEAK_SORTING                     ACC_DETECTOR_DISTANCE_PEAK_SORTING_CLOSEST
#define SMALL_TANK_REFLECTOR_SHAPE                  ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_PLANAR
#define SMALL_TANK_THRESHOLD_SENSITIVITY            0.0f
#define SMALL_TANK_SIGNAL_QUALITY                   20.0f
#define SMALL_TANK_CLOSE_RANGE_LEAKAGE_CANCELLATION true

// Settings for a medium tank
#define MEDIUM_TANK_MEDIAN_FILTER_LENGTH             3U
#define MEDIUM_TANK_NUM_MEDIANS_TO_AVERAGE           3U
#define MEDIUM_TANK_RANGE_START_M                    0.05f
#define MEDIUM_TANK_RANGE_END_M                      6.0f
#define MEDIUM_TANK_MAX_STEP_LENGTH                  0U // 0 means no limit
#define MEDIUM_TANK_MAX_PROFILE                      ACC_CONFIG_PROFILE_3
#define MEDIUM_TANK_NUM_FRAMES_REC                   50U
#define MEDIUM_TANK_PEAK_SORTING                     ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST
#define MEDIUM_TANK_REFLECTOR_SHAPE                  ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_PLANAR
#define MEDIUM_TANK_THRESHOLD_SENSITIVITY            0.0f
#define MEDIUM_TANK_SIGNAL_QUALITY                   20.0f
#define MEDIUM_TANK_CLOSE_RANGE_LEAKAGE_CANCELLATION true

// Settings for a large tank
#define LARGE_TANK_MEDIAN_FILTER_LENGTH             3U
#define LARGE_TANK_NUM_MEDIANS_TO_AVERAGE           1U
#define LARGE_TANK_RANGE_START_M                    0.1f
#define LARGE_TANK_RANGE_END_M                      10.0f
#define LARGE_TANK_MAX_STEP_LENGTH                  0U // 0 means no limit
#define LARGE_TANK_MAX_PROFILE                      ACC_CONFIG_PROFILE_5
#define LARGE_TANK_NUM_FRAMES_REC                   50U
#define LARGE_TANK_PEAK_SORTING                     ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST
#define LARGE_TANK_REFLECTOR_SHAPE                  ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_PLANAR
#define LARGE_TANK_THRESHOLD_SENSITIVITY            0.0f
#define LARGE_TANK_SIGNAL_QUALITY                   20.0f
#define LARGE_TANK_CLOSE_RANGE_LEAKAGE_CANCELLATION true

/** \example ref_app_tank_level.c
 * @brief This is a reference application for measuring liquid level in tanks
 * @n
 * This reference application executes as follows:
 *   - Retrieve HAL integration
 *   - Initialize application resources:
 *     + Create application configuration
 *     + Create distance detector configuration
 *     + Update configuration settings
 *     + Create distance detector handle
 *     + Create buffer for detector calibration data
 *     + Create buffer for sensor data
 *   - Create and calibrate the sensor
 *   - Calibrate the detector
 *   - Measure distances with the detector (loop):
 *     + Prepare sensor with the detector
 *     + Measure and wait until a read can be done
 *     + Process sensor measurement and get distance detector result
 *     + Process distance detector result and print the result
 *     + Handle "sensor_calibration_needed" indication
 *   - Cleanup:
 *     + Destroy detector configuration
 *     + Destroy detector handle
 *     + Destroy sensor data buffer
 *     + Destroy detector calibration data buffer
 */


typedef enum
{
	TANK_LEVEL_PRESET_CONFIG_NONE = 0,
	TANK_LEVEL_PRESET_CONFIG_SMALL_TANK,
	TANK_LEVEL_PRESET_CONFIG_MEDIUM_TANK,
	TANK_LEVEL_PRESET_CONFIG_LARGE_TANK,
} tank_level_preset_config_t;

#define DEFAULT_PRESET_CONFIG TANK_LEVEL_PRESET_CONFIG_SMALL_TANK


typedef struct
{
	float                          tank_range_start_m;
	float                          tank_range_end_m;
	uint16_t                       median_filter_length;
	uint16_t                       num_medians_to_average;
	acc_detector_distance_config_t *distance_config;
} acc_ref_app_tank_level_config_t;

typedef struct
{
	acc_ref_app_tank_level_config_t *app_config;
	acc_sensor_t                    *sensor;
	float                           *level_history;
	uint16_t                        level_history_length;
	float                           *median_vector;
	uint16_t                        median_vector_length;
	uint16_t                        median_counter;
	uint16_t                        mean_counter;
	uint16_t                        median_edge_status_count;
	uint16_t                        mean_edge_status_count;
	acc_detector_distance_handle_t  *detector_handle;
	void                            *buffer;
	uint32_t                        buffer_size;
	void                            *calibration_buffer;
	uint32_t                        calibration_buffer_size;
} app_context_t;

typedef enum
{
	PEAK_STATUS_IN_RANGE,
	PEAK_STATUS_NO_DETECTION,
	PEAK_STATUS_OVERFLOW,
	PEAK_STATUS_OUT_OF_RANGE
} peak_status_t;

typedef struct
{
	bool          peak_detected;
	peak_status_t peak_status;
	float         level;
	bool          result_ready;
} app_result_t;


static void cleanup(app_context_t *context);


static float get_detector_start_m(acc_ref_app_tank_level_config_t *app_config);


static float get_detector_end_m(acc_ref_app_tank_level_config_t *app_config);


static void set_config(acc_ref_app_tank_level_config_t *app_config, tank_level_preset_config_t preset);


static bool initialize_application_resources(app_context_t *context);


static bool sensor_calibration(acc_sensor_t     *sensor,
                               acc_cal_result_t *cal_result,
                               void             *buffer,
                               uint32_t         buffer_size);


static bool full_detector_calibration(app_context_t          *context,
                                      const acc_cal_result_t *sensor_cal_result);


static bool detector_recalibration(app_context_t          *context,
                                   const acc_cal_result_t *sensor_cal_result);


static bool detector_get_next(app_context_t                  *context,
                              const acc_cal_result_t         *sensor_cal_result,
                              acc_detector_distance_result_t *detector_result);


static void swap(float *array, uint16_t idx_a, uint16_t idx_b);


static void sort(float *array, uint16_t array_length);


static float median(float *array, uint16_t array_length);


static float nanmean(float *array, uint16_t array_length);


static void process_detector_result(const acc_detector_distance_result_t *distance_result, app_result_t *app_result,
                                    app_context_t *context);


static void print_result(const app_result_t *app_result, const acc_ref_app_tank_level_config_t *app_config);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	app_context_t                   context    = { 0 };
	acc_ref_app_tank_level_config_t app_config = { 0 };
	context.app_config = &app_config;
	context.sensor     = NULL;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	context.app_config->distance_config = acc_detector_distance_config_create();
	if (context.app_config->distance_config == NULL)
	{
		printf("acc_detector_distance_config_create() failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	set_config(&app_config, DEFAULT_PRESET_CONFIG);

	uint32_t sleep_time_ms = (uint32_t)(1000.0f / DEFAULT_UPDATE_RATE);

	acc_integration_set_periodic_wakeup(sleep_time_ms);

	if (!initialize_application_resources(&context))
	{
		printf("Initializing detector context failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	/* Turn the sensor on */
	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	context.sensor = acc_sensor_create(SENSOR_ID);
	if (context.sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	acc_cal_result_t cal_result;

	if (!sensor_calibration(context.sensor, &cal_result, context.buffer, context.buffer_size))
	{
		printf("Sensor calibration failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	if (!full_detector_calibration(&context, &cal_result))
	{
		printf("Detector calibration failed\n");
		cleanup(&context);
		return EXIT_FAILURE;
	}

	while (true)
	{
		acc_detector_distance_result_t detector_result = { 0 };
		app_result_t                   app_result      = { 0 };

		if (!detector_get_next(&context, &cal_result, &detector_result))
		{
			printf("Could not get next result\n");
			cleanup(&context);
			return EXIT_FAILURE;
		}

		process_detector_result(&detector_result, &app_result, &context);

		/* If "sensor calibration needed" is indicated, the sensor and detector needs to be recalibrated */
		if (detector_result.sensor_calibration_needed)
		{
			printf("Sensor- and detector recalibration needed ... \n");

			if (!sensor_calibration(context.sensor, &cal_result, context.buffer, context.buffer_size))
			{
				printf("Sensor calibration failed\n");
				cleanup(&context);
				return EXIT_FAILURE;
			}

			/* Once the sensor is recalibrated, the detector should be recalibrated and measuring can continue. */
			if (!detector_recalibration(&context, &cal_result))
			{
				printf("Detector recalibration failed\n");
				cleanup(&context);
				return EXIT_FAILURE;
			}

			printf("Sensor- and detector recalibration done!\n");
		}

		if (app_result.result_ready)
		{
			acc_hal_integration_sensor_disable(SENSOR_ID);

			print_result(&app_result, context.app_config);

			acc_integration_sleep_until_periodic_wakeup();

			acc_hal_integration_sensor_enable(SENSOR_ID);
		}
	}

	cleanup(&context);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(app_context_t *context)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	acc_detector_distance_config_destroy(context->app_config->distance_config);
	acc_detector_distance_destroy(context->detector_handle);

	acc_integration_mem_free(context->buffer);
	acc_integration_mem_free(context->calibration_buffer);
	acc_integration_mem_free(context->level_history);
	acc_integration_mem_free(context->median_vector);

	if (context->sensor != NULL)
	{
		acc_sensor_destroy(context->sensor);
	}
}


static float get_detector_start_m(acc_ref_app_tank_level_config_t *app_config)
{
	// Decrease start point by 15 mm to make sure that start of the tank is fully covered
	return app_config->tank_range_start_m - 0.015f;
}


static float get_detector_end_m(acc_ref_app_tank_level_config_t *app_config)
{
	// Increase end point by 5% to make sure that the bottom of the tank is fully covered
	return app_config->tank_range_end_m * 1.05f;
}


static void set_config(acc_ref_app_tank_level_config_t *app_config, tank_level_preset_config_t preset)
{
	switch (preset)
	{
		case TANK_LEVEL_PRESET_CONFIG_NONE:
			break;
		case TANK_LEVEL_PRESET_CONFIG_SMALL_TANK:
			app_config->tank_range_start_m     = SMALL_TANK_RANGE_START_M;
			app_config->tank_range_end_m       = SMALL_TANK_RANGE_END_M;
			app_config->median_filter_length   = SMALL_TANK_MEDIAN_FILTER_LENGTH;
			app_config->num_medians_to_average = SMALL_TANK_NUM_MEDIANS_TO_AVERAGE;

			acc_detector_distance_config_start_set(app_config->distance_config, get_detector_start_m(app_config));
			acc_detector_distance_config_end_set(app_config->distance_config, get_detector_end_m(app_config));
			acc_detector_distance_config_max_step_length_set(app_config->distance_config, SMALL_TANK_MAX_STEP_LENGTH);
			acc_detector_distance_config_max_profile_set(app_config->distance_config, SMALL_TANK_MAX_PROFILE);
			acc_detector_distance_config_num_frames_recorded_threshold_set(app_config->distance_config, SMALL_TANK_NUM_FRAMES_REC);
			acc_detector_distance_config_peak_sorting_set(app_config->distance_config, SMALL_TANK_PEAK_SORTING);
			acc_detector_distance_config_reflector_shape_set(app_config->distance_config, SMALL_TANK_REFLECTOR_SHAPE);
			acc_detector_distance_config_threshold_sensitivity_set(app_config->distance_config, SMALL_TANK_THRESHOLD_SENSITIVITY);
			acc_detector_distance_config_signal_quality_set(app_config->distance_config, SMALL_TANK_SIGNAL_QUALITY);
			acc_detector_distance_config_close_range_leakage_cancellation_set(app_config->distance_config,
			                                                                  SMALL_TANK_CLOSE_RANGE_LEAKAGE_CANCELLATION);
			break;
		case TANK_LEVEL_PRESET_CONFIG_MEDIUM_TANK:
			app_config->tank_range_start_m     = MEDIUM_TANK_RANGE_START_M;
			app_config->tank_range_end_m       = MEDIUM_TANK_RANGE_END_M;
			app_config->median_filter_length   = MEDIUM_TANK_MEDIAN_FILTER_LENGTH;
			app_config->num_medians_to_average = MEDIUM_TANK_NUM_MEDIANS_TO_AVERAGE;

			acc_detector_distance_config_start_set(app_config->distance_config, get_detector_start_m(app_config));
			acc_detector_distance_config_end_set(app_config->distance_config, get_detector_end_m(app_config));
			acc_detector_distance_config_max_step_length_set(app_config->distance_config, MEDIUM_TANK_MAX_STEP_LENGTH);
			acc_detector_distance_config_max_profile_set(app_config->distance_config, MEDIUM_TANK_MAX_PROFILE);
			acc_detector_distance_config_num_frames_recorded_threshold_set(app_config->distance_config, MEDIUM_TANK_NUM_FRAMES_REC);
			acc_detector_distance_config_peak_sorting_set(app_config->distance_config, MEDIUM_TANK_PEAK_SORTING);
			acc_detector_distance_config_reflector_shape_set(app_config->distance_config, MEDIUM_TANK_REFLECTOR_SHAPE);
			acc_detector_distance_config_threshold_sensitivity_set(app_config->distance_config, MEDIUM_TANK_THRESHOLD_SENSITIVITY);
			acc_detector_distance_config_signal_quality_set(app_config->distance_config, MEDIUM_TANK_SIGNAL_QUALITY);
			acc_detector_distance_config_close_range_leakage_cancellation_set(app_config->distance_config,
			                                                                  MEDIUM_TANK_CLOSE_RANGE_LEAKAGE_CANCELLATION);
			break;
		case TANK_LEVEL_PRESET_CONFIG_LARGE_TANK:
			app_config->tank_range_start_m     = LARGE_TANK_RANGE_START_M;
			app_config->tank_range_end_m       = LARGE_TANK_RANGE_END_M;
			app_config->median_filter_length   = LARGE_TANK_MEDIAN_FILTER_LENGTH;
			app_config->num_medians_to_average = LARGE_TANK_NUM_MEDIANS_TO_AVERAGE;

			acc_detector_distance_config_start_set(app_config->distance_config, get_detector_start_m(app_config));
			acc_detector_distance_config_end_set(app_config->distance_config, get_detector_end_m(app_config));
			acc_detector_distance_config_max_step_length_set(app_config->distance_config, LARGE_TANK_MAX_STEP_LENGTH);
			acc_detector_distance_config_max_profile_set(app_config->distance_config, LARGE_TANK_MAX_PROFILE);
			acc_detector_distance_config_num_frames_recorded_threshold_set(app_config->distance_config, LARGE_TANK_NUM_FRAMES_REC);
			acc_detector_distance_config_peak_sorting_set(app_config->distance_config, LARGE_TANK_PEAK_SORTING);
			acc_detector_distance_config_reflector_shape_set(app_config->distance_config, LARGE_TANK_REFLECTOR_SHAPE);
			acc_detector_distance_config_threshold_sensitivity_set(app_config->distance_config, LARGE_TANK_THRESHOLD_SENSITIVITY);
			acc_detector_distance_config_signal_quality_set(app_config->distance_config, LARGE_TANK_SIGNAL_QUALITY);
			acc_detector_distance_config_close_range_leakage_cancellation_set(app_config->distance_config,
			                                                                  LARGE_TANK_CLOSE_RANGE_LEAKAGE_CANCELLATION);
			break;
	}
}


static bool initialize_application_resources(app_context_t *context)
{
	context->detector_handle = acc_detector_distance_create(context->app_config->distance_config);
	if (context->detector_handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		return false;
	}

	if (!acc_detector_distance_get_buffer_sizes(context->detector_handle, &(context->buffer_size), &(context->calibration_buffer_size)))
	{
		printf("acc_detector_distance_get_buffer_sizes() failed\n");
		return false;
	}

	context->buffer = acc_integration_mem_alloc(context->buffer_size);
	if (context->buffer == NULL)
	{
		printf("sensor buffer allocation failed\n");
		return false;
	}

	context->calibration_buffer = acc_integration_mem_alloc(context->calibration_buffer_size);
	if (context->calibration_buffer == NULL)
	{
		printf("calibration buffer allocation failed\n");
		return false;
	}

	context->level_history_length = context->app_config->median_filter_length;

	context->level_history = acc_integration_mem_alloc(context->level_history_length * sizeof(*context->level_history));
	if (context->level_history == NULL)
	{
		printf("level history buffer allocation failed\n");
		return false;
	}

	context->median_vector_length = context->app_config->num_medians_to_average;

	context->median_vector = acc_integration_mem_alloc(context->median_vector_length * sizeof(*context->median_vector));
	if (context->median_vector == NULL)
	{
		printf("median vector allocation failed\n");
		return false;
	}

	context->median_counter           = 0U;
	context->mean_counter             = 0U;
	context->median_edge_status_count = 0U;
	context->mean_edge_status_count   = 0U;

	return true;
}


static bool sensor_calibration(acc_sensor_t     *sensor,
                               acc_cal_result_t *sensor_cal_result,
                               void             *buffer,
                               uint32_t         buffer_size)
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


static bool full_detector_calibration(app_context_t          *context,
                                      const acc_cal_result_t *sensor_cal_result)
{
	bool done = false;
	bool status;

	do
	{
		status = acc_detector_distance_calibrate(context->sensor,
		                                         context->detector_handle,
		                                         sensor_cal_result,
		                                         context->buffer,
		                                         context->buffer_size,
		                                         context->calibration_buffer,
		                                         context->calibration_buffer_size,
		                                         &done);
	} while (status && !done && acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS));

	return status;
}


static bool detector_recalibration(app_context_t          *context,
                                   const acc_cal_result_t *sensor_cal_result)
{
	bool done = false;
	bool status;

	do
	{
		status = acc_detector_distance_recalibrate(context->sensor,
		                                           context->detector_handle,
		                                           sensor_cal_result,
		                                           context->buffer,
		                                           context->buffer_size,
		                                           context->calibration_buffer,
		                                           context->calibration_buffer_size,
		                                           &done);
	} while (status && !done && acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS));

	return status;
}


static bool detector_get_next(app_context_t                  *context,
                              const acc_cal_result_t         *sensor_cal_result,
                              acc_detector_distance_result_t *detector_result)
{
	bool result_available = false;

	do
	{
		if (!acc_detector_distance_prepare(context->detector_handle, context->app_config->distance_config, context->sensor, sensor_cal_result,
		                                   context->buffer,
		                                   context->buffer_size))
		{
			printf("acc_detector_distance_prepare() failed\n");
			return false;
		}

		if (!acc_sensor_measure(context->sensor))
		{
			printf("acc_sensor_measure() failed\n");
			return false;
		}

		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("Sensor interrupt timeout\n");
			return false;
		}

		if (!acc_sensor_read(context->sensor, context->buffer, context->buffer_size))
		{
			printf("acc_sensor_read() failed\n");
			return false;
		}

		if (!acc_detector_distance_process(context->detector_handle, context->buffer, context->calibration_buffer, &result_available,
		                                   detector_result))
		{
			printf("acc_detector_distance_process() failed\n");
			return false;
		}
	} while (!result_available);

	return true;
}


static void swap(float *array, uint16_t idx_a, uint16_t idx_b)
{
	float tmp = array[idx_a];

	array[idx_a] = array[idx_b];
	array[idx_b] = tmp;
}


static void sort(float *array, uint16_t array_length)
{
	for (uint16_t i = 0; i < array_length - 1; i++)
	{
		for (uint16_t j = 0; j < array_length - i - 1; j++)
		{
			if (array[j] > array[j + 1])
			{
				swap(array, j, j + 1);
			}
		}
	}
}


static float median(float *array, uint16_t array_length)
{
	bool  status = true;
	float result = 0.0f;

	for (uint16_t i = 0U; i < array_length && status; i++)
	{
		if (isnan(array[i]))
		{
			result = (float)NAN;
			status = false;
		}
	}

	if (status)
	{
		sort(array, array_length);

		if (array_length % 2U == 0U)
		{
			uint16_t n = array_length / 2U;
			result = (array[n - 1] + array[n]) / 2.0f;
		}
		else
		{
			uint16_t n = array_length / 2;
			result = array[n];
		}
	}

	return result;
}


static float nanmean(float *array, uint16_t array_length)
{
	uint16_t samples = 0U;
	float    sum     = 0.0f;

	for (uint16_t i = 0U; i < array_length; i++)
	{
		if (!isnan(array[i]))
		{
			samples++;
			sum += array[i];
		}
	}

	return samples > 0U ? sum / (float)samples : (float)NAN;
}


static void process_detector_result(const acc_detector_distance_result_t *distance_result, app_result_t *app_result,
                                    app_context_t *context)
{
	app_result->peak_detected = false;
	app_result->peak_status   = PEAK_STATUS_NO_DETECTION;
	app_result->level         = 0.0f;
	app_result->result_ready  = false;
	float level = 0.0f;

	if (distance_result->num_distances > 0)
	{
		app_result->peak_detected = true;
		level                     = context->app_config->tank_range_end_m - distance_result->distances[0];
	}
	else
	{
		level = (float)NAN;
	}

	if (distance_result->near_start_edge_status)
	{
		context->median_edge_status_count++;
	}

	context->level_history[context->median_counter++] = level;

	if (context->median_counter == context->level_history_length)
	{
		float med = median(context->level_history, context->level_history_length);

		context->median_vector[context->mean_counter++] = med;

		if (context->median_edge_status_count > context->level_history_length / 2)
		{
			context->mean_edge_status_count++;
		}

		context->median_counter           = 0U;
		context->median_edge_status_count = 0U;
	}

	if (context->mean_counter == context->median_vector_length)
	{
		level = nanmean(context->median_vector, context->median_vector_length);

		if (!isnan(level))
		{
			if (level < 0.0f)
			{
				app_result->peak_status = PEAK_STATUS_OUT_OF_RANGE;
			}
			else if (level > (context->app_config->tank_range_end_m - context->app_config->tank_range_start_m))
			{
				app_result->peak_status = PEAK_STATUS_OVERFLOW;
			}
			else
			{
				app_result->peak_status = PEAK_STATUS_IN_RANGE;
				app_result->level       = level;
			}
		}
		else if (context->mean_edge_status_count > context->median_vector_length / 2)
		{
			app_result->peak_status = PEAK_STATUS_OVERFLOW;
		}

		if (app_result->peak_status == PEAK_STATUS_OVERFLOW || app_result->peak_status == PEAK_STATUS_OUT_OF_RANGE)
		{
			app_result->level = (float)NAN;
		}

		app_result->result_ready        = true;
		context->mean_counter           = 0U;
		context->mean_edge_status_count = 0U;
	}
}


static void print_result(const app_result_t *app_result, const acc_ref_app_tank_level_config_t *app_config)
{
	switch (app_result->peak_status)
	{
		case PEAK_STATUS_IN_RANGE:
			printf("Level within range\n");
			printf("Level: %" PRIfloat " cm, %" PRIfloat " %%\n", ACC_LOG_FLOAT_TO_INTEGER(
				       app_result->level * 100.0f),
			       ACC_LOG_FLOAT_TO_INTEGER((app_result->level /
			                                 (app_config->tank_range_end_m - app_config->tank_range_start_m) *
			                                 100.f)));
			break;
		case PEAK_STATUS_NO_DETECTION:
			printf("No detection\n");
			break;
		case PEAK_STATUS_OVERFLOW:
			printf("Overflow!\n");
			break;
		case PEAK_STATUS_OUT_OF_RANGE:
			printf("Out of range\n");
			break;
		default:
			break;
	}
}
