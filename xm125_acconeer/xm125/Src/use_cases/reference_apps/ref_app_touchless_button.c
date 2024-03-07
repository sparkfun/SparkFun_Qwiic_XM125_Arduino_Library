// Copyright (c) Acconeer AB, 2023-2024
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "acc_algorithm.h"
#include "acc_config.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_integration_log.h"
#include "acc_processing.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"
#include "acc_version.h"


#define SENSOR_ID (1U)
// The time to measure for the application presets should be well below 1s.
// If timeouts occurs, try increasing the number.
#define SENSOR_TIMEOUT_MS (1000U)

/**
 * @brief Application measurement type
 */
typedef enum
{
/**
 * Corresponds to a range of approximate 0 cm - 5 cm
 */
	ACC_TOUCHLESS_BUTTON_CLOSE_RANGE,
/**
 * Corresponds to a range of approximate 0 cm - 24 cm
 */
	ACC_TOUCHLESS_BUTTON_FAR_RANGE,
/**
 * Gives two detection outputs, one for each range
 */
	ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE,
} acc_touchless_button_measurement_type_t;

/**
 * @brief Touchless button application config
 */
typedef struct
{
	/**
	 * Sensitivity for close range detection. High sensitivity equals low
	 * detection threshold, low sensitivity equals high detection threshold
	 */
	float sensitivity_close;
	/**
	 * Number of frames in a row above threshold to count as a new close range detection,
	 * also number of frames in a row below threshold to count as end of detection
	 */
	uint16_t patience_close;
	/**
	 * Sensitivity for far range detection. High sensitivity equals low
	 * detection threshold, low sensitivity equals high detection threshold
	 */
	float sensitivity_far;
	/**
	 * Number of frames in a row above threshold to count as a new close range detection,
	 * also number of frames in a row below threshold to count as end of detection
	 */
	uint16_t patience_far;
	/**
	 * Calibration duration in seconds
	 */
	float calibration_duration_s;
	/**
	 * Interval between calibrations in seconds. When reached a new calibration is made.
	 * Should not be set lower than the longest estimated continuous detection event
	 */
	float calibration_interval_s;
	/**
	 * The measurement type
	 * See @ref acc_touchless_button_measurement_type_t for more information
	 */
	acc_touchless_button_measurement_type_t measurement_type;
	/**
	 * The sensor configuration
	 */
	acc_config_t *sensor_config;
} acc_touchless_button_config_t;

typedef enum
{
	ACC_TOUCHLESS_BUTTON_PRESET_NONE,
	ACC_TOUCHLESS_BUTTON_PRESET_CLOSE,
	ACC_TOUCHLESS_BUTTON_PRESET_FAR,
	ACC_TOUCHLESS_BUTTON_PRESET_CLOSE_AND_FAR,
} acc_touchless_button_preset_t;

#define DEFAULT_PRESET ACC_TOUCHLESS_BUTTON_PRESET_CLOSE

/**
 * @brief Touchless button application handle
 */
typedef struct
{
	acc_touchless_button_config_t config;
	acc_sensor_t                  *sensor;
	acc_processing_t              *processing;
	acc_processing_metadata_t     proc_metadata;
	acc_processing_result_t       proc_result;
	void                          *buffer;
	uint32_t                      buffer_size;
	int32_t                       *double_buffer_filter_buffer;
	float                         *frame_variance;
	float complex                 *arg_norm;
	float                         *ampl_mean;
	float                         *ampl_std;
	float                         *phase_mean;
	float                         *phase_std;
	uint16_t                      cal_interval_frames;
	uint16_t                      cal_sweeps;
	acc_int16_complex_t           *dynamic_background;
	uint16_t                      rows_in_dynamic_background;
	acc_int16_complex_t           *dynamic_background_guard;
	bool                          update_background;
	uint16_t                      frames_since_last_cal;
	float                         close_threshold;
	float                         far_threshold;
	uint16_t                      *threshold_check_count;
	bool                          run_close;
	bool                          run_far;
	uint16_t                      close_num_points;
	uint16_t                      far_num_points;
	uint16_t                      close_signal;
	uint16_t                      far_signal;
	uint16_t                      close_non_signal;
	uint16_t                      far_non_signal;
	bool                          close_detection;
	bool                          far_detection;
} acc_touchless_button_handle_t;


