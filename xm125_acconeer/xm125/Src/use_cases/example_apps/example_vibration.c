// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "acc_alg_basic_utils.h"
#include "acc_algorithm.h"
#include "acc_config.h"
#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_integration_log.h"
#include "acc_processing.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"


/** \example example_vibration.c
 * @brief This is an example on how the Service API can be used to measure
 *        vibrations
 * @n
 * The example executes as follows:
 *   - Create a configuration
 *   - Create a processing instance using the previously created configuration
 *   - Create a sensor instance
 *   - Prepare a sensor
 *   - Perform a sensor measurement and read out the data
 *   - Process the measurement and print vibration result
 *   - Check 'calibration_needed' indication
 *   - Destroy the sensor instance
 *   - Destroy the processing instance
 *   - Destroy the configuration
 */

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

typedef enum
{
	ACC_VIBRATION_REPORT_DISPLACEMENT_AS_AMPLITUDE,
	ACC_VIBRATION_REPORT_DISPLACEMENT_AS_PEAK2PEAK,
} acc_vibration_reported_displacement_mode_t;

/**
 * @brief Vibration example config container
 */
typedef struct
{
	/** Length of time series */
	uint16_t time_series_length;

	/** Filter coefficient of exponential filter */
	float lp_coeff;

	/** Threshold sensitivity (um) */
	float threshold_sensitivity;

	/** Minimum amplitude for calculating vibration */
	float amplitude_threshold;

	/** Reported displacement mode: amplitude or peak2peak */
	acc_vibration_reported_displacement_mode_t reported_displacement_mode;

	/** Sensor config */
	acc_config_t *sensor_config;
} acc_vibration_config_t;


//=============================================================================
// Default application configuration (continuous data acquisition)
//
// Data is delivered as a continuous stream of sweeps. This will set a limit on
// the time that can be used for inter sweep data processing, but will give
// a smooth data series without any gaps.
//=============================================================================
#define APP_CONFIG_TIME_SERIES_LENGTH         (1024U)
#define APP_CONFIG_TIME_FILTERING_COEFFICIENT (0.95f)
#define APP_CONFIG_THRESHOLD_SENSITIVITY      (10.0f)
#define APP_CONFIG_AMPLITUDE_THRESHOLD        (100.0f)
#define APP_CONFIG_REPORTED_DISPLACEMENT_MODE (ACC_VIBRATION_REPORT_DISPLACEMENT_AS_AMPLITUDE)
#define APP_CONFIG_SWEEPS_PER_FRAME           (50U)
#define APP_CONFIG_SWEEP_RATE                 (2000U)
#define APP_CONFIG_FRAME_RATE                 (0.0f)
#define APP_CONFIG_INTER_SWEEP_IDLE_STATE     (ACC_CONFIG_IDLE_STATE_READY)
#define APP_CONFIG_INTER_FRAME_IDLE_STATE     (ACC_CONFIG_IDLE_STATE_READY)
#define APP_CONFIG_CONTINUOUS_SWEEP_MODE      (true)
#define APP_CONFIG_DOUBLE_BUFFERING           (true)
#define APP_CONFIG_START_POINT                (80U)
#define APP_CONFIG_STEP_LENGTH                (1U)
#define APP_CONFIG_HWAAS                      (16U)
#define APP_CONFIG_PROFILE                    (ACC_CONFIG_PROFILE_3)
#define APP_CONFIG_RECEIVER_GAIN              (10U)

//=============================================================================
// Alternative application configuration (data is generated on-demand)
//=============================================================================
#define APP_CONFIG_DATA_ON_DEMAND (false)

#define APP_CONFIG_DATA_ON_DEMAND_SWEEPS_PER_FRAME      (APP_CONFIG_TIME_SERIES_LENGTH)
#define APP_CONFIG_DATA_ON_DEMAND_CONTINUOUS_SWEEP_MODE (false)
#define APP_CONFIG_DATA_ON_DEMAND_DOUBLE_BUFFERING      (false)

//=============================================================================

#define CFAR_WINDOW_LENGTH     (10U)
#define CFAR_HALF_GUARD_LENGTH (5U)
#define CFAR_MARGIN            (CFAR_WINDOW_LENGTH + CFAR_HALF_GUARD_LENGTH)

