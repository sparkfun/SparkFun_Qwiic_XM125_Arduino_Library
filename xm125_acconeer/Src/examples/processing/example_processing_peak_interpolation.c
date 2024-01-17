// Copyright (c) Acconeer AB, 2022-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <assert.h>
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


/** \example example_processing_peak_interpolation.c
 * @brief example_processing_peak_interpolation.c
 * This program shows how to apply a distance filter and how to interpolate the
 * position of the peak to improve distance accuracy.
 *
 * The distance filter is applied to the IQ data and will reduce noise and smooth out
 * the the amplitude of the radar eacho peaks. The smoothing is important for the
 * next step where we estimate the acctual position of the peak, i.e the position
 * of the peak that we would have gotten if we had used an shorter distance between
 * the measurement steps.
 */

int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	acc_control_helper_t control_helper_state = {0};

	const uint32_t peak_width_points = 104; // Approximate peak width for profile 3

	printf("Acconeer software version %s \n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	bool status_ok = acc_control_helper_create(&control_helper_state, SENSOR_ID);

	if (!status_ok)
	{
		printf("acc_control_helper_create ()failed \n");
		return EXIT_FAILURE;
	}

	update_configuration(control_helper_state.config);

	status_ok = acc_control_helper_activate(&control_helper_state);

	if (!status_ok)
	{
		printf("acc_control_helper_activate ()failed\n");
		acc_control_helper_destroy(&control_helper_state);
		return EXIT_FAILURE;
	}

	uint32_t sweep_data_length = control_helper_state.proc_meta.sweep_data_length;
	uint32_t filter_length     = acc_processing_helper_get_filter_length(peak_width_points,
	                                                                     acc_config_step_length_get(control_helper_state.config));

	acc_vector_iq_t *current_sweep_iq  = acc_vector_iq_alloc(sweep_data_length);
	acc_vector_iq_t *filtered_sweep_iq = acc_vector_iq_alloc(sweep_data_length);

	acc_vector_float_t *filter_vector            = acc_vector_float_alloc(filter_length);
	acc_vector_float_t *filtered_sweep_amplitude = acc_vector_float_alloc(sweep_data_length);

	bool mem_ok = (current_sweep_iq != NULL) && (filtered_sweep_iq != NULL) &&
	              (filter_vector != NULL) && (filtered_sweep_amplitude != NULL);
	if (!mem_ok)
	{
		printf("Memory allocation for vectors failed\n");
		goto clean_up;
	}

	acc_vector_float_create_depth_filter_vector(filter_vector);

	uint32_t iterations = 50U;
	for (uint32_t i = 0U; i < iterations; i++)
	{
		if (!acc_control_helper_get_next(&control_helper_state))
		{
			printf("acc_control_helper_get_next() failed\n");
			break;
		}

		acc_get_iq_sweep_vector(&control_helper_state, current_sweep_iq);

		// Apply distance filter to smooth out the amplitude of the radar signal and
		// reduce noise. Note that phase enhancement should be enabled when applying
		// the distance filter on an IQ data vector.
		acc_vector_iq_apply_filter(current_sweep_iq, filter_vector, filtered_sweep_iq);

		// Find the index of the element with the highest amplitude. We skip the first and last
		// element of the vector in the search as we would make an of bounds read in the next step
		// if the max value was found in the first or last element.
		acc_vector_iq_amplitude(filtered_sweep_iq, filtered_sweep_amplitude);

		uint32_t max_peak_index = acc_vector_float_argmax_skip_edges(filtered_sweep_amplitude, 1);

		// We know that the distance between steps is constant, so it is enough to
		// use only the peak amplitude and the amplitudes before and after the peak to
		// estimate the location of the peak relative to the location of the max value.
		float peak_offset = acc_processing_helper_interpolate_peak_position(filtered_sweep_amplitude->data[max_peak_index - 1],
		                                                                    filtered_sweep_amplitude->data[max_peak_index],
		                                                                    filtered_sweep_amplitude->data[max_peak_index + 1]);

		// Calculate the distance to the start of the measuring range and the distance between
		// measurement steps.
		float start       = acc_processing_points_to_meter(acc_config_start_point_get(control_helper_state.config));
		float step_length = acc_processing_points_to_meter(acc_config_step_length_get(control_helper_state.config));

		// Make the final calculation of the distance to the reflecting object using the index of the
		// value with the higest amplitude and the interpolated peak offset.
		float distance = start + (max_peak_index + peak_offset) * step_length;

		printf("Interpolated distance: %d mm\n", (int)(distance * 1000 + .5f));
	}

clean_up:
	acc_control_helper_destroy(&control_helper_state);

	acc_vector_iq_free(current_sweep_iq);
	acc_vector_iq_free(filtered_sweep_iq);
	acc_vector_float_free(filter_vector);
	acc_vector_float_free(filtered_sweep_amplitude);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void update_configuration(acc_config_t *config)
{
	int32_t  start_point = 100; // start at approx 100*2.5 mm ~= 250 mm
	uint16_t step_length = 8;   // 8 * 2.5 mm ~= 20 mm between each step
	uint16_t num_points  = 50;  // range length ~= 250 mm + 50*20 mm ~= 1250 mm

	acc_config_start_point_set(config, start_point);
	acc_config_num_points_set(config, num_points);
	acc_config_step_length_set(config, step_length);
	acc_config_profile_set(config, ACC_CONFIG_PROFILE_3);
	acc_config_hwaas_set(config, 200);
	// The processing in this example assumes that sweeps_per_frame = 1
	acc_config_sweeps_per_frame_set(config, 1);
	acc_config_frame_rate_set(config, 5.0f);
	acc_config_phase_enhancement_set(config, true);
	acc_config_prf_set(config, ACC_CONFIG_PRF_13_0_MHZ);
}