/**
 * @brief Touchless button range result
 */
typedef enum
{
	ACC_TOUCHLESS_BUTTON_RANGE_NOT_USED,
	ACC_TOUCHLESS_BUTTON_RANGE_NO_DETECTION,
	ACC_TOUCHLESS_BUTTON_RANGE_DETECTION,
} acc_touchless_button_range_result_t;

/**
 * @brief Touchless button application result
 */
typedef struct
{
	acc_touchless_button_range_result_t close_result;
	acc_touchless_button_range_result_t far_result;
} acc_touchless_button_result_t;


static void cleanup(acc_touchless_button_handle_t *handle);


static void set_config(acc_touchless_button_config_t *config, acc_touchless_button_preset_t preset);


static bool validate_config(acc_touchless_button_config_t *config);


static void config_log(acc_touchless_button_config_t *config);


static bool init_handle(acc_touchless_button_handle_t *handle);


static float get_threshold(float sensitivity);


static void reset_background(acc_touchless_button_handle_t *handle);


static bool init_sensor(acc_touchless_button_handle_t *handle);


static bool do_sensor_calibration_and_prepare(acc_touchless_button_handle_t *handle);


static bool measure(acc_touchless_button_handle_t *handle);


static bool check_count(uint16_t *count, uint16_t check_offset, uint16_t check_length);


static void update_background(acc_touchless_button_handle_t *handle);


static bool get_detection(bool current_detection, uint16_t sig_count, uint16_t non_sig_count, uint16_t patience);


static bool process(acc_touchless_button_handle_t *handle, acc_touchless_button_result_t *result);


static void print_result(acc_touchless_button_result_t *result);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	acc_touchless_button_handle_t handle;
	handle.sensor                      = NULL;
	handle.processing                  = NULL;
	handle.buffer                      = NULL;
	handle.double_buffer_filter_buffer = NULL;
	handle.frame_variance              = NULL;
	handle.arg_norm                    = NULL;
	handle.ampl_mean                   = NULL;
	handle.ampl_std                    = NULL;
	handle.phase_mean                  = NULL;
	handle.phase_std                   = NULL;
	handle.dynamic_background          = NULL;
	handle.dynamic_background_guard    = NULL;
	handle.threshold_check_count       = NULL;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	handle.config.sensor_config = acc_config_create();
	if (handle.config.sensor_config == NULL)
	{
		printf("acc_config_create() failed\n");
		cleanup(&handle);
		return EXIT_FAILURE;
	}

	set_config(&handle.config, DEFAULT_PRESET);

	if (!validate_config(&handle.config))
	{
		cleanup(&handle);
		return EXIT_FAILURE;
	}

	config_log(&handle.config);

	if (!init_handle(&handle))
	{
		printf("Failed to init handle\n");
		cleanup(&handle);
		return EXIT_FAILURE;
	}

	if (!init_sensor(&handle))
	{
		printf("Failed to init sensor\n");
		cleanup(&handle);
		return EXIT_FAILURE;
	}

	while (true)
	{
		if (!measure(&handle))
		{
			printf("Failed to measure\n");
			cleanup(&handle);
			return EXIT_FAILURE;
		}

		if (handle.proc_result.data_saturated)
		{
			printf("Data saturated. The detector result is not reliable.\n");
		}

		if (handle.proc_result.frame_delayed)
		{
			printf("Frame delayed. Could not read data fast enough.\n");
			printf("Try lowering the frame rate or call 'acc_sensor_read' more frequently.\n");
		}

		if (handle.proc_result.calibration_needed)
		{
			printf("The current calibration is not valid for the current temperature.\n");
			printf("The sensor needs to be re-calibrated.\n");

			if (!do_sensor_calibration_and_prepare(&handle))
			{
				printf("do_sensor_calibration_and_prepare() failed\n");
				acc_sensor_status(handle.sensor);
				cleanup(&handle);
				return EXIT_FAILURE;
			}

			reset_background(&handle);

			printf("The sensor was successfully re-calibrated.\n");
		}
		else
		{
			acc_touchless_button_result_t result = { 0 };

			if (!process(&handle, &result))
			{
				printf("Failed to process sensor result\n");
				cleanup(&handle);
				return EXIT_FAILURE;
			}

			print_result(&result);
		}
	}

	cleanup(&handle);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void cleanup(acc_touchless_button_handle_t *handle)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (handle->config.sensor_config != NULL)
	{
		acc_config_destroy(handle->config.sensor_config);
	}

	if (handle->sensor != NULL)
	{
		acc_sensor_destroy(handle->sensor);
	}

	if (handle->processing != NULL)
	{
		acc_processing_destroy(handle->processing);
	}

	if (handle->buffer != NULL)
	{
		acc_integration_mem_free(handle->buffer);
	}

	if (handle->double_buffer_filter_buffer != NULL)
	{
		acc_integration_mem_free(handle->double_buffer_filter_buffer);
	}

	if (handle->frame_variance != NULL)
	{
		acc_integration_mem_free(handle->frame_variance);
	}

	if (handle->arg_norm != NULL)
	{
		acc_integration_mem_free(handle->arg_norm);
	}

	if (handle->ampl_mean != NULL)
	{
		acc_integration_mem_free(handle->ampl_mean);
	}

	if (handle->ampl_std != NULL)
	{
		acc_integration_mem_free(handle->ampl_std);
	}

	if (handle->phase_mean != NULL)
	{
		acc_integration_mem_free(handle->phase_mean);
	}

	if (handle->phase_std != NULL)
	{
		acc_integration_mem_free(handle->phase_std);
	}

	if (handle->dynamic_background != NULL)
	{
		acc_integration_mem_free(handle->dynamic_background);
	}

	if (handle->dynamic_background_guard != NULL)
	{
		acc_integration_mem_free(handle->dynamic_background_guard);
	}

	if (handle->threshold_check_count != NULL)
	{
		acc_integration_mem_free(handle->threshold_check_count);
	}
}