#define DISPLACEMENT_HISTORY_COLUMN_WIDTH (16U)

#define IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)


typedef struct
{
	float max_sweep_amplitude;
	float max_displacement;
	float max_displacement_freq;
	float time_series_std;
} acc_vibration_result_t;

typedef struct
{
	acc_sensor_t *sensor;

	acc_processing_t          *processing;
	acc_processing_metadata_t proc_meta;
	acc_processing_result_t   proc_result;

	void     *rss_buffer;
	uint32_t rss_buffer_size;

	bool  continuous_data_acquisition;
	float psd_to_radians_conversion_factor;
	float radians_to_displacement;
	float displacement_conversion_factor;

	bool has_init;

	float *time_series;
	float *frequencies;
	float *lp_displacements;

	int32_t *double_buffer_filter_buffer;

	uint16_t frame_length;
	uint16_t padded_time_series_length;
	uint16_t rfft_length_shift;
	uint16_t rfft_output_length;
	uint16_t rfft_read_offset;
	uint16_t data_length;

	float         *zero_mean_time_series;
	complex float *rfft_output;
	float         *threshold;
} acc_vibration_app_t;


static void set_config(acc_vibration_config_t *config);


static bool validate_config(acc_vibration_config_t *config);


static bool init_sensor_resources(acc_vibration_app_t *app, acc_vibration_config_t *config);


static bool init_app_resources(acc_vibration_app_t *app, acc_vibration_config_t *config);


static void setup_sample_frequencies(acc_vibration_app_t *app, float sweep_rate);


static bool do_sensor_calibration_and_prepare(acc_vibration_app_t *app, acc_vibration_config_t *config);


static bool do_sensor_measurement(acc_vibration_app_t *app);


static void process(acc_vibration_app_t *app, acc_vibration_config_t *config, acc_vibration_result_t *result);


static void update_vibration_result(acc_vibration_app_t *app, acc_vibration_config_t *config, acc_vibration_result_t *result);


static void roll_and_update_time_series(acc_vibration_app_t *app, acc_vibration_config_t *config);


static void calculate_threshold(acc_vibration_app_t *app, acc_vibration_config_t *config);


static float *get_zero_mean_time_series(acc_vibration_app_t *app, acc_vibration_config_t *config);


static float calculate_time_series_std(float *zm_time_series, uint16_t zm_time_series_length, float radians_to_displacement);


static void setup_rfft_bounds(acc_vibration_app_t *app, acc_vibration_config_t *config);


static void print_result(acc_vibration_app_t *app, acc_vibration_result_t *result);


