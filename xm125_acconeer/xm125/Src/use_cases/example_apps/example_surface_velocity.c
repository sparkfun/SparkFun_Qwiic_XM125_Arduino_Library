// Copyright (c) Acconeer AB, 2023
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


/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * Distance to surface in meters.
 */
#define CONFIG_SURFACE_DISTANCE (1.0f)

/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * Angle of the sensor compared to the surface.
 */
#define CONFIG_SENSOR_ANGLE (45.0f)

/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * Threshold level sensitivity.
 */
#define CONFIG_THRESHOLD_SENSITIVITY (0.15f)

/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * Low-pass filtered power spectral density coefficient.
 *
 * Affects the stability of the estimate.
 */
#define CONFIG_PSD_LP_COEFF (0.75f)

/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * Low-pass filtered velocity coefficient.
 *
 * Affects the stability of the estimate.
 */
#define CONFIG_VELOCITY_LP_COEFF (0.98f)

/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * The sweep rate for sweeps in a frame during measurement.
 */
#define CONFIG_SWEEP_RATE (3000U)

/**
 * [Default app config - can be adapted to reflect the setup]
 *
 * Hardware accelerated average sampling.
 */
#define CONFIG_HWAAS (16U)


#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

#define MIN_PEAK_VS               0.1f
#define APPROX_BASE_STEP_LENGTH_M 2.5e-3f

/**
 * @brief Surface velocity application config container
 */
typedef struct
{
	float        surface_distance;
	uint16_t     time_series_length;
	uint16_t     slow_zone_half_length;
	float        psd_lp_coeff;
	uint16_t     cfar_guard;
	uint16_t     cfar_win;
	float        threshold_sensitivity;
	float        velocity_lp_coeff;
	float        max_peak_interval_s;
	float        sensor_angle;
	acc_config_t *sensor_config;
} acc_surface_velocity_config_t;


/**
 * @brief Surface velocity application context handle
 */
typedef struct
{
	acc_sensor_t              *sensor;
	acc_processing_t          *processing;
	acc_processing_metadata_t proc_meta;
	acc_processing_result_t   proc_result;
	void                      *buffer;
	uint32_t                  buffer_size;

	acc_surface_velocity_config_t surface_velocity_config;

	float    max_peak_interval_n;
	float    sweep_rate;
	uint16_t step_length;
	uint32_t start_point;
	float    base_step_length_m;
	uint16_t num_distances;
	uint16_t sweeps_per_frame;
	uint16_t segment_length;
	uint16_t padded_segment_length;
	uint16_t padded_segment_length_shift;
	uint16_t middle_index;

	int32_t       *double_buffer_filter_buffer;
	float complex *frame;
	float complex *time_series;
	float complex *time_series_buffer;
	float complex *fft_out;
	float         *psds;
	float         *lp_psds;
	float         *psd;
	float         *window;
	uint32_t      *threshold_check;
	float         *bin_rad_vs;
	float         *bin_vertical_vs;

	uint16_t update_index;
	uint16_t wait_n;
	float    lp_velocity;
	float    vertical_v;

	uint16_t *peak_indexes;
	uint16_t peak_indexes_length;
	uint16_t num_peaks;
	float    *merged_velocities;
	float    *merged_energies;
	uint16_t merged_peaks_length;
	uint16_t num_merged_peaks;
} acc_surface_velocity_handle_t;


/**
 * @brief Surface velocity application results container
 */
typedef struct
{
	float estimated_v;
	float distance_m;
} acc_surface_velocity_result_t;


static void cleanup(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config);


static void set_config(acc_surface_velocity_config_t *config);


static void set_prf(acc_surface_velocity_config_t *config);


static void set_profile(acc_surface_velocity_config_t *config);


static bool validate_config(acc_surface_velocity_config_t *config);


static bool do_sensor_calibration_and_prepare(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config);


static bool init_handle(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config);


static bool init_sensor_resources(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config);


static bool measure(acc_surface_velocity_handle_t *handle);


static bool process(acc_surface_velocity_handle_t *handle, acc_surface_velocity_result_t *result);


static void print_result(acc_surface_velocity_result_t *result);