static void set_config(acc_touchless_button_config_t *config, acc_touchless_button_preset_t preset)
{
	switch (preset)
	{
		case ACC_TOUCHLESS_BUTTON_PRESET_NONE:
			break;
		case ACC_TOUCHLESS_BUTTON_PRESET_CLOSE:
			acc_config_sweeps_per_frame_set(config->sensor_config, 16U);
			acc_config_sweep_rate_set(config->sensor_config, 320U);
			acc_config_continuous_sweep_mode_set(config->sensor_config, true);
			acc_config_double_buffering_set(config->sensor_config, true);
			acc_config_inter_sweep_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
			acc_config_inter_frame_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
			acc_config_num_points_set(config->sensor_config, 3U);
			acc_config_profile_set(config->sensor_config, ACC_CONFIG_PROFILE_1);
			acc_config_receiver_gain_set(config->sensor_config, 0U);
			acc_config_hwaas_set(config->sensor_config, 40U);
			acc_config_start_point_set(config->sensor_config, 0U);
			acc_config_step_length_set(config->sensor_config, 6U);

			config->sensitivity_close      = 1.9f;
			config->patience_close         = 2U;
			config->calibration_duration_s = 0.6f;
			config->calibration_interval_s = 20.0f;
			config->measurement_type       = ACC_TOUCHLESS_BUTTON_CLOSE_RANGE;
			break;
		case ACC_TOUCHLESS_BUTTON_PRESET_FAR:
			acc_config_sweeps_per_frame_set(config->sensor_config, 16U);
			acc_config_sweep_rate_set(config->sensor_config, 320U);
			acc_config_continuous_sweep_mode_set(config->sensor_config, true);
			acc_config_double_buffering_set(config->sensor_config, true);
			acc_config_inter_sweep_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
			acc_config_inter_frame_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
			acc_config_num_points_set(config->sensor_config, 3U);
			acc_config_profile_set(config->sensor_config, ACC_CONFIG_PROFILE_3);
			acc_config_receiver_gain_set(config->sensor_config, 5U);
			acc_config_hwaas_set(config->sensor_config, 60U);
			acc_config_start_point_set(config->sensor_config, 0U);
			acc_config_step_length_set(config->sensor_config, 24U);

			config->sensitivity_far        = 2.0f;
			config->patience_far           = 2U;
			config->calibration_duration_s = 0.6f;
			config->calibration_interval_s = 20.0f;
			config->measurement_type       = ACC_TOUCHLESS_BUTTON_FAR_RANGE;
			break;
		case ACC_TOUCHLESS_BUTTON_PRESET_CLOSE_AND_FAR:
			acc_config_num_subsweeps_set(config->sensor_config, 2U);
			acc_config_sweeps_per_frame_set(config->sensor_config, 16U);
			acc_config_sweep_rate_set(config->sensor_config, 320U);
			acc_config_continuous_sweep_mode_set(config->sensor_config, true);
			acc_config_double_buffering_set(config->sensor_config, true);
			acc_config_inter_sweep_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
			acc_config_inter_frame_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);

			acc_config_subsweep_num_points_set(config->sensor_config, 3U, 0U);
			acc_config_subsweep_profile_set(config->sensor_config, ACC_CONFIG_PROFILE_1, 0U);
			acc_config_subsweep_receiver_gain_set(config->sensor_config, 0U, 0U);
			acc_config_subsweep_hwaas_set(config->sensor_config, 40U, 0U);
			acc_config_subsweep_start_point_set(config->sensor_config, 0U, 0U);
			acc_config_subsweep_step_length_set(config->sensor_config, 6U, 0U);

			acc_config_subsweep_num_points_set(config->sensor_config, 3U, 1U);
			acc_config_subsweep_profile_set(config->sensor_config, ACC_CONFIG_PROFILE_3, 1U);
			acc_config_subsweep_receiver_gain_set(config->sensor_config, 5U, 1U);
			acc_config_subsweep_hwaas_set(config->sensor_config, 60U, 1U);
			acc_config_subsweep_start_point_set(config->sensor_config, 0U, 1U);
			acc_config_subsweep_step_length_set(config->sensor_config, 24U, 1U);

			config->sensitivity_close      = 1.9f;
			config->patience_close         = 2U;
			config->sensitivity_far        = 2.0f;
			config->patience_far           = 2U;
			config->calibration_duration_s = 0.6f;
			config->calibration_interval_s = 20.0f;
			config->measurement_type       = ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE;
			break;
	}
}