static void cleanup(acc_vibration_app_t *app, acc_vibration_config_t *config);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	acc_vibration_config_t config = {0};
	acc_vibration_app_t    app    = {0};
	acc_vibration_result_t result = {0};

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	config.sensor_config = acc_config_create();
	if (config.sensor_config == NULL)
	{
		printf("acc_config_create() failed\n");
		cleanup(&app, &config);
		return EXIT_FAILURE;
	}

	set_config(&config);

	// Print the configuration
	acc_config_log(config.sensor_config);

	if (!validate_config(&config))
	{
		printf("Invalid sensor config\n");
		cleanup(&app, &config);
		return EXIT_FAILURE;
	}

	if (!init_sensor_resources(&app, &config))
	{
		printf("Failed to initialize sensor resources\n");
		cleanup(&app, &config);
		return EXIT_FAILURE;
	}

	if (!init_app_resources(&app, &config))
	{
		printf("Failed to initialize application\n");
		cleanup(&app, &config);
		return EXIT_FAILURE;
	}

	if (!do_sensor_calibration_and_prepare(&app, &config))
	{
		printf("do_sensor_calibration_and_prepare() failed\n");
		acc_sensor_status(app.sensor);
		cleanup(&app, &config);
		return EXIT_FAILURE;
	}

	while (true)
	{
		if (!do_sensor_measurement(&app))
		{
			printf("Failed to do sensor measurement\n");
			cleanup(&app, &config);
			return EXIT_FAILURE;
		}

		if (app.proc_result.calibration_needed)
		{
			printf("The current calibration is not valid for the current temperature.\n");
			printf("The sensor needs to be re-calibrated.\n");

			if (!do_sensor_calibration_and_prepare(&app, &config))
			{
				printf("do_sensor_calibration_and_prepare() failed\n");
				acc_sensor_status(app.sensor);
				cleanup(&app, &config);
				return EXIT_FAILURE;
			}

			printf("The sensor was successfully re-calibrated.\n");
		}
		else
		{
			process(&app, &config, &result);
			print_result(&app, &result);
		}
	}

	cleanup(&app, &config);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void set_config(acc_vibration_config_t *config)
{
	config->time_series_length         = APP_CONFIG_TIME_SERIES_LENGTH;
	config->lp_coeff                   = APP_CONFIG_TIME_FILTERING_COEFFICIENT;
	config->threshold_sensitivity      = APP_CONFIG_THRESHOLD_SENSITIVITY;
	config->amplitude_threshold        = APP_CONFIG_AMPLITUDE_THRESHOLD;
	config->reported_displacement_mode = APP_CONFIG_REPORTED_DISPLACEMENT_MODE;

	acc_config_profile_set(config->sensor_config, APP_CONFIG_PROFILE);
	acc_config_hwaas_set(config->sensor_config, APP_CONFIG_HWAAS);
	acc_config_num_points_set(config->sensor_config, 1U);  // Must be 1.
	acc_config_step_length_set(config->sensor_config, APP_CONFIG_STEP_LENGTH);
	acc_config_start_point_set(config->sensor_config, APP_CONFIG_START_POINT);
	acc_config_receiver_gain_set(config->sensor_config, APP_CONFIG_RECEIVER_GAIN);
	acc_config_sweep_rate_set(config->sensor_config, APP_CONFIG_SWEEP_RATE);
	acc_config_frame_rate_set(config->sensor_config, APP_CONFIG_FRAME_RATE);
	acc_config_inter_frame_idle_state_set(config->sensor_config, APP_CONFIG_INTER_FRAME_IDLE_STATE);
	acc_config_inter_sweep_idle_state_set(config->sensor_config, APP_CONFIG_INTER_SWEEP_IDLE_STATE);

	if (APP_CONFIG_DATA_ON_DEMAND)
	{
		acc_config_sweeps_per_frame_set(config->sensor_config, APP_CONFIG_DATA_ON_DEMAND_SWEEPS_PER_FRAME);
		acc_config_continuous_sweep_mode_set(config->sensor_config, APP_CONFIG_DATA_ON_DEMAND_CONTINUOUS_SWEEP_MODE);
		acc_config_double_buffering_set(config->sensor_config, APP_CONFIG_DATA_ON_DEMAND_DOUBLE_BUFFERING);
	}
	else
	{
		acc_config_sweeps_per_frame_set(config->sensor_config, APP_CONFIG_SWEEPS_PER_FRAME);
		acc_config_continuous_sweep_mode_set(config->sensor_config, APP_CONFIG_CONTINUOUS_SWEEP_MODE);
		acc_config_double_buffering_set(config->sensor_config, APP_CONFIG_DOUBLE_BUFFERING);
	}
}


static bool validate_config(acc_vibration_config_t *config)
{
	bool success = true;

	if (!IS_POWER_OF_TWO(config->time_series_length))
	{
		printf("**************************** [Warning] *********************************\n");
		printf("** time_series_length should be power of 2 for efficient usage of FFT **\n");
		printf("************************************************************************\n");
	}

	if (config->time_series_length < 2U)
	{
		printf("time_series_length too small\n");
		success = false;
	}

	if (acc_config_sweep_rate_get(config->sensor_config) == 0.0f)
	{
		printf("sweep_rate must be set\n");
		success = false;
	}

	if (acc_config_num_points_get(config->sensor_config) != 1U)
	{
		printf("num_points must be set to 1\n");
		success = false;
	}

	if (acc_config_continuous_sweep_mode_get(config->sensor_config) && !acc_config_double_buffering_get(config->sensor_config))
	{
		printf("double buffering needs to be activated to achieve sufficient sweep rate when using continuous sweep mode\n");
		success = false;
	}

	return success;
}