static void update_threshold(acc_surface_velocity_handle_t *handle);


static bool get_velocity_estimate(acc_surface_velocity_handle_t *handle);


static float get_angle_correction(float surface_distance, float distance);


static uint16_t calc_power_spectral_density(acc_surface_velocity_handle_t *handle);


static float get_perceived_wavelength(void);


static float calc_dynamic_smoothing_factor(float static_sf, uint32_t update_count);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_surface_velocity_config_t config = { 0 };
	acc_surface_velocity_handle_t handle = { 0 };
	acc_surface_velocity_result_t result = { 0 };

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
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	set_config(&config);

	set_profile(&config);

	set_prf(&config);

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

	if (!init_sensor_resources(&handle, &config))
	{
		printf("Failed to initialize sensor resources\n");
		cleanup(&handle, &config);
		return EXIT_FAILURE;
	}

	while (true)
	{
		if (!measure(&handle))
		{
			cleanup(&handle, &config);
			return EXIT_FAILURE;
		}

		if (handle.proc_result.calibration_needed)
		{
			printf("The current calibration is not valid for the current temperature.\n");
			printf("The sensor needs to be re-calibrated.\n");

			if (!do_sensor_calibration_and_prepare(&handle, &config))
			{
				printf("do_sensor_calibration_and_prepare() failed\n");
				acc_sensor_status(handle.sensor);
				cleanup(&handle, &config);
				return EXIT_FAILURE;
			}

			printf("The sensor was successfully re-calibrated.\n");
		}
		else
		{
			if (!process(&handle, &result))
			{
				cleanup(&handle, &config);
				return EXIT_FAILURE;
			}

			print_result(&result);
		}
	}

	cleanup(&handle, &config);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void update_threshold(acc_surface_velocity_handle_t *handle)
{
	for (uint16_t i = 0U; i < handle->segment_length; i++)
	{
		float threshold = acc_algorithm_calculate_mirrored_one_sided_cfar(handle->psd, handle->segment_length, handle->middle_index,
		                                                                  handle->surface_velocity_config.cfar_win,
		                                                                  handle->surface_velocity_config.cfar_guard,
		                                                                  handle->surface_velocity_config.threshold_sensitivity, i);

		if (handle->psd[i] > threshold)
		{
			acc_alg_basic_utils_set_bit_bitarray_uint32(handle->threshold_check, i);
		}
		else
		{
			acc_alg_basic_utils_clear_bit_bitarray_uint32(handle->threshold_check, i);
		}
	}
}


static bool get_velocity_estimate(acc_surface_velocity_handle_t *handle)
{
	memset(handle->merged_velocities, 0,
	       handle->merged_peaks_length * sizeof(*handle->merged_velocities));
	memset(handle->merged_energies, 0,
	       handle->merged_peaks_length * sizeof(*handle->merged_energies));

	bool status = acc_algorithm_merge_peaks(MIN_PEAK_VS, handle->bin_vertical_vs, handle->psd, handle->peak_indexes, handle->num_peaks,
	                                        handle->merged_velocities,
	                                        handle->merged_energies, handle->merged_peaks_length, &(handle->num_merged_peaks));

	if (status)
	{
		handle->vertical_v = acc_algorithm_get_peak_velocity(handle->merged_velocities,
		                                                     handle->merged_energies, NULL, handle->num_merged_peaks,
		                                                     handle->bin_vertical_vs[handle->middle_index +
		                                                                             handle->surface_velocity_config.slow_zone_half_length]);
	}

	return status;
}


float get_angle_correction(float surface_distance, float distance)
{
	float insonation_angle = asinf(surface_distance / distance);
	float angle_correction = 1.0f / cosf(insonation_angle);

	return angle_correction;
}