static bool validate_config(acc_touchless_button_config_t *config)
{
	bool status = true;

	if (acc_config_num_subsweeps_get(config->sensor_config) != 2U &&
	    config->measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE)
	{
		printf("Number of subsweeps must be 2 for 'close and far'\n");
		status = false;
	}

	if (acc_config_num_subsweeps_get(config->sensor_config) != 1U &&
	    (config->measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_RANGE || config->measurement_type == ACC_TOUCHLESS_BUTTON_FAR_RANGE))
	{
		printf("Number of subsweeps must be 1 for 'close' and 'far'\n");
		status = false;
	}

	float    sweep_rate = acc_config_sweep_rate_get(config->sensor_config);
	uint16_t spf        = acc_config_sweeps_per_frame_get(config->sensor_config);

	if (sweep_rate == 0.0f)
	{
		printf("Sweep rate must be set\n");
		status = false;
	}
	else
	{
		if (spf > (config->calibration_duration_s * sweep_rate))
		{
			float calibration_limit = (float)spf / sweep_rate;

			printf("Calibration duration must be at least %" PRIfloat " s\n", ACC_LOG_FLOAT_TO_INTEGER(calibration_limit));
			status = false;
		}
	}

	return status;
}


static void config_log(acc_touchless_button_config_t *config)
{
	float                                   sensitivity_close      = config->sensitivity_close;
	uint16_t                                patience_close         = config->patience_close;
	float                                   sensitivity_far        = config->sensitivity_far;
	uint16_t                                patience_far           = config->patience_far;
	float                                   calibration_duration_s = config->calibration_duration_s;
	float                                   calibration_interval_s = config->calibration_interval_s;
	acc_touchless_button_measurement_type_t measurement_type       = config->measurement_type;

	printf("Reference application Touchless Button config\n");

	if (measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_RANGE || measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE)
	{
		printf("sensitivity close: %" PRIfloat "\n", ACC_LOG_FLOAT_TO_INTEGER(sensitivity_close));
		printf("patience close: %" PRIu16 "\n", patience_close);
	}

	if (measurement_type == ACC_TOUCHLESS_BUTTON_FAR_RANGE || measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE)
	{
		printf("sensitivity far: %" PRIfloat "\n", ACC_LOG_FLOAT_TO_INTEGER(sensitivity_far));
		printf("patience far: %" PRIu16 "\n", patience_far);
	}

	printf("calibration duration: %" PRIfloat "\n", ACC_LOG_FLOAT_TO_INTEGER(calibration_duration_s));
	printf("calibration interval: %" PRIfloat "\n", ACC_LOG_FLOAT_TO_INTEGER(calibration_interval_s));

	char *measurement_type_str = "";

	switch (measurement_type)
	{
		case ACC_TOUCHLESS_BUTTON_CLOSE_RANGE:
			measurement_type_str = "close";
			break;
		case ACC_TOUCHLESS_BUTTON_FAR_RANGE:
			measurement_type_str = "far";
			break;
		case ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE:
			measurement_type_str = "close and far";
			break;
	}

	printf("measurement type: %s\n", measurement_type_str);

	printf("Sensor config\n");
	acc_config_log(config->sensor_config);
}