static bool init_sensor_resources(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	app->processing = acc_processing_create(config->sensor_config, &app->proc_meta);
	if (app->processing == NULL)
	{
		printf("acc_processing_create() failed\n");
		return false;
	}

	if (!acc_rss_get_buffer_size(config->sensor_config, &app->rss_buffer_size))
	{
		printf("acc_rss_get_buffer_size() failed\n");
		return false;
	}

	app->rss_buffer = acc_integration_mem_alloc(app->rss_buffer_size);
	if (app->rss_buffer == NULL)
	{
		printf("buffer allocation failed\n");
		return false;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	app->sensor = acc_sensor_create(SENSOR_ID);
	if (app->sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		return false;
	}

	return true;
}


static bool init_app_resources(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	uint16_t sweeps_per_frame = acc_config_sweeps_per_frame_get(config->sensor_config);
	uint16_t num_points       = acc_config_num_points_get(config->sensor_config);
	float    sweep_rate       = acc_config_sweep_rate_get(config->sensor_config);

	app->has_init = false;

	setup_rfft_bounds(app, config);

	app->frame_length       = sweeps_per_frame * num_points;
	app->rfft_output_length = (app->padded_time_series_length / 2U) + 1U;
	app->rfft_read_offset   = 1U;
	app->data_length        = app->rfft_output_length - app->rfft_read_offset;

	app->continuous_data_acquisition = acc_config_continuous_sweep_mode_get(config->sensor_config);

	/*
	 * speed_of_light = 299792458f
	 * radio_frequency = 60.5e9f
	 * perceived_wavelength = (speed_of_light / radio_frequency) / 2.0f
	 */
	const float perceived_wavelength = 0.0024776f;  // 0.002477623619834711

	app->radians_to_displacement = (perceived_wavelength * 1e6f) / (2.0f * (float)M_PI);

	app->psd_to_radians_conversion_factor =
		2.0f / (app->continuous_data_acquisition ? (float)config->time_series_length : (float)sweeps_per_frame);

	app->displacement_conversion_factor = app->psd_to_radians_conversion_factor * app->radians_to_displacement;

	if (config->reported_displacement_mode == ACC_VIBRATION_REPORT_DISPLACEMENT_AS_PEAK2PEAK)
	{
		app->displacement_conversion_factor *= 2.0f;
	}

	app->time_series = acc_integration_mem_calloc(config->time_series_length, sizeof(*app->time_series));
	if (app->time_series == NULL)
	{
		printf("Failed to allocate memory for time_series array\n");
		return false;
	}

	app->frequencies = acc_integration_mem_calloc(app->rfft_output_length, sizeof(*app->frequencies));
	if (app->frequencies == NULL)
	{
		printf("Failed to allocate memory for frequencies array\n");
		return false;
	}

	setup_sample_frequencies(app, sweep_rate);

	app->double_buffer_filter_buffer =
		acc_integration_mem_calloc(sweeps_per_frame - 2U, sizeof(*app->double_buffer_filter_buffer));
	if (app->double_buffer_filter_buffer == NULL)
	{
		printf("Failed to allocate memory for double_buffer_filter_buffer\n");
		return false;
	}

	app->zero_mean_time_series = acc_integration_mem_calloc(config->time_series_length, sizeof(*app->zero_mean_time_series));
	if (app->zero_mean_time_series == NULL)
	{
		printf("Failed to allocate memory for zero_mean_time_series array\n");
		return false;
	}

	app->rfft_output = acc_integration_mem_calloc(app->rfft_output_length, sizeof(*app->rfft_output));
	if (app->rfft_output == NULL)
	{
		printf("Failed to allocate memory for rfft_output array\n");
		return false;
	}

	app->lp_displacements = acc_integration_mem_calloc(app->data_length, sizeof(*app->lp_displacements));
	if (app->lp_displacements == NULL)
	{
		printf("Failed to allocate memory for lp_displacements array\n");
		return false;
	}

	app->threshold = acc_integration_mem_calloc(app->data_length, sizeof(*app->threshold));
	if (app->threshold == NULL)
	{
		printf("Failed to allocate memory for threshold\n");
		return false;
	}

	return true;
}


static void setup_sample_frequencies(acc_vibration_app_t *app, float sweep_rate)
{
	float sample_spacing = 1.0f / sweep_rate;

	acc_algorithm_rfftfreq(app->padded_time_series_length, sample_spacing, app->frequencies);
}


static bool do_sensor_calibration_and_prepare(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	bool             status       = false;
	bool             cal_complete = false;
	acc_cal_result_t cal_result   = {0};

	const uint16_t calibration_retries = 1U;

	// Random disturbances may cause the calibration to fail. At failure, retry at least once.
	for (uint16_t i = 0; !status && (i <= calibration_retries); i++)
	{
		// Reset sensor before calibration by disabling/enabling it
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);

		do
		{
			status = acc_sensor_calibrate(app->sensor,
			                              &cal_complete,
			                              &cal_result,
			                              app->rss_buffer,
			                              app->rss_buffer_size);

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

		status = acc_sensor_prepare(app->sensor,
		                            config->sensor_config,
		                            &cal_result,
		                            app->rss_buffer,
		                            app->rss_buffer_size);
	}

	return status;
}


static bool do_sensor_measurement(acc_vibration_app_t *app)
{
	if (!acc_sensor_measure(app->sensor))
	{
		printf("acc_sensor_measure failed\n");
		acc_sensor_status(app->sensor);
		return false;
	}

	if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
	{
		printf("Sensor interrupt timeout\n");
		acc_sensor_status(app->sensor);
		return false;
	}

	if (!acc_sensor_read(app->sensor, app->rss_buffer, app->rss_buffer_size))
	{
		printf("acc_sensor_read failed\n");
		acc_sensor_status(app->sensor);
		return false;
	}

	acc_processing_execute(app->processing, app->rss_buffer, &app->proc_result);

	return true;
}


static void process(acc_vibration_app_t *app, acc_vibration_config_t *config, acc_vibration_result_t *result)
{
	const uint16_t num_points       = acc_config_num_points_get(config->sensor_config);
	const uint16_t sweeps_per_frame = acc_config_sweeps_per_frame_get(config->sensor_config);

	result->max_sweep_amplitude   = 0.0f;
	result->max_displacement      = FLT_MAX;
	result->max_displacement_freq = FLT_MAX;
	result->time_series_std       = FLT_MAX;

	/*
	 * Check if an object is close enough in front of the sensor to
	 * reach the configured threshold.
	 */

	for (uint16_t i = 0; i < app->frame_length; i++)
	{
		acc_int16_complex_t point = app->proc_result.frame[i];
		result->max_sweep_amplitude = fmaxf(result->max_sweep_amplitude, cabsf(point.real + (point.imag * I)));
	}

	if (result->max_sweep_amplitude < config->amplitude_threshold)
	{
		// Return without a (complete) result.
		app->has_init = false;
		return;
	}

	/*
	 * Process sensor data.
	 */

	// Handle frame based on whether or not continuous sweep mode is used.
	if (app->continuous_data_acquisition)
	{
		acc_algorithm_double_buffering_frame_filter(app->proc_result.frame, sweeps_per_frame, num_points,
		                                            app->double_buffer_filter_buffer);
		roll_and_update_time_series(app, config);
	}
	else
	{
		for (uint16_t i = 0; i < app->frame_length; i++)
		{
			acc_int16_complex_t point = app->proc_result.frame[i];
			app->time_series[i] = cargf(point.real + (point.imag * I));
		}

		acc_algorithm_unwrap(app->time_series, app->frame_length);
	}

	/*
	 * Estimate displacement per frequency.
	 */

	float *zero_mean_time_series = get_zero_mean_time_series(app, config);

	acc_algorithm_rfft(zero_mean_time_series, config->time_series_length, app->rfft_length_shift, app->rfft_output);

	for (uint16_t i = 0; i < app->data_length; i++)
	{
		float z_abs        = cabsf(app->rfft_output[i + app->rfft_read_offset]);
		float displacement = z_abs * app->displacement_conversion_factor;

		if (app->has_init)
		{
			app->lp_displacements[i] =
				app->lp_displacements[i] * config->lp_coeff + displacement * (1.0f - config->lp_coeff);
		}
		else
		{
			app->lp_displacements[i] = displacement;
		}
	}

	app->has_init = true;

	result->time_series_std = calculate_time_series_std(zero_mean_time_series,
	                                                    config->time_series_length,
	                                                    app->radians_to_displacement);

	update_vibration_result(app, config, result);
}


static void update_vibration_result(acc_vibration_app_t *app, acc_vibration_config_t *config, acc_vibration_result_t *result)
{
	/*
	 * Compare displacements to threshold (exclude first point as it
	 * does not form a peak).
	 */

	calculate_threshold(app, config);

	float    max_displacement     = 0.0f;
	uint16_t max_displacement_idx = 0;

	result->max_displacement = 0.0f;

	for (uint16_t i = 1U; i < app->data_length; i++)
	{
		if (app->lp_displacements[i] > app->threshold[i])
		{
			max_displacement = result->max_displacement;

			result->max_displacement = fmaxf(result->max_displacement, app->lp_displacements[i]);

			if (result->max_displacement > max_displacement)
			{
				max_displacement_idx = i;
			}
		}
	}

	if (max_displacement_idx > 0)
	{
		result->max_displacement_freq = app->frequencies[max_displacement_idx + app->rfft_read_offset];
	}
	else
	{
		result->max_displacement = FLT_MAX;
	}
}


static void roll_and_update_time_series(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	const uint16_t sweeps_per_frame = acc_config_sweeps_per_frame_get(config->sensor_config);

	for (uint16_t i = 0; i < sweeps_per_frame; i++)
	{
		acc_int16_complex_t point       = app->proc_result.frame[i];
		float               new_element = cargf(point.real + (point.imag * I));

		acc_algorithm_roll_and_push(app->time_series, config->time_series_length, new_element);
	}

	acc_algorithm_unwrap(app->time_series, config->time_series_length);
}


static void calculate_threshold(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	for (uint16_t i = 0; i < app->data_length; i++)
	{
		app->threshold[i] = acc_algorithm_calculate_cfar(app->lp_displacements,
		                                                 app->data_length,
		                                                 CFAR_WINDOW_LENGTH,
		                                                 CFAR_HALF_GUARD_LENGTH,
		                                                 config->threshold_sensitivity,
		                                                 i);
	}

	/*
	 * Extend the CFAR threshold using extrapolation.
	 *
	 * The head is extended using linear extrapolation based on the first
	 * points of the original threshold.
	 *
	 * The tail is extended using the average of the last points of the
	 * original threshold.
	 */

	const float    head_slope_multiplier        = 2.0f;
	const uint16_t head_slope_calculation_width = 2U;
	const uint16_t tail_mean_calculation_width  = 10U;
	const uint16_t tail_mean_calculation_start  = app->data_length - CFAR_MARGIN - tail_mean_calculation_width;

	const float    head_base_offset = app->threshold[CFAR_MARGIN];
	const uint16_t tail_offset      = app->data_length - CFAR_MARGIN;

	/*
	 * Calculate head.
	 */

	float head_mean_slope = 0.0f;

	for (uint16_t i = 0; i < head_slope_calculation_width; i++)
	{
		uint16_t pos = CFAR_MARGIN + i;
		head_mean_slope += app->threshold[pos + 1U] - app->threshold[pos];
	}

	head_mean_slope /= (float)head_slope_calculation_width;
	head_mean_slope *= head_slope_multiplier;

	/*
	 * Calculate tail.
	 */

	float tail_mean = 0.0f;

	for (int16_t i = 0; i < tail_mean_calculation_width; i++)
	{
		tail_mean += app->threshold[tail_mean_calculation_start + i];
	}

	tail_mean /= (float)tail_mean_calculation_width;

	/*
	 * Extend the threshold at both ends.
	 */

	for (uint16_t i = 0; i < CFAR_MARGIN; i++)
	{
		app->threshold[i]               = ((float)i - CFAR_MARGIN) * head_mean_slope + head_base_offset;
		app->threshold[tail_offset + i] = tail_mean;
	}
}


static float *get_zero_mean_time_series(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	float mean = 0.0f;

	for (uint16_t i = 0; i < config->time_series_length; i++)
	{
		mean += app->time_series[i];
	}

	mean /= (float)config->time_series_length;

	for (uint16_t i = 0; i < config->time_series_length; i++)
	{
		app->zero_mean_time_series[i] = app->time_series[i] - mean;
	}

	return app->zero_mean_time_series;
}


static float calculate_time_series_std(float *zm_time_series, uint16_t zm_time_series_length, float radians_to_displacement)
{
	double time_series_mean = 0.0;

	for (uint16_t i = 0; i < zm_time_series_length; i++)
	{
		float zm_time_series_um = zm_time_series[i] * radians_to_displacement;
		time_series_mean += (double)(zm_time_series_um * zm_time_series_um);
	}

	time_series_mean /= (double)zm_time_series_length;

	return (float)sqrt(time_series_mean);
}


static void setup_rfft_bounds(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	const uint16_t N = config->time_series_length;

	uint16_t shift  = 0;
	uint16_t length = 1U;

	if (N > 1U)
	{
		while (length < N)
		{
			length <<= 1;
			shift++;
		}
	}

	app->padded_time_series_length = length;
	app->rfft_length_shift         = shift;
}


static void print_result(acc_vibration_app_t *app, acc_vibration_result_t *result)
{
	char   buf[80]  = "";
	size_t buf_size = sizeof(buf);

	if (result->max_displacement != FLT_MAX)
	{
		snprintf(buf, buf_size, ", max_displacement=%" PRIfloat " um, max_displacement_freq=%" PRIfloat " Hz",
		         ACC_LOG_FLOAT_TO_INTEGER(result->max_displacement),
		         ACC_LOG_FLOAT_TO_INTEGER(result->max_displacement_freq));
	}

	printf("Vibration: max_sweep_amplitude=%" PRIfloat " um%s\n", ACC_LOG_FLOAT_TO_INTEGER(result->max_sweep_amplitude), buf);

	if (!app->continuous_data_acquisition)
	{
		printf("\nDisplacement history:\n");

		for (uint16_t i = 0; i < app->data_length; i++)
		{
			printf("%" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(app->lp_displacements[i]));
			if ((i % DISPLACEMENT_HISTORY_COLUMN_WIDTH) == 0)
			{
				printf("\n");
			}
		}

		printf("\n\n");
	}
}


static void cleanup(acc_vibration_app_t *app, acc_vibration_config_t *config)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (app->sensor != NULL)
	{
		acc_sensor_destroy(app->sensor);
		app->sensor = NULL;
	}

	if (app->processing != NULL)
	{
		acc_processing_destroy(app->processing);
		app->processing = NULL;
	}

	if (config->sensor_config != NULL)
	{
		acc_config_destroy(config->sensor_config);
		config->sensor_config = NULL;
	}

	if (app->rss_buffer != NULL)
	{
		acc_integration_mem_free(app->rss_buffer);
		app->rss_buffer = NULL;
	}

	if (app->time_series != NULL)
	{
		acc_integration_mem_free(app->time_series);
		app->time_series = NULL;
	}

	if (app->frequencies != NULL)
	{
		acc_integration_mem_free(app->frequencies);
		app->frequencies = NULL;
	}

	if (app->lp_displacements != NULL)
	{
		acc_integration_mem_free(app->lp_displacements);
		app->lp_displacements = NULL;
	}

	if (app->double_buffer_filter_buffer != NULL)
	{
		acc_integration_mem_free(app->double_buffer_filter_buffer);
		app->double_buffer_filter_buffer = NULL;
	}

	if (app->zero_mean_time_series != NULL)
	{
		acc_integration_mem_free(app->zero_mean_time_series);
		app->zero_mean_time_series = NULL;
	}

	if (app->rfft_output != NULL)
	{
		acc_integration_mem_free(app->rfft_output);
		app->rfft_output = NULL;
	}

	if (app->threshold != NULL)
	{
		acc_integration_mem_free(app->threshold);
		app->threshold = NULL;
	}
}
