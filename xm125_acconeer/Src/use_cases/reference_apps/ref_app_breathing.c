// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "acc_alg_basic_utils.h"
#include "acc_algorithm.h"
#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_detector_presence.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_processing.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"
#include "acc_version.h"


typedef enum
{
	BREATHING_PRESET_NONE,
	BREATHING_PRESET_SITTING,
	BREATHING_PRESET_INFANT,
} breathing_preset_t;

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

#define ACC_ALGORITHM_APPROX_BASE_STEP_LENGTH_M (2.5e-3f)

#define DEFAULT_PRESET_CONFIG BREATHING_PRESET_SITTING

#define B_STATIC_LENGTH (3U)
#define A_STATIC_LENGTH (2U)
#define B_ANGLE_LENGTH  (5U)
#define A_ANGLE_LENGTH  (4U)

/**
 * @brief Breathing application config container
 */
typedef struct
{
	/**
	 * Length of time series
	 */
	uint16_t time_series_length_s;
	/**
	 * Lowest anticipated breathing rate (breaths per minute)
	 */
	uint16_t lowest_breathing_rate;
	/**
	 * Highest anticipated breathing rate (breaths per minute)
	 */
	uint16_t highest_breathing_rate;
	/**
	 * Number of distance to analyze in breathing
	 */
	uint16_t num_dists_to_analyze;
	/**
	 * Use presence detector to determine distance to motion
	 */
	bool use_presence_processor;
	/**
	 * Time to determine distance to presence
	 */
	uint16_t distance_determination_duration_s;
	/**
	 * Presence config
	 */
	acc_detector_presence_config_t *presence_config;
} acc_breathing_config_t;

typedef enum
{
	ACC_BREATHING_APP_STATE_INIT,
	ACC_BREATHING_APP_STATE_NO_PRESENCE,
	ACC_BREATHING_APP_STATE_INTRA_PRESENCE,
	ACC_BREATHING_APP_STATE_DETERMINE_DISTANCE,
	ACC_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE,
} acc_breathing_app_state_t;


/**
 * @brief Breathing application context handle
 */
typedef struct
{
	acc_sensor_t                   *sensor;
	acc_cal_result_t               sensor_cal_result;
	acc_detector_presence_handle_t *presence_handle;
	void                           *buffer;
	uint32_t                       buffer_size;

	float    start_m;
	float    step_length_m;
	int32_t  start_point;
	uint16_t num_points_to_analyze_half_width;
	uint16_t num_points_to_analyze;
	uint16_t end_point;
	float    frame_rate;
	float    lowest_freq;
	float    highest_freq;
	uint16_t use_presence_processor;
	uint16_t distance_determination_count;
	uint16_t time_series_length_s;
	uint16_t time_series_length;
	uint16_t padded_time_series_length_shift;
	uint16_t padded_time_series_length;
	uint16_t num_points;
	uint16_t sweeps_per_frame;
	float    intra_detection_threshold;

	acc_breathing_app_state_t app_state;
	acc_breathing_app_state_t prev_app_state;

	float presence_sf;
	float breathing_sf;

	float b_static[B_STATIC_LENGTH];
	float a_static[A_STATIC_LENGTH];
	float b_angle[B_ANGLE_LENGTH];
	float a_angle[A_ANGLE_LENGTH];

	float complex *mean_sweep;
	float complex *sparse_iq_buffer;
	float complex *filt_sparse_iq_buffer;
	float complex *filt_sparse_iq;
	float         *angle;
	float         *prev_angle;
	float         *lp_filt_ampl;
	float         *unwrapped_angle;
	float         *angle_buffer;
	float         *filt_angle_buffer;
	float         *breathing_motion_buffer;
	float         *hamming_window;
	float         *windowed_breathing_motion_buffer;
	float complex *rfft_output;
	uint16_t      rfft_output_length;
	float         *weighted_psd;
	float         freq_delta;

	uint16_t distance_determination_counter;
	bool     presence_init;
	float    presence_distance;
	bool     base_presence_dist;
	float    base_presence_distance;
	float    presence_distance_threshold;
	bool     first;
	uint16_t init_count;
	uint16_t count;
	bool     initialized;
	uint16_t count_limit;
} acc_breathing_handle_t;