static bool init_handle(acc_touchless_button_handle_t *handle)
{
	bool status = true;

	if (!acc_rss_get_buffer_size(handle->config.sensor_config, &handle->buffer_size))
	{
		printf("acc_rss_get_buffer_size() failed\n");
		status = false;
	}

	if (status)
	{
		handle->processing = acc_processing_create(handle->config.sensor_config, &handle->proc_metadata);
		handle->buffer     = acc_integration_mem_alloc(handle->buffer_size);

		uint16_t spf        = acc_config_sweeps_per_frame_get(handle->config.sensor_config);
		float    sweep_rate = acc_config_sweep_rate_get(handle->config.sensor_config);
		float    frame_rate = sweep_rate / (float)spf;

		handle->cal_interval_frames = (uint16_t)((frame_rate * handle->config.calibration_interval_s) + 0.5f);
		handle->cal_sweeps          = (uint16_t)((sweep_rate * handle->config.calibration_duration_s) + 0.5f);
		uint16_t num_points = handle->proc_metadata.sweep_data_length;

		handle->double_buffer_filter_buffer =
			acc_integration_mem_alloc((spf - 2U) * sizeof(*handle->double_buffer_filter_buffer));
		handle->frame_variance =
			acc_integration_mem_alloc(handle->proc_metadata.frame_data_length * sizeof(*handle->frame_variance));
		handle->arg_norm                 = acc_integration_mem_alloc(handle->proc_metadata.sweep_data_length * sizeof(*handle->arg_norm));
		handle->ampl_mean                = acc_integration_mem_alloc(handle->proc_metadata.sweep_data_length * sizeof(*handle->ampl_mean));
		handle->ampl_std                 = acc_integration_mem_alloc(handle->proc_metadata.sweep_data_length * sizeof(*handle->ampl_std));
		handle->phase_mean               = acc_integration_mem_alloc(handle->proc_metadata.sweep_data_length * sizeof(*handle->phase_mean));
		handle->phase_std                = acc_integration_mem_alloc(handle->proc_metadata.sweep_data_length * sizeof(*handle->phase_std));
		handle->dynamic_background       = acc_integration_mem_alloc(handle->cal_sweeps * num_points * sizeof(*handle->dynamic_background));
		handle->dynamic_background_guard =
			acc_integration_mem_alloc(handle->proc_metadata.frame_data_length * sizeof(*handle->dynamic_background_guard));
		handle->threshold_check_count =
			acc_integration_mem_alloc(handle->proc_metadata.sweep_data_length * sizeof(*handle->threshold_check_count));

		status =
			(handle->processing != NULL && handle->buffer != NULL && handle->double_buffer_filter_buffer != NULL &&
			 handle->frame_variance != NULL && handle->arg_norm != NULL && handle->ampl_mean != NULL &&
			 handle->ampl_std != NULL && handle->phase_mean != NULL && handle->phase_std != NULL &&
			 handle->dynamic_background != NULL && handle->dynamic_background_guard != NULL &&
			 handle->threshold_check_count != NULL);
	}

	if (status)
	{
		reset_background(handle);

		handle->run_close = handle->config.measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_RANGE ||
		                    handle->config.measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE;
		handle->run_far = handle->config.measurement_type == ACC_TOUCHLESS_BUTTON_FAR_RANGE ||
		                  handle->config.measurement_type == ACC_TOUCHLESS_BUTTON_CLOSE_AND_FAR_RANGE;
		handle->close_num_points = handle->run_close ? acc_config_subsweep_num_points_get(handle->config.sensor_config, 0U) : 0U;
		handle->far_num_points   =
			handle->run_far ? (handle->run_close ? acc_config_subsweep_num_points_get(handle->config.sensor_config,
			                                                                          1U) : acc_config_num_points_get(handle->config.
			                                                                                                          sensor_config)) : 0U;

		handle->close_threshold = get_threshold(handle->config.sensitivity_close);
		handle->far_threshold   = get_threshold(handle->config.sensitivity_far);

		handle->close_signal     = 0U;
		handle->far_signal       = 0U;
		handle->close_non_signal = handle->config.patience_close + 1U;
		handle->far_non_signal   = handle->config.patience_far + 1U;
		handle->close_detection  = false;
		handle->far_detection    = false;
	}

	return status;
}