static void cleanup(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (handle->processing != NULL)
	{
		acc_processing_destroy(handle->processing);
	}

	if (config->sensor_config != NULL)
	{
		acc_config_destroy(config->sensor_config);
	}

	if (handle->buffer != NULL)
	{
		acc_integration_mem_free(handle->buffer);
	}

	if (handle->sensor != NULL)
	{
		acc_sensor_destroy(handle->sensor);
	}

	if (handle->double_buffer_filter_buffer != NULL)
	{
		acc_integration_mem_free(handle->double_buffer_filter_buffer);
	}

	if (handle->frame != NULL)
	{
		acc_integration_mem_free(handle->frame);
	}

	if (handle->time_series != NULL)
	{
		acc_integration_mem_free(handle->time_series);
	}

	if (handle->time_series_buffer != NULL)
	{
		acc_integration_mem_free(handle->time_series_buffer);
	}

	if (handle->bin_rad_vs != NULL)
	{
		acc_integration_mem_free(handle->bin_rad_vs);
	}

	if (handle->bin_vertical_vs != NULL)
	{
		acc_integration_mem_free(handle->bin_vertical_vs);
	}

	if (handle->lp_psds != NULL)
	{
		acc_integration_mem_free(handle->lp_psds);
	}

	if (handle->fft_out != NULL)
	{
		acc_integration_mem_free(handle->fft_out);
	}

	if (handle->psds != NULL)
	{
		acc_integration_mem_free(handle->psds);
	}

	if (handle->psd != NULL)
	{
		acc_integration_mem_free(handle->psd);
	}

	if (handle->window != NULL)
	{
		acc_integration_mem_free(handle->window);
	}

	if (handle->threshold_check != NULL)
	{
		acc_integration_mem_free(handle->threshold_check);
	}

	if (handle->merged_velocities != NULL)
	{
		acc_integration_mem_free(handle->merged_velocities);
	}

	if (handle->merged_energies != NULL)
	{
		acc_integration_mem_free(handle->merged_energies);
	}

	if (handle->peak_indexes != NULL)
	{
		acc_integration_mem_free(handle->peak_indexes);
	}
}


static void set_profile(acc_surface_velocity_config_t *config)
{
	float    optimal_distance = config->surface_distance / cosf(config->sensor_angle * (((float)M_PI) / 180.0f));
	uint16_t optimal_point    = (int)ceil(optimal_distance / APPROX_BASE_STEP_LENGTH_M);
	int32_t  start_point      = rint(optimal_point - ((int)floor((acc_config_num_points_get(
									      config->sensor_config) - 1)) / 2) *
	                                 acc_config_step_length_get(config->sensor_config));

	acc_config_start_point_set(config->sensor_config, start_point);

	acc_config_profile_t profile = acc_algorithm_select_profile(start_point, APPROX_BASE_STEP_LENGTH_M);

	acc_config_profile_set(config->sensor_config, profile);
}


static void set_prf(acc_surface_velocity_config_t *config)
{
	uint16_t end_point = acc_config_start_point_get(config->sensor_config) + (acc_config_num_points_get(config->sensor_config) - 1U) *
	                     acc_config_step_length_get(config->sensor_config);

	acc_config_prf_set(config->sensor_config,
	                   acc_algorithm_select_prf(end_point, acc_config_profile_get(config->sensor_config), APPROX_BASE_STEP_LENGTH_M));
}


static void set_config(acc_surface_velocity_config_t *config)
{
	config->surface_distance      = CONFIG_SURFACE_DISTANCE;
	config->sensor_angle          = CONFIG_SENSOR_ANGLE;
	config->psd_lp_coeff          = CONFIG_PSD_LP_COEFF;
	config->threshold_sensitivity = CONFIG_THRESHOLD_SENSITIVITY;
	config->velocity_lp_coeff     = CONFIG_VELOCITY_LP_COEFF;

	acc_config_hwaas_set(config->sensor_config, CONFIG_HWAAS);
	acc_config_sweep_rate_set(config->sensor_config, CONFIG_SWEEP_RATE);

	config->time_series_length    = 512U;
	config->slow_zone_half_length = 3U;
	config->cfar_guard            = 6U;
	config->cfar_win              = 6U;
	config->max_peak_interval_s   = 4.0f;

	acc_config_num_points_set(config->sensor_config, 4U);
	acc_config_step_length_set(config->sensor_config, 12U);
	acc_config_sweeps_per_frame_set(config->sensor_config, 128U);
	acc_config_frame_rate_set(config->sensor_config, 0.0f);
	acc_config_double_buffering_set(config->sensor_config, true);
	acc_config_continuous_sweep_mode_set(config->sensor_config, true);
	acc_config_inter_frame_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
	acc_config_inter_sweep_idle_state_set(config->sensor_config, ACC_CONFIG_IDLE_STATE_READY);
}