/**
 * @brief Breathing application results container
 */
typedef struct
{
	/**
	 * Indication when a new breathing rate result is produced
	 */
	bool result_ready;
	/**
	 * Breathing rate in BPM
	 */
	float breathing_rate;
} acc_breathing_result_t;


static void cleanup(acc_breathing_handle_t *handle, acc_breathing_config_t *config);


static void set_config(acc_breathing_config_t *config, breathing_preset_t preset);


static bool validate_config(acc_breathing_config_t *config);


static bool init_handle(acc_breathing_handle_t *handle, acc_breathing_config_t *config);


static bool init_sensor_resources(acc_breathing_handle_t *handle, acc_breathing_config_t *config);


static bool sensor_calibration(acc_breathing_handle_t *handle);


static bool reinit_breathing(acc_breathing_handle_t *handle, uint16_t start_point, uint16_t end_point);


static bool measure(acc_breathing_handle_t *handle, acc_detector_presence_result_t *presence_result);


static bool enter_hibernate(acc_sensor_t *sensor);


static bool exit_hibernate(acc_sensor_t *sensor);


static void determine_state(acc_breathing_handle_t *handle, acc_detector_presence_result_t *presence_result);


static void update_presence_distance(acc_breathing_handle_t *handle, float presence_distance);


static bool perform_action_based_on_state(acc_breathing_handle_t *handle, acc_int16_complex_t *frame, acc_breathing_result_t *result);


static bool process_breathing(acc_breathing_handle_t *handle, acc_int16_complex_t *frame, acc_breathing_result_t *result);


static void print_app_state(acc_breathing_handle_t *handle);


static void print_result(acc_breathing_result_t *result);