static float get_threshold(float sensitivity)
{
	return (1.0f / sensitivity) * 10.0f;
}


static void reset_background(acc_touchless_button_handle_t *handle)
{
	handle->frames_since_last_cal = 0U;

	uint16_t spf        = acc_config_sweeps_per_frame_get(handle->config.sensor_config);
	uint16_t num_points = handle->proc_metadata.sweep_data_length;

	memset(handle->dynamic_background, 0, handle->cal_sweeps * num_points * sizeof(*handle->dynamic_background));
	handle->rows_in_dynamic_background = 0U;
	memset(handle->dynamic_background_guard, 0, spf * num_points * sizeof(*handle->dynamic_background_guard));
	handle->update_background = false;
}


static bool init_sensor(acc_touchless_button_handle_t *handle)
{
	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	handle->sensor = acc_sensor_create(SENSOR_ID);

	if (handle->sensor == NULL)
	{
		return false;
	}

	if (!do_sensor_calibration_and_prepare(handle))
	{
		return false;
	}

	return true;
}


static bool do_sensor_calibration_and_prepare(acc_touchless_button_handle_t *handle)
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
			status = acc_sensor_calibrate(handle->sensor, &cal_complete, &cal_result, handle->buffer, handle->buffer_size);

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

		status = acc_sensor_prepare(handle->sensor, handle->config.sensor_config, &cal_result, handle->buffer, handle->buffer_size);
	}

	return status;
}


static bool measure(acc_touchless_button_handle_t *handle)
{
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
		printf("acc_sensor_read failed\n");
		acc_sensor_status(handle->sensor);
		return false;
	}

	acc_processing_execute(handle->processing, handle->buffer, &handle->proc_result);

	return true;
}