static bool validate_config(acc_surface_velocity_config_t *config)
{
	uint16_t num_subsweeps = acc_config_num_subsweeps_get(config->sensor_config);
	float    sweep_rate    = acc_config_sweep_rate_get(config->sensor_config);
	uint16_t start_point   = acc_config_start_point_get(config->sensor_config);

	bool status = true;

	if (num_subsweeps != 1U)
	{
		printf("Multiple subsweeps are not supported\n");
		status = false;
	}

	if (sweep_rate == 0.0f)
	{
		printf("Sweep rate must be set\n");
		status = false;
	}

	if (start_point * APPROX_BASE_STEP_LENGTH_M <= config->surface_distance)
	{
		printf("Start point must be > surface distance");
		status = false;
	}

	if (config->sensor_angle <= 0 || config->sensor_angle >= 90)
	{
		printf("Sensor angle must be between 0 and 90 degrees");
		status = false;
	}

	return status;
}


static bool do_sensor_calibration_and_prepare(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config)
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

		status = acc_sensor_prepare(handle->sensor, config->sensor_config, &cal_result, handle->buffer, handle->buffer_size);
	}

	return status;
}


static bool init_sensor_resources(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config)
{
	handle->processing = acc_processing_create(config->sensor_config, &handle->proc_meta);
	if (handle->processing == NULL)
	{
		printf("acc_processing_create() failed\n");
		return false;
	}

	if (!acc_rss_get_buffer_size(config->sensor_config, &handle->buffer_size))
	{
		printf("acc_rss_get_buffer_size() failed\n");
		return false;
	}

	handle->buffer = acc_integration_mem_alloc(handle->buffer_size);
	if (handle->buffer == NULL)
	{
		printf("buffer allocation failed\n");
		return false;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	handle->sensor = acc_sensor_create(SENSOR_ID);
	if (handle->sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		return false;
	}

	if (!do_sensor_calibration_and_prepare(handle, config))
	{
		printf("do_sensor_calibration_and_prepare() failed\n");
		acc_sensor_status(handle->sensor);
		cleanup(handle, config);
		return EXIT_FAILURE;
	}

	return true;
}


static bool init_handle(acc_surface_velocity_handle_t *handle, acc_surface_velocity_config_t *config)
{
	handle->wait_n       = 0U;
	handle->update_index = 0U;
	handle->lp_velocity  = 0.0f;
	handle->vertical_v   = 0.0f;

	handle->surface_velocity_config.cfar_guard            = config->cfar_guard;
	handle->surface_velocity_config.cfar_win              = config->cfar_win;
	handle->surface_velocity_config.slow_zone_half_length = config->slow_zone_half_length;
	handle->surface_velocity_config.velocity_lp_coeff     = config->velocity_lp_coeff;
	handle->surface_velocity_config.surface_distance      = config->surface_distance;
	handle->surface_velocity_config.psd_lp_coeff          = config->psd_lp_coeff;

	if (config->threshold_sensitivity <= 0.0f)
	{
		printf("Invalid CFAR sensitivity config\n");
		return false;
	}

	handle->surface_velocity_config.threshold_sensitivity = config->threshold_sensitivity;

	handle->num_distances      = acc_config_num_points_get(config->sensor_config);
	handle->sweep_rate         = acc_config_sweep_rate_get(config->sensor_config);
	handle->start_point        = acc_config_start_point_get(config->sensor_config);
	handle->sweeps_per_frame   = acc_config_sweeps_per_frame_get(config->sensor_config);
	handle->step_length        = acc_config_step_length_get(config->sensor_config);
	handle->base_step_length_m = acc_processing_points_to_meter(1);

	float estimated_frame_rate;
	if (acc_config_frame_rate_get(config->sensor_config) == 0.0f)
	{
		estimated_frame_rate = handle->sweep_rate / handle->sweeps_per_frame;
	}
	else
	{
		estimated_frame_rate = acc_config_frame_rate_get(config->sensor_config);
	}

	handle->max_peak_interval_n = config->max_peak_interval_s * estimated_frame_rate;

	if (!acc_config_continuous_sweep_mode_get(config->sensor_config))
	{
		handle->surface_velocity_config.time_series_length = handle->sweeps_per_frame;
	}
	else
	{
		if (handle->sweeps_per_frame > config->time_series_length)
		{
			printf("Too many sweeps per frame to be handled\n");
			return false;
		}
		else
		{
			handle->surface_velocity_config.time_series_length = config->time_series_length;
		}
	}

	handle->segment_length = handle->surface_velocity_config.time_series_length / 4U;

	if (handle->segment_length % 2U != 0U)
	{
		handle->segment_length += 1U;
	}

	handle->padded_segment_length_shift = 0U;
	handle->padded_segment_length       = 1U << handle->padded_segment_length_shift;

	while (handle->padded_segment_length < handle->segment_length)
	{
		handle->padded_segment_length_shift++;
		handle->padded_segment_length = 1U << handle->padded_segment_length_shift;
	}

	handle->middle_index = rint((float)handle->segment_length / 2.0f);

	handle->double_buffer_filter_buffer =
		acc_integration_mem_alloc((handle->sweeps_per_frame - 2U) * sizeof(*handle->double_buffer_filter_buffer));
	handle->time_series =
		acc_integration_mem_alloc(handle->surface_velocity_config.time_series_length * handle->num_distances * sizeof(*handle->time_series));
	handle->time_series_buffer = acc_integration_mem_alloc(
		handle->segment_length * sizeof(*handle->time_series_buffer));
	handle->frame           = acc_integration_mem_alloc(handle->num_distances * handle->sweeps_per_frame * sizeof(*handle->frame));
	handle->bin_rad_vs      = acc_integration_mem_alloc(handle->segment_length * sizeof(*handle->bin_rad_vs));
	handle->bin_vertical_vs = acc_integration_mem_alloc(handle->segment_length * sizeof(*handle->bin_vertical_vs));
	handle->lp_psds         = acc_integration_mem_alloc(handle->segment_length * handle->num_distances * sizeof(*handle->lp_psds));
	handle->fft_out         = acc_integration_mem_alloc(handle->padded_segment_length * sizeof(*handle->fft_out));
	handle->psds            = acc_integration_mem_alloc(handle->segment_length * handle->num_distances * sizeof(*handle->psds));
	handle->psd             = acc_integration_mem_alloc(handle->segment_length * sizeof(*handle->psd));
	handle->window          = acc_integration_mem_alloc(handle->segment_length * sizeof(*handle->window));

	size_t threshold_check_length =
		acc_alg_basic_utils_calculate_length_of_bitarray_uint32(handle->segment_length);
	handle->threshold_check = acc_integration_mem_alloc(threshold_check_length * sizeof(*handle->threshold_check));

	handle->merged_peaks_length = handle->segment_length / 2U;
	handle->merged_velocities   = acc_integration_mem_alloc(handle->merged_peaks_length * sizeof(*handle->merged_velocities));
	handle->merged_energies     = acc_integration_mem_alloc(handle->merged_peaks_length * sizeof(*handle->merged_energies));
	handle->num_merged_peaks    = 0U;

	handle->peak_indexes_length = handle->segment_length / 2U;
	handle->peak_indexes        = acc_integration_mem_alloc(handle->peak_indexes_length * sizeof(*handle->peak_indexes));
	handle->num_peaks           = 0U;

	bool alloc_success =
		handle->double_buffer_filter_buffer && handle->time_series != NULL && handle->time_series_buffer != NULL && handle->frame != NULL &&
		handle->bin_rad_vs != NULL && handle->bin_vertical_vs != NULL && handle->lp_psds != NULL &&
		handle->fft_out != NULL && handle->psds != NULL && handle->window != NULL && handle->threshold_check != NULL &&
		handle->merged_velocities != NULL && handle->merged_energies != NULL && handle->peak_indexes != NULL;

	if (!alloc_success)
	{
		printf("Failed to allocate resources for the surface velocity handle\n");
		return false;
	}

	memset(handle->time_series, 0,
	       handle->surface_velocity_config.time_series_length * handle->num_distances * sizeof(*handle->time_series));
	memset(handle->lp_psds, 0,
	       handle->segment_length * handle->num_distances * sizeof(*handle->lp_psds));
	memset(handle->psds, 0,
	       handle->segment_length * handle->num_distances * sizeof(*handle->psds));
	memset(handle->merged_velocities, 0,
	       handle->merged_peaks_length * sizeof(*handle->merged_velocities));
	memset(handle->merged_energies, 0,
	       handle->merged_peaks_length * sizeof(*handle->merged_energies));

	acc_algorithm_hann(handle->segment_length, handle->window);

	acc_algorithm_fftfreq(handle->segment_length, 1.0f / handle->sweep_rate, handle->bin_rad_vs);
	acc_algorithm_fftshift(handle->bin_rad_vs, handle->segment_length);

	float perceived_wavelength = get_perceived_wavelength();

	for (uint16_t i = 0U; i < handle->segment_length; i++)
	{
		handle->bin_rad_vs[i] *= perceived_wavelength;
	}

	return true;
}


static bool measure(acc_surface_velocity_handle_t *handle)
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


/**
 * @brief Calculate PSD (power spectral density)
 *
 * @param handle Surface velocity handle
 * @return Distance index
 */
static uint16_t calc_power_spectral_density(acc_surface_velocity_handle_t *handle)
{
	for (uint16_t i = 0U; i < handle->sweeps_per_frame; i++)
	{
		for (uint16_t j = 0U; j < handle->num_distances; j++)
		{
			uint16_t index = i * handle->num_distances + j;

			handle->frame[j] = ((float)handle->proc_result.frame[index].real) + ((float)handle->proc_result.frame[index].imag) * I;
		}

		acc_algorithm_roll_and_push_matrix_f32_complex(handle->time_series, handle->surface_velocity_config.time_series_length,
		                                               handle->num_distances,
		                                               handle->frame,
		                                               false);
	}

	acc_algorithm_welch_matrix(handle->time_series, handle->surface_velocity_config.time_series_length, handle->num_distances,
	                           handle->segment_length, handle->time_series_buffer, handle->fft_out, handle->psds, handle->window,
	                           handle->padded_segment_length_shift, handle->sweep_rate);

	acc_algorithm_fftshift_matrix(handle->psds, handle->segment_length, handle->num_distances);

	if (handle->update_index * handle->sweeps_per_frame < handle->surface_velocity_config.time_series_length)
	{
		for (uint16_t i = 0U; i < handle->segment_length; i++)
		{
			for (uint16_t j = 0U; j < handle->num_distances; j++)
			{
				handle->lp_psds[i * handle->num_distances + j] = handle->psds[i * handle->num_distances + j];
			}
		}
	}

	for (uint16_t i = 0U; i < handle->segment_length; i++)
	{
		for (uint16_t j = 0U; j < handle->num_distances; j++)
		{
			handle->lp_psds[i * handle->num_distances + j] = handle->lp_psds[i * handle->num_distances + j] *
			                                                 handle->surface_velocity_config.psd_lp_coeff +
			                                                 handle->psds[i * handle->num_distances + j] *
			                                                 (1 - handle->surface_velocity_config.psd_lp_coeff);
		}
	}

	uint16_t index = acc_algorithm_get_distance_idx(handle->lp_psds, handle->num_distances, handle->segment_length, handle->middle_index,
	                                                handle->surface_velocity_config.slow_zone_half_length);

	for (uint16_t i = 0U; i < handle->segment_length; i++)
	{
		handle->psd[i] = handle->lp_psds[i * handle->num_distances + index];
	}

	return index;
}


static bool process(acc_surface_velocity_handle_t *handle, acc_surface_velocity_result_t *result)
{
	bool status = false;

	acc_algorithm_double_buffering_frame_filter(handle->proc_result.frame, handle->sweeps_per_frame, handle->num_distances,
	                                            handle->double_buffer_filter_buffer);

	uint16_t distance_index = calc_power_spectral_density(handle);
	float    distance       =
		acc_algorithm_get_distance_m(handle->step_length, handle->start_point, handle->base_step_length_m, distance_index);
	float angle_correction = get_angle_correction(handle->surface_velocity_config.surface_distance, distance);

	for (uint16_t i = 0U; i < handle->segment_length; i++)
	{
		handle->bin_vertical_vs[i] = handle->bin_rad_vs[i] * angle_correction;
	}

	update_threshold(handle);
	handle->num_peaks = 0U;
	status            = acc_algorithm_find_peaks(handle->psd, handle->segment_length, handle->threshold_check, handle->peak_indexes,
	                                             handle->peak_indexes_length,
	                                             &(handle->num_peaks));
	if (status)
	{
		if (handle->num_peaks > 0U)
		{
			float max_abs_bin_vertical_v = -INFINITY;
			for (uint16_t i = 0U; i < handle->num_peaks; i++)
			{
				float abs_bin_vertical_v =  fabsf(handle->bin_vertical_vs[handle->peak_indexes[i]]);

				max_abs_bin_vertical_v = fmax(abs_bin_vertical_v, max_abs_bin_vertical_v);
			}

			if (max_abs_bin_vertical_v > handle->bin_vertical_vs[handle->surface_velocity_config.slow_zone_half_length])
			{
				status = get_velocity_estimate(handle);

				if (!status)
				{
					printf("Failed to merge peaks\n");
				}
			}
			else
			{
				uint16_t velocity_index = handle->middle_index + handle->surface_velocity_config.slow_zone_half_length;

				handle->vertical_v = acc_algorithm_get_peak_velocity(handle->bin_vertical_vs,
				                                                     handle->psd,
				                                                     handle->peak_indexes, handle->num_peaks,
				                                                     handle->bin_vertical_vs[velocity_index]);
			}

			if (status)
			{
				if (fabsf(handle->lp_velocity) > 0.0f && handle->vertical_v / handle->lp_velocity < 0.8f)
				{
					if (handle->wait_n < handle->max_peak_interval_n)
					{
						handle->vertical_v = handle->lp_velocity;
						handle->wait_n    += 1U;
					}
				}
				else
				{
					handle->wait_n = 0U;
				}
			}
		}
		else
		{
			if (handle->wait_n < handle->max_peak_interval_n)
			{
				handle->vertical_v = handle->lp_velocity;
				handle->wait_n    += 1U;
			}
			else
			{
				handle->vertical_v = 0.0f;
			}
		}

		if (status)
		{
			float sf = calc_dynamic_smoothing_factor(handle->surface_velocity_config.velocity_lp_coeff, handle->update_index);

			if (handle->update_index * handle->sweeps_per_frame > handle->surface_velocity_config.time_series_length)
			{
				handle->lp_velocity = sf * handle->lp_velocity + (1.0f - sf) * handle->vertical_v;
			}

			handle->update_index += 1U;

			result->estimated_v = handle->lp_velocity;
			result->distance_m  = distance;
		}
	}
	else
	{
		printf("Failed to detect peaks\n");
	}

	return status;
}


static void print_result(acc_surface_velocity_result_t *result)
{
	printf("Velocity: %" PRIfloat " m/s, distance: %" PRIfloat " m\n", ACC_LOG_FLOAT_TO_INTEGER(result->estimated_v),
	       ACC_LOG_FLOAT_TO_INTEGER(result->distance_m));
}


static float get_perceived_wavelength(void)
{
	const float speed_of_light  = 299792458.0f;
	const float radio_frequency = 60.5e9f;
	const float wavelength      = speed_of_light / radio_frequency;

	return wavelength / 2.0f;
}


/**
 * @brief Calculate a dynamic smoothing factor
 *
 * @param[in] static_sf The target smoothing factor that will be used after the first sweeps
 * @param[in] update_count The update count should be 0 for the first sweep and increased by one for each update
 * @return Dynamic smoothing factor
 */
static float calc_dynamic_smoothing_factor(float static_sf, uint32_t update_count)
{
	return fminf(static_sf, 1.0f - 1.0f / (1.0f + update_count));
}