static bool check_indications(acc_breathing_handle_t *handle, acc_breathing_config_t *config, acc_detector_presence_result_t *presence_result,
                              bool *recalibration_done);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_breathing_config_t config = { 0 };
	acc_breathing_handle_t handle = { 0 };

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	config.presence_config = acc_detector_presence_config_create();
	if (config.presence_config == NULL)
	{
		printf("acc_config_create() failed\n");
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	set_config(&config, DEFAULT_PRESET_CONFIG);

	if (!validate_config(&config))
	{
		printf("Failed to validate config\n");
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	if (!init_handle(&handle, &config))
	{
		printf("Failed to initialize handle\n");
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	uint32_t sleep_time_ms = (uint32_t)(1000.0f / handle.frame_rate);
	acc_integration_set_periodic_wakeup(sleep_time_ms);

	if (!init_sensor_resources(&handle, &config))
	{
		printf("Failed to initialize sensor resources\n");
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	acc_breathing_result_t         result             = { 0 };
	acc_detector_presence_result_t presence_result    = { 0 };
	bool                           recalibration_done = false;

	if (!enter_hibernate(handle.sensor))
	{
		printf("enter_hibernate failed\n");
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	while (true)
	{
		if (!measure(&handle, &presence_result))
		{
			cleanup(&handle, &config);
			return EXIT_FAILURE;
		}

		if (!check_indications(&handle, &config, &presence_result, &recalibration_done))
		{
			cleanup(&handle, &config);
			return EXIT_FAILURE;
		}

		if (recalibration_done)
		{
			recalibration_done            = false;
			handle.base_presence_dist     = false;
			handle.base_presence_distance = 0.0f;
		}
		else
		{
			determine_state(&handle, &presence_result);

			update_presence_distance(&handle, presence_result.presence_distance);

			if (!perform_action_based_on_state(&handle, presence_result.processing_result.frame, &result))
			{
				cleanup(&handle, &config);
				return EXIT_FAILURE;
			}

			if (handle.prev_app_state != handle.app_state)
			{
				print_app_state(&handle);
			}

			print_result(&result);
		}

		handle.prev_app_state = handle.app_state;

		acc_integration_sleep_until_periodic_wakeup();
	}

	cleanup(&handle, &config);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(acc_breathing_handle_t *handle, acc_breathing_config_t *config)
{
	acc_sensor_hibernate_off(handle->sensor);
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (config->presence_config != NULL)
	{
		acc_detector_presence_config_destroy(config->presence_config);
	}

	if (handle->sensor != NULL)
	{
		acc_sensor_destroy(handle->sensor);
	}

	if (handle->presence_handle != NULL)
	{
		acc_detector_presence_destroy(handle->presence_handle);
	}

	if (handle->buffer != NULL)
	{
		acc_integration_mem_free(handle->buffer);
	}

	if (handle->mean_sweep != NULL)
	{
		acc_integration_mem_free(handle->mean_sweep);
	}

	if (handle->filt_sparse_iq != NULL)
	{
		acc_integration_mem_free(handle->filt_sparse_iq);
	}

	if (handle->sparse_iq_buffer != NULL)
	{
		acc_integration_mem_free(handle->sparse_iq_buffer);
	}

	if (handle->filt_sparse_iq_buffer != NULL)
	{
		acc_integration_mem_free(handle->filt_sparse_iq_buffer);
	}

	if (handle->angle != NULL)
	{
		acc_integration_mem_free(handle->angle);
	}

	if (handle->prev_angle != NULL)
	{
		acc_integration_mem_free(handle->prev_angle);
	}

	if (handle->lp_filt_ampl != NULL)
	{
		acc_integration_mem_free(handle->lp_filt_ampl);
	}

	if (handle->unwrapped_angle != NULL)
	{
		acc_integration_mem_free(handle->unwrapped_angle);
	}

	if (handle->angle_buffer != NULL)
	{
		acc_integration_mem_free(handle->angle_buffer);
	}

	if (handle->filt_angle_buffer != NULL)
	{
		acc_integration_mem_free(handle->filt_angle_buffer);
	}

	if (handle->breathing_motion_buffer != NULL)
	{
		acc_integration_mem_free(handle->breathing_motion_buffer);
	}

	if (handle->hamming_window != NULL)
	{
		acc_integration_mem_free(handle->hamming_window);
	}

	if (handle->windowed_breathing_motion_buffer != NULL)
	{
		acc_integration_mem_free(handle->windowed_breathing_motion_buffer);
	}

	if (handle->rfft_output != NULL)
	{
		acc_integration_mem_free(handle->rfft_output);
	}

	if (handle->weighted_psd != NULL)
	{
		acc_integration_mem_free(handle->weighted_psd);
	}
}


static void set_config(acc_breathing_config_t *config, breathing_preset_t preset)
{
	config->time_series_length_s              = 20U;
	config->lowest_breathing_rate             = 6U;
	config->highest_breathing_rate            = 60U;
	config->num_dists_to_analyze              = 3U;
	config->use_presence_processor            = true;
	config->distance_determination_duration_s = 5U;

	acc_detector_presence_config_t *presence_config = config->presence_config;

	acc_detector_presence_config_start_set(presence_config, 0.3f);
	acc_detector_presence_config_hwaas_set(presence_config, 32U);
	acc_detector_presence_config_frame_rate_set(presence_config, 10.0f);
	acc_detector_presence_config_sweeps_per_frame_set(presence_config, 16U);
	acc_detector_presence_config_auto_profile_set(presence_config, false);
	acc_detector_presence_config_profile_set(presence_config, ACC_CONFIG_PROFILE_3);
	acc_detector_presence_config_auto_step_length_set(presence_config, false);
	acc_detector_presence_config_step_length_set(presence_config, 24U);
	acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 0U);
	acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 20.0f);
	acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.5f);
	acc_detector_presence_config_inter_output_time_const_set(presence_config, 5.0f);

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


static bool validate_config(acc_breathing_config_t *config)
{
	float frame_rate = acc_detector_presence_config_frame_rate_get(config->presence_config);

	bool status = true;

	if (frame_rate == 0.0f)
	{
		printf("Frame rate must be set, i.e. > 0.0\n");
		status = false;
	}

	if (config->lowest_breathing_rate >= config->highest_breathing_rate)
	{
		printf("Lowest breathing rate must be lower than highest breathing rate\n");
		status = false;
	}

	if (config->num_dists_to_analyze < 1U)
	{
		printf("Number of distances to analyze must be higher than 1");
		status = false;
	}

	return status;
}


static bool init_handle(acc_breathing_handle_t *handle, acc_breathing_config_t *config)
{
	handle->frame_rate                = acc_detector_presence_config_frame_rate_get(config->presence_config);
	handle->sweeps_per_frame          = acc_detector_presence_config_sweeps_per_frame_get(config->presence_config);
	handle->intra_detection_threshold = acc_detector_presence_config_intra_detection_threshold_get(config->presence_config);

	acc_detector_presence_metadata_t presence_metadata;

	handle->presence_handle = acc_detector_presence_create(config->presence_config, &presence_metadata);

	if (handle->presence_handle == NULL)
	{
		printf("acc_detector_presence_create() failed\n");
		return false;
	}

	handle->start_m       = presence_metadata.start_m;
	handle->step_length_m = presence_metadata.step_length_m;
	handle->num_points    = presence_metadata.num_points;

	handle->lowest_freq                      = (float)config->lowest_breathing_rate / 60.0f;
	handle->highest_freq                     = (float)config->highest_breathing_rate / 60.0f;
	handle->use_presence_processor           = config->use_presence_processor;
	handle->time_series_length_s             = config->time_series_length_s;
	handle->distance_determination_count     = config->distance_determination_duration_s * handle->frame_rate;
	handle->num_points_to_analyze_half_width = config->num_dists_to_analyze / 2U;
	handle->num_points_to_analyze            = config->use_presence_processor ? handle->num_points_to_analyze_half_width * 2U +
	                                           1U : handle->num_points;

	acc_detector_presence_get_buffer_size(handle->presence_handle, &handle->buffer_size);

	handle->time_series_length              = handle->time_series_length_s * handle->frame_rate;
	handle->padded_time_series_length_shift = 0U;
	handle->padded_time_series_length       = 1U << handle->padded_time_series_length_shift;

	while (handle->padded_time_series_length < handle->time_series_length)
	{
		handle->padded_time_series_length_shift++;
		handle->padded_time_series_length = 1U << handle->padded_time_series_length_shift;
	}

	handle->rfft_output_length             = (handle->padded_time_series_length / 2U) + 1U;
	handle->distance_determination_counter = 0U;
	handle->presence_init                  = false;
	handle->presence_distance              = 0.0f;
	handle->base_presence_dist             = false;
	handle->base_presence_distance         = 0.0f;
	handle->presence_distance_threshold    = acc_algorithm_get_fwhm(acc_detector_presence_config_profile_get(config->presence_config)) *
	                                         2.0f;
	handle->first       = true;
	handle->init_count  = 0U;
	handle->count       = 0U;
	handle->initialized = false;
	handle->presence_sf = acc_algorithm_exp_smoothing_coefficient(handle->frame_rate,
	                                                              (float)config->distance_determination_duration_s / 4.0f);
	handle->breathing_sf = acc_algorithm_exp_smoothing_coefficient(handle->frame_rate, handle->time_series_length_s / 2.0f);

	handle->app_state      = ACC_BREATHING_APP_STATE_INIT;
	handle->prev_app_state = ACC_BREATHING_APP_STATE_INIT;

	handle->count_limit = handle->time_series_length / 2U;

	acc_algorithm_butter_lowpass(handle->lowest_freq, handle->frame_rate, handle->b_static, handle->a_static);
	acc_algorithm_butter_bandpass(handle->lowest_freq, handle->highest_freq, handle->frame_rate,
	                              handle->b_angle, handle->a_angle);

	handle->buffer           = acc_integration_mem_alloc(handle->buffer_size);
	handle->mean_sweep       = acc_integration_mem_alloc(handle->num_points_to_analyze * sizeof(*handle->mean_sweep));
	handle->filt_sparse_iq   = acc_integration_mem_alloc(handle->num_points_to_analyze * sizeof(*handle->filt_sparse_iq));
	handle->sparse_iq_buffer =
		acc_integration_mem_alloc(B_STATIC_LENGTH * handle->num_points_to_analyze * sizeof(*handle->sparse_iq_buffer));
	handle->filt_sparse_iq_buffer = acc_integration_mem_alloc(A_STATIC_LENGTH * handle->num_points_to_analyze *
	                                                          sizeof(*handle->filt_sparse_iq_buffer));
	handle->angle             = acc_integration_mem_alloc(handle->num_points_to_analyze * sizeof(*handle->angle));
	handle->prev_angle        = acc_integration_mem_alloc(handle->num_points_to_analyze * sizeof(*handle->prev_angle));
	handle->lp_filt_ampl      = acc_integration_mem_alloc(handle->num_points_to_analyze * sizeof(*handle->lp_filt_ampl));
	handle->unwrapped_angle   = acc_integration_mem_alloc(handle->num_points_to_analyze * sizeof(*handle->unwrapped_angle));
	handle->angle_buffer      = acc_integration_mem_alloc(B_ANGLE_LENGTH * handle->num_points_to_analyze * sizeof(*handle->angle_buffer));
	handle->filt_angle_buffer = acc_integration_mem_alloc(
		A_ANGLE_LENGTH * handle->num_points_to_analyze * sizeof(*handle->filt_angle_buffer));
	handle->breathing_motion_buffer =
		acc_integration_mem_alloc(handle->time_series_length * handle->num_points_to_analyze * sizeof(*handle->breathing_motion_buffer));
	handle->hamming_window                   = acc_integration_mem_alloc(handle->time_series_length * sizeof(*handle->hamming_window));
	handle->windowed_breathing_motion_buffer =
		acc_integration_mem_alloc(handle->time_series_length * handle->num_points_to_analyze *
		                          sizeof(*handle->windowed_breathing_motion_buffer));
	handle->rfft_output  = acc_integration_mem_alloc(handle->rfft_output_length * handle->num_points_to_analyze * sizeof(*handle->rfft_output));
	handle->weighted_psd = acc_integration_mem_alloc(handle->rfft_output_length * sizeof(*handle->weighted_psd));

	bool status = handle->buffer != NULL && handle->mean_sweep != NULL && handle->filt_sparse_iq != NULL &&
	              handle->sparse_iq_buffer != NULL && handle->filt_sparse_iq_buffer != NULL && handle->angle != NULL &&
	              handle->prev_angle != NULL && handle->lp_filt_ampl != NULL && handle->unwrapped_angle != NULL && handle->angle_buffer != NULL &&
	              handle->filt_angle_buffer != NULL && handle->breathing_motion_buffer != NULL && handle->hamming_window != NULL &&
	              handle->windowed_breathing_motion_buffer != NULL && handle->rfft_output != NULL && handle->weighted_psd != NULL;

	if (status)
	{
		handle->freq_delta = acc_algorithm_fftfreq_delta(handle->padded_time_series_length, 1.0f / handle->frame_rate);
		acc_algorithm_hamming(handle->time_series_length, handle->hamming_window);
	}

	return status;
}


static bool init_sensor_resources(acc_breathing_handle_t *handle, acc_breathing_config_t *config)
{
	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	handle->sensor = acc_sensor_create(SENSOR_ID);
	if (handle->sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		return false;
	}

	if (!sensor_calibration(handle))
	{
		printf("Sensor calibration failed\n");
		return false;
	}

	if (!acc_detector_presence_prepare(handle->presence_handle, config->presence_config, handle->sensor, &handle->sensor_cal_result,
	                                   handle->buffer,
	                                   handle->buffer_size))
	{
		printf("acc_detector_presence_prepare() failed\n");
		return false;
	}

	return true;
}


static bool sensor_calibration(acc_breathing_handle_t *handle)
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
			status = acc_sensor_calibrate(handle->sensor, &cal_complete, &handle->sensor_cal_result, handle->buffer, handle->buffer_size);

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
		acc_sensor_status(handle->sensor);
	}

	return status;
}


static bool reinit_breathing(acc_breathing_handle_t *handle, uint16_t start_point, uint16_t end_point)
{
	handle->start_point           = start_point;
	handle->end_point             = end_point;
	handle->num_points_to_analyze = end_point - start_point;

	handle->first       = true;
	handle->init_count  = 0U;
	handle->count       = 0U;
	handle->initialized = false;

	memset(handle->sparse_iq_buffer, 0, B_STATIC_LENGTH * handle->num_points_to_analyze * sizeof(*handle->sparse_iq_buffer));
	memset(handle->filt_sparse_iq_buffer, 0, A_STATIC_LENGTH * handle->num_points_to_analyze * sizeof(*handle->filt_sparse_iq_buffer));
	memset(handle->prev_angle, 0, handle->num_points_to_analyze * sizeof(*handle->prev_angle));
	memset(handle->lp_filt_ampl, 0, handle->num_points_to_analyze * sizeof(*handle->lp_filt_ampl));
	memset(handle->unwrapped_angle, 0, handle->num_points_to_analyze * sizeof(*handle->unwrapped_angle));
	memset(handle->angle_buffer, 0, B_ANGLE_LENGTH * handle->num_points_to_analyze * sizeof(*handle->angle_buffer));
	memset(handle->filt_angle_buffer, 0, A_ANGLE_LENGTH * handle->num_points_to_analyze * sizeof(*handle->filt_angle_buffer));
	memset(handle->breathing_motion_buffer, 0,
	       handle->time_series_length * handle->num_points_to_analyze * sizeof(*handle->breathing_motion_buffer));

	return true;
}


static bool measure(acc_breathing_handle_t *handle, acc_detector_presence_result_t *presence_result)
{
	if (!exit_hibernate(handle->sensor))
	{
		printf("exit_hibernate failed\n");
		return EXIT_FAILURE;
	}

	if (!acc_sensor_measure(handle->sensor))
	{
		printf("acc_sensor_measure failed\n");
		acc_sensor_status(handle->sensor);
		return false;
	}

	if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
	{
		printf("Sensor interrupt timeout\n");
		acc_sensor_status(handle->sensor);
		return false;
	}

	if (!acc_sensor_read(handle->sensor, handle->buffer, handle->buffer_size))
	{
		printf("acc_sensor_read() failed\n");
		acc_sensor_status(handle->sensor);
		return false;
	}

	if (!enter_hibernate(handle->sensor))
	{
		printf("enter_hibernate failed\n");
		return EXIT_FAILURE;
	}

	if (!acc_detector_presence_process(handle->presence_handle, handle->buffer, presence_result))
	{
		printf("acc_detector_presence_process() failed\n");
		return false;
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


static void determine_state(acc_breathing_handle_t *handle, acc_detector_presence_result_t *presence_result)
{
	if (!presence_result->presence_detected)
	{
		handle->app_state = ACC_BREATHING_APP_STATE_NO_PRESENCE;
	}
	else if (handle->intra_detection_threshold < presence_result->intra_presence_score)
	{
		handle->app_state = ACC_BREATHING_APP_STATE_INTRA_PRESENCE;
	}
	else if (!handle->base_presence_dist && handle->use_presence_processor)
	{
		handle->app_state = ACC_BREATHING_APP_STATE_DETERMINE_DISTANCE;
	}
	else if (!handle->use_presence_processor || handle->distance_determination_count <= handle->distance_determination_counter)
	{
		handle->app_state = ACC_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE;
	}
	else
	{
		// Do nothing
	}
}


static void update_presence_distance(acc_breathing_handle_t *handle, float presence_distance)
{
	if (!handle->presence_init)
	{
		handle->presence_init     = true;
		handle->presence_distance = presence_distance;
	}

	handle->presence_distance = handle->presence_distance * handle->presence_sf + presence_distance * (1.0f - handle->presence_sf);

	if (handle->base_presence_dist && handle->presence_distance_threshold < fabsf(handle->base_presence_distance - handle->presence_distance))
	{
		handle->base_presence_dist     = false;
		handle->base_presence_distance = 0.0f;
	}
}


static bool perform_action_based_on_state(acc_breathing_handle_t *handle, acc_int16_complex_t *frame, acc_breathing_result_t *result)
{
	bool status = true;

	result->result_ready = false;

	switch (handle->app_state)
	{
		case ACC_BREATHING_APP_STATE_INIT:
			// Do nothing
			break;
		// No presence and intra presence require the same action
		case ACC_BREATHING_APP_STATE_NO_PRESENCE:
		case ACC_BREATHING_APP_STATE_INTRA_PRESENCE:
			handle->base_presence_dist     = false;
			handle->base_presence_distance = 0.0f;
			break;
		case ACC_BREATHING_APP_STATE_DETERMINE_DISTANCE:
			if (handle->app_state != handle->prev_app_state)
			{
				handle->distance_determination_counter = 0U;
			}
			else
			{
				handle->distance_determination_counter++;
				handle->base_presence_dist     = true;
				handle->base_presence_distance = handle->presence_distance;
			}

			break;
		case ACC_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE:
			if (handle->app_state != handle->prev_app_state)
			{
				uint16_t start_p;
				uint16_t end_p;

				if (handle->use_presence_processor)
				{
					uint16_t center_idx =
						(uint16_t)(((handle->base_presence_distance - handle->start_m) / handle->step_length_m) + 0.5f);
					start_p = center_idx >=
					          handle->num_points_to_analyze_half_width ? (uint16_t)(center_idx -
					                                                                handle->num_points_to_analyze_half_width) : 0U;
					end_p = (center_idx + handle->num_points_to_analyze_half_width + 1U) <=
					        handle->num_points ? (center_idx + handle->num_points_to_analyze_half_width +
					                              1U) : handle->num_points;
				}
				else
				{
					start_p = 0U;
					end_p   = handle->num_points;
				}

				reinit_breathing(handle, start_p, end_p);
			}

			status = process_breathing(handle, frame, result);
			break;
		default:
			// Should never happen
			break;
	}

	return status;
}


static bool process_breathing(acc_breathing_handle_t *handle, acc_int16_complex_t *frame, acc_breathing_result_t *result)
{
	acc_algorithm_mean_sweep(frame, handle->num_points, handle->sweeps_per_frame, handle->start_point, handle->end_point,
	                         handle->mean_sweep);

	acc_algorithm_roll_and_push_f32_complex_matrix(handle->sparse_iq_buffer, B_STATIC_LENGTH, handle->num_points_to_analyze,
	                                               handle->mean_sweep,
	                                               true);

	acc_algorithm_apply_filter_complex(handle->a_static, handle->filt_sparse_iq_buffer, A_STATIC_LENGTH, handle->num_points_to_analyze,
	                                   handle->b_static, handle->sparse_iq_buffer, B_STATIC_LENGTH, handle->num_points_to_analyze,
	                                   handle->filt_sparse_iq, handle->num_points_to_analyze);

	acc_algorithm_roll_and_push_f32_complex_matrix(handle->filt_sparse_iq_buffer, A_STATIC_LENGTH, handle->num_points_to_analyze,
	                                               handle->filt_sparse_iq, true);

	for (uint16_t i = 0U; i < handle->num_points_to_analyze; i++)
	{
		handle->mean_sweep[i] = handle->mean_sweep[i] - handle->filt_sparse_iq[i];
		handle->angle[i]      = cargf(handle->mean_sweep[i]);
	}

	if (handle->first)
	{
		for (uint16_t i = 0U; i < handle->num_points_to_analyze; i++)
		{
			handle->prev_angle[i]   = handle->angle[i];
			handle->lp_filt_ampl[i] = cabsf(handle->mean_sweep[i]);
		}

		handle->first = false;
	}

	for (uint16_t i = 0U; i < handle->num_points_to_analyze; i++)
	{
		handle->lp_filt_ampl[i] = handle->breathing_sf * handle->lp_filt_ampl[i] + (1.0f - handle->breathing_sf) *
		                          cabsf(handle->mean_sweep[i]);
	}

	for (uint16_t i = 0U; i < handle->num_points_to_analyze; i++)
	{
		float angle_diff = handle->angle[i] - handle->prev_angle[i];
		handle->prev_angle[i] = handle->angle[i];

		if ((float)M_PI < angle_diff)
		{
			angle_diff -= 2.0f * (float)M_PI;
		}
		else if (angle_diff < -(float)M_PI)
		{
			angle_diff += 2.0f * (float)M_PI;
		}

		handle->unwrapped_angle[i] += angle_diff;
	}

	acc_algorithm_roll_and_push_f32_matrix(handle->angle_buffer, B_ANGLE_LENGTH, handle->num_points_to_analyze, handle->unwrapped_angle,
	                                       true);

	acc_algorithm_apply_filter(handle->a_angle, handle->filt_angle_buffer, A_ANGLE_LENGTH, handle->num_points_to_analyze, handle->b_angle,
	                           handle->angle_buffer, B_ANGLE_LENGTH, handle->num_points_to_analyze, handle->angle,
	                           handle->num_points_to_analyze);

	acc_algorithm_roll_and_push_f32_matrix(handle->filt_angle_buffer, A_ANGLE_LENGTH, handle->num_points_to_analyze, handle->angle, true);

	acc_algorithm_roll_and_push_f32_matrix(handle->breathing_motion_buffer, handle->time_series_length, handle->num_points_to_analyze,
	                                       handle->angle,
	                                       false);

	if (handle->init_count > handle->time_series_length)
	{
		handle->initialized = true;
	}
	else
	{
		handle->init_count++;
	}

	if (handle->time_series_length - handle->count_limit <= handle->count)
	{
		handle->count = 0;

		if (handle->initialized)
		{
			float lp_filt_ampl_sum = 0U;

			for (uint16_t r = 0U; r < handle->time_series_length; r++)
			{
				for (uint16_t c = 0U; c < handle->num_points_to_analyze; c++)
				{
					handle->windowed_breathing_motion_buffer[r * handle->num_points_to_analyze +
					                                         c] =
						handle->breathing_motion_buffer[r * handle->num_points_to_analyze + c] *
						handle->hamming_window[r];
					lp_filt_ampl_sum += handle->lp_filt_ampl[c];
				}
			}

			acc_algorithm_rfft_matrix(handle->windowed_breathing_motion_buffer, handle->time_series_length,
			                          handle->num_points_to_analyze, handle->padded_time_series_length_shift,
			                          handle->rfft_output, 0U);

			for (uint16_t r = 0U; r < handle->rfft_output_length; r++)
			{
				float sum_psd = 0U;

				for (uint16_t c = 0U; c < handle->num_points_to_analyze; c++)
				{
					sum_psd += cabsf(handle->rfft_output[r * handle->num_points_to_analyze + c]) * handle->lp_filt_ampl[c];
				}

				handle->weighted_psd[r] = sum_psd / lp_filt_ampl_sum;
			}

			uint16_t peak_loc = acc_algorithm_argmax(handle->weighted_psd, handle->rfft_output_length);

			if (peak_loc > 0U)
			{
				float freq = acc_algorithm_interpolate_peaks_equidistant(handle->weighted_psd, 0.0f, handle->freq_delta, peak_loc);
				result->result_ready   = true;
				result->breathing_rate = freq * 60.0f;
			}
		}
	}
	else
	{
		handle->count++;
	}

	return true;
}


static void print_app_state(acc_breathing_handle_t *handle)
{
	switch (handle->app_state)
	{
		case ACC_BREATHING_APP_STATE_INIT:
			break;
		case ACC_BREATHING_APP_STATE_NO_PRESENCE:
			printf("NO_PRESENCE_DETECTED\n");
			break;
		case ACC_BREATHING_APP_STATE_INTRA_PRESENCE:
			printf("INTRA_PRESENCE_DETECTED\n");
			break;
		case ACC_BREATHING_APP_STATE_DETERMINE_DISTANCE:
			printf("DETERMINE_DISTANCE_ESTIMATE\n");
			break;
		case ACC_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE:
			printf("ESTIMATE_BREATHING_RATE\n");
			break;
		default:
			break;
	}
}


static void print_result(acc_breathing_result_t *result)
{
	if (result->result_ready)
	{
		printf("Breaths: %" PRIu16 " bpm\n", (uint16_t)result->breathing_rate);
	}
}


static bool check_indications(acc_breathing_handle_t *handle, acc_breathing_config_t *config, acc_detector_presence_result_t *presence_result,
                              bool *recalibration_done)
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

		if (!sensor_calibration(handle))
		{
			printf("Sensor calibration failed\n");
			return false;
		}

		printf("Sensor recalibration done!\n");

		// Before measuring again, the sensor needs to be prepared through the detector
		if (!acc_detector_presence_prepare(handle->presence_handle, config->presence_config, handle->sensor, &handle->sensor_cal_result,
		                                   handle->buffer,
		                                   handle->buffer_size))
		{
			printf("acc_detector_presence_prepare() failed\n");
			return false;
		}

		*recalibration_done = true;
	}

	return true;
}
