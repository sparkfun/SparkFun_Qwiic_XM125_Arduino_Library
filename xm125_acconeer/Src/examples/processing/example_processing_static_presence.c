// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.


/** \example example_processing_static_presence.c
 * @brief example_processing_static_presence.c
 * Example program that shows how the spread of the phase values can be used
 * to determine if there is a static object in front of the radar.
 *
 * In this example we use the the ratio of the coherent average to the noncoherent
 * average to calculate the spread and determine if the phase values are aligned or
 * not. It is also possible to measure the spread in other way, for example by
 * calculating the standard deviation of the phase values.
 *
 * Note that this example is mainly intended for detecting the precense of static
 * objects in front of the radar sensor, while Acconeer's presence detector detects
 * objects that moves or vibrates sligtly, ignoring static objects.
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "acc_config.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration_log.h"
#include "acc_rss_a121.h"
#include "acc_version.h"


#include "acc_processing_helpers.h"


#define SENSOR_ID (1U)

#define SWEEPS_PER_FRAME      16
#define PHASE_SPREAD_TRESHOLD (0.01f)


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
		acc_control_helper_destroy(&control_helper_state);
		return EXIT_FAILURE;
	}

	uint32_t sweep_data_length   = control_helper_state.proc_meta.sweep_data_length;
	uint32_t point_vector_length = SWEEPS_PER_FRAME;

	acc_vector_float_t *phase_spread = acc_vector_float_alloc(sweep_data_length);
	acc_vector_iq_t    *point_vector = acc_vector_iq_alloc(point_vector_length);

	bool mem_ok = (phase_spread != NULL) && (point_vector != NULL);
	if (!mem_ok)
	{
		printf("Memory allocation for vectors failed\n");
		goto clean_up;
	}

	uint32_t iterations = 5U;
	for (uint32_t i = 0U; i < iterations; i++)
	{
		res = acc_control_helper_get_next(&control_helper_state);
		if (!res)
		{
			printf("acc_control_helper_get_next() failed\n");
			break;
		}

		for (uint32_t p = 0U; p < sweep_data_length; p++)
		{
			acc_get_iq_point_vector(&control_helper_state, p, point_vector);

			// The ratio of the coherent average to the noncoherent average is used to determine
			// the amount of phase spread. When the phases of the values in the array are similar,
			// the coherent average approaches the noncoherent average. However, if the phase
			// values are uniformly distributed around the unit circle and have the same
			// amplitudes, they cancel out, resulting in a coherent average of 0, while the
			// noncoherent average is still the average of the amplitudes.

			// As a result, when there is an object in front of the radar and the spread is low,
			// the ratio is close to 1. When there is no object in front of the radar and the
			// phase values are random, the ratio is closer to 0.

			// We set the phase spread to be 1 minus the ratio.

			float coherent_mean    = acc_vector_iq_coherent_mean_amplitude(point_vector);
			float noncoherent_mean = acc_vector_iq_noncoherent_mean_amplitude(point_vector);

			phase_spread->data[p] = 1 - coherent_mean / noncoherent_mean;
		}

		// Print a line with a dot or star for each distance point. A star ('*') means that there
		// is an object in front of the radar at the distance point and a dot ('.') means that
		// there is no object in front of the radar.
		for (uint32_t p = 0U; p < sweep_data_length; p++)
		{
			printf("%c", phase_spread->data[p] > PHASE_SPREAD_TRESHOLD ? '.' : '*');
		}

		printf("\n");
	}

clean_up:
	acc_vector_iq_free(point_vector);
	acc_vector_float_free(phase_spread);
	acc_control_helper_destroy(&control_helper_state);

	if (res && mem_ok)
	{
		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}


static void update_configuration(acc_config_t *config)
{
	int32_t  start_point = 100;  // start at 250 mm
	uint16_t step_length = 24;   // 24*2.5 mm = 60 mm
	uint16_t num_points  = 20;   // range length 20*24*2.5 mm = 1200 mm

	acc_config_start_point_set(config, start_point);
	acc_config_num_points_set(config, num_points);
	acc_config_step_length_set(config, step_length);
	acc_config_profile_set(config, ACC_CONFIG_PROFILE_3);
	acc_config_hwaas_set(config, 30);
	// The processing in this example assumes that sweeps_per_frame > 1
	acc_config_sweeps_per_frame_set(config, SWEEPS_PER_FRAME);
	acc_config_prf_set(config, ACC_CONFIG_PRF_13_0_MHZ);
}