static void calc_variance(acc_touchless_button_handle_t *handle)
{
	uint16_t sweep_data_length = handle->proc_metadata.sweep_data_length;

	acc_algorithm_mean_matrix_i16_complex(handle->dynamic_background, handle->cal_sweeps, sweep_data_length, handle->arg_norm, 0U);

	acc_algorithm_conj_f32(handle->arg_norm, sweep_data_length);

	acc_algorithm_normalize_f32_complex(handle->arg_norm, sweep_data_length);

	for (uint16_t c = 0U; c < sweep_data_length; c++)
	{
		float abs_mean      = 0.0f;
		float abs_sq_term   = 0.0f;
		float phase_mean    = 0.0f;
		float phase_sq_term = 0.0f;
		for (uint16_t r = 0U; r < handle->cal_sweeps; r++)
		{
			float         ac      = handle->dynamic_background[r * sweep_data_length + c].real * crealf(handle->arg_norm[c]);
			float         bd      = handle->dynamic_background[r * sweep_data_length + c].imag * cimagf(handle->arg_norm[c]);
			float         ad      = handle->dynamic_background[r * sweep_data_length + c].real * cimagf(handle->arg_norm[c]);
			float         bc      = handle->dynamic_background[r * sweep_data_length + c].imag * crealf(handle->arg_norm[c]);
			float complex element = (ac - bd) + I * (ad + bc);

			float    delta = cabsf(element) - abs_mean;
			uint16_t div   = (r == 0U) ? 1U : r;
			abs_mean    += (delta / (float)div);
			abs_sq_term += delta * (cabsf(element) - abs_mean);

			delta          = cargf(element) - phase_mean;
			phase_mean    += (delta / (float)div);
			phase_sq_term += delta * (cargf(element) - phase_mean);
		}

		handle->ampl_mean[c]  = abs_mean;
		handle->ampl_std[c]   = sqrtf(abs_sq_term / handle->cal_sweeps);
		handle->phase_mean[c] = phase_mean;
		handle->phase_std[c]  = sqrtf(phase_sq_term / handle->cal_sweeps);
	}

	uint16_t spf = acc_config_sweeps_per_frame_get(handle->config.sensor_config);

	for (uint16_t r = 0U; r < spf; r++)
	{
		for (uint16_t c = 0U; c < sweep_data_length; c++)
		{
			float         ac      = handle->proc_result.frame[r * sweep_data_length + c].real * crealf(handle->arg_norm[c]);
			float         bd      = handle->proc_result.frame[r * sweep_data_length + c].imag * cimagf(handle->arg_norm[c]);
			float         ad      = handle->proc_result.frame[r * sweep_data_length + c].real * cimagf(handle->arg_norm[c]);
			float         bc      = handle->proc_result.frame[r * sweep_data_length + c].imag * crealf(handle->arg_norm[c]);
			float complex element = (ac - bd) + I * (ad + bc);
			float         a       = (cabsf(element) - handle->ampl_mean[c]) / handle->ampl_std[c];
			float         b       = (cargf(element) - handle->phase_mean[c]) / handle->phase_std[c];

			handle->frame_variance[r * sweep_data_length + c] = sqrtf(powf(a, 2.0f) + powf(b, 2.0f));
		}
	}
}


static bool check_count(uint16_t *count, uint16_t check_offset, uint16_t check_length)
{
	bool success = false;

	for (uint16_t i = 0U; i < check_length; i++)
	{
		if (count[check_offset + i] > 1U)
		{
			success = true;
		}
	}

	return success;
}


static void update_background(acc_touchless_button_handle_t *handle)
{
	uint16_t spf = acc_config_sweeps_per_frame_get(handle->config.sensor_config);

	acc_algorithm_roll_and_push_mult_matrix_i16_complex(handle->dynamic_background, handle->cal_sweeps, handle->proc_metadata.sweep_data_length,
	                                                    handle->dynamic_background_guard, acc_config_sweeps_per_frame_get(
								    handle->config.sensor_config), false);

	handle->rows_in_dynamic_background += spf;
	handle->rows_in_dynamic_background  = handle->rows_in_dynamic_background >
	                                      handle->cal_sweeps ? handle->cal_sweeps : handle->rows_in_dynamic_background;
	handle->frames_since_last_cal = 0U;
}


static bool get_detection(bool current_detection, uint16_t sig_count, uint16_t non_sig_count, uint16_t patience)
{
	bool new_detection  = !current_detection && sig_count >= patience;
	bool keep_detection = current_detection && non_sig_count <= patience;

	return new_detection || keep_detection;
}


