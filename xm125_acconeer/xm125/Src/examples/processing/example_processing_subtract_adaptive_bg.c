// Copyright (c) Acconeer AB, 2022-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.


/** \example example_processing_subtract_adaptive_bg.c
 * @brief example_processing_subtract_adaptive_bg.c
 * This program shows how to estimate background reflections by calculating a coherent
 * exponential average over the incomming radar measurements. As we use IQ data and
 * include the phase information when we do the calculations, it is possible to
 * subtract the background that contains all static - or at least almost static -
 * reflections from the signal. What we have left are the reflections from objects
 * in motion.
 */


#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "acc_config.h"
#include "acc_definitions_a121.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration_log.h"
#include "acc_rss_a121.h"
#include "acc_version.h"

#include "acc_control_helper.h"
#include "acc_processing_helpers.h"


#define SENSOR_ID (1U)


static void update_configuration(acc_config_t *config);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	acc_control_helper_t control_helper_state = {0};

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	bool res = acc_control_helper_create(&control_helper_state, SENSOR_ID);
	if (!res)
	{
		printf("acc_control_helper_create() failed\n");
		return EXIT_FAILURE;
	}

	update_configuration(control_helper_state.config);

	res = acc_control_helper_activate(&control_helper_state);
	if (!res)
	{
		printf("acc_control_helper_activate() failed\n");
		return EXIT_FAILURE;
	}

	uint32_t sweep_data_length = control_helper_state.proc_meta.sweep_data_length;

	acc_vector_iq_t *current_sweep_iq       = acc_vector_iq_alloc(sweep_data_length);
	acc_vector_iq_t *adaptive_background_iq = acc_vector_iq_alloc(sweep_data_length);
	acc_vector_iq_t *motion_reflections_iq  = acc_vector_iq_alloc(sweep_data_length);

	acc_vector_float_t *adaptive_background_amplitude = acc_vector_float_alloc(sweep_data_length);
	acc_vector_float_t *motion_reflections_amplitude  = acc_vector_float_alloc(sweep_data_length);

	bool mem_ok = (current_sweep_iq != NULL) && (adaptive_background_iq != NULL) &&
	              (motion_reflections_iq != NULL) && (adaptive_background_amplitude != NULL) &&
	              (motion_reflections_amplitude != NULL);

	if (!mem_ok)
	{
		printf("Memory allocation for vectors failed\n");
		goto clean_up;
	}

	float time_constant_static_background = 5.0f;
	float sf_adaptive_background          = acc_processing_helper_tc_to_sf(time_constant_static_background,
	                                                                       acc_config_frame_rate_get(control_helper_state.config));

	uint32_t iterations = 50U;

	for (uint32_t i = 0U; i < iterations; i++)
	{
		if (!acc_control_helper_get_next(&control_helper_state))
		{
			printf("acc_control_helper_get_next() failed\n");
			break;
		}

		acc_get_iq_sweep_vector(&control_helper_state, current_sweep_iq);

		float sf = acc_processing_helper_dynamic_sf(sf_adaptive_background, i);
		acc_vector_iq_update_exponential_average(current_sweep_iq, adaptive_background_iq, sf);
		acc_vector_iq_subtract(current_sweep_iq, adaptive_background_iq, motion_reflections_iq);

		acc_vector_iq_amplitude(adaptive_background_iq, adaptive_background_amplitude);
		acc_vector_iq_amplitude(motion_reflections_iq, motion_reflections_amplitude);

		acc_vector_float_print("Background amplitude", adaptive_background_amplitude);
		acc_vector_float_print("Motion amplitude", motion_reflections_amplitude);

		uint32_t max_peak_index_static = acc_vector_float_argmax(adaptive_background_amplitude);
		uint32_t max_peak_index_motion = acc_vector_float_argmax(motion_reflections_amplitude);
		printf("Highest background peak index : %" PRIu32 "\n", max_peak_index_static);
		printf("Highest motion peak index : %" PRIu32 "\n", max_peak_index_motion);
	}

clean_up:
	acc_vector_iq_free(current_sweep_iq);
	acc_vector_iq_free(adaptive_background_iq);
	acc_vector_iq_free(motion_reflections_iq);

	acc_vector_float_free(adaptive_background_amplitude);
	acc_vector_float_free(motion_reflections_amplitude);

	acc_control_helper_destroy(&control_helper_state);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void update_configuration(acc_config_t *config)
{
	int32_t  start_point = 100; // start at 250 mm
	uint16_t step_length = 12;
	uint16_t num_points  = 16; // range length 16*12*2.5mm = 480 mm

	acc_config_start_point_set(config, start_point);
	acc_config_num_points_set(config, num_points);
	acc_config_step_length_set(config, step_length);
	acc_config_profile_set(config, ACC_CONFIG_PROFILE_2);
	acc_config_hwaas_set(config, 30);
	// The processing in this example assumes that sweeps_per_frame = 1
	acc_config_sweeps_per_frame_set(config, 1);
	acc_config_frame_rate_set(config, 10);
	acc_config_prf_set(config, ACC_CONFIG_PRF_13_0_MHZ);
}