static bool process(acc_touchless_button_handle_t *handle, acc_touchless_button_result_t *result)
{
	if (acc_config_double_buffering_get(handle->config.sensor_config))
	{
		acc_algorithm_double_buffering_frame_filter(handle->proc_result.frame, acc_config_sweeps_per_frame_get(
								    handle->config.sensor_config),
		                                            acc_config_num_points_get(handle->config.sensor_config),
		                                            handle->double_buffer_filter_buffer);
	}

	memset(handle->frame_variance, 0, handle->proc_metadata.frame_data_length * sizeof(*handle->frame_variance));

	if (handle->frames_since_last_cal > handle->cal_interval_frames)
	{
		reset_background(handle);
	}
	else if (handle->rows_in_dynamic_background == handle->cal_sweeps)
	{
		calc_variance(handle);
	}

	uint16_t threshold_check_offset = 0U;

	if (handle->run_close)
	{
		acc_algorithm_count_points_above_threshold(handle->frame_variance, acc_config_sweeps_per_frame_get(
								   handle->config.sensor_config), handle->proc_metadata.sweep_data_length, handle->close_threshold, handle->threshold_check_count, threshold_check_offset, handle->close_num_points,
		                                           1U);

		threshold_check_offset = handle->close_num_points;
	}

	if (handle->run_far)
	{
		acc_algorithm_count_points_above_threshold(handle->frame_variance, acc_config_sweeps_per_frame_get(
								   handle->config.sensor_config), handle->proc_metadata.sweep_data_length, handle->far_threshold, handle->threshold_check_count, threshold_check_offset, handle->far_num_points,
		                                           1U);
	}

	if (check_count(handle->threshold_check_count, 0U, handle->proc_metadata.sweep_data_length))
	{
		uint16_t close_offset = 0U;
		uint16_t far_offset   = handle->close_num_points;

		if (handle->run_close)
		{
			if (check_count(handle->threshold_check_count, close_offset, handle->close_num_points))
			{
				handle->close_signal++;
				handle->close_non_signal = 0U;
			}
			else
			{
				handle->close_signal = 0U;
				handle->close_non_signal++;
			}
		}

		if (handle->run_far)
		{
			if (check_count(handle->threshold_check_count, far_offset, handle->far_num_points))
			{
				handle->far_signal++;
				handle->far_non_signal = 0U;
			}
			else
			{
				handle->far_signal = 0U;
				handle->far_non_signal++;
			}
		}

		handle->update_background = false;
		memset(handle->dynamic_background_guard, 0, handle->proc_metadata.frame_data_length * sizeof(*handle->dynamic_background_guard));
		handle->frames_since_last_cal++;
	}
	else
	{
		handle->close_signal = 0U;
		handle->far_signal   = 0U;
		handle->close_non_signal++;
		handle->far_non_signal++;

		if (handle->update_background)
		{
			update_background(handle);
		}

		memcpy(handle->dynamic_background_guard, handle->proc_result.frame,
		       handle->proc_metadata.frame_data_length * sizeof(*handle->dynamic_background_guard));
		handle->update_background = true;
	}

	memset(result, 0, sizeof(*result));

	result->close_result = ACC_TOUCHLESS_BUTTON_RANGE_NOT_USED;
	result->far_result   = ACC_TOUCHLESS_BUTTON_RANGE_NOT_USED;

	if (handle->run_close)
	{
		handle->close_detection = get_detection(handle->close_detection, handle->close_signal, handle->close_non_signal,
		                                        handle->config.patience_close);
		result->close_result = handle->close_detection ? ACC_TOUCHLESS_BUTTON_RANGE_DETECTION : ACC_TOUCHLESS_BUTTON_RANGE_NO_DETECTION;
	}

	if (handle->run_far)
	{
		handle->far_detection = get_detection(handle->far_detection, handle->far_signal, handle->far_non_signal, handle->config.patience_far);
		result->far_result    = handle->far_detection ? ACC_TOUCHLESS_BUTTON_RANGE_DETECTION : ACC_TOUCHLESS_BUTTON_RANGE_NO_DETECTION;
	}

	return true;
}


static void print_result(acc_touchless_button_result_t *result)
{
	if (result->close_result != ACC_TOUCHLESS_BUTTON_RANGE_NOT_USED)
	{
		printf("Close detection: %s\n", result->close_result == ACC_TOUCHLESS_BUTTON_RANGE_DETECTION ? "True" : "False");
	}

	if (result->far_result != ACC_TOUCHLESS_BUTTON_RANGE_NOT_USED)
	{
		printf("Far detection: %s\n", result->far_result == ACC_TOUCHLESS_BUTTON_RANGE_DETECTION ? "True" : "False");
	}
}
