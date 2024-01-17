// Copyright (c) Acconeer AB, 2020-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_config.h"
#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_processing.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"

#include "acc_control_helper.h"

/** \example example_control_helper.c
 * @brief This is an example on how the control helper API can be used
 * @n
 * The example executes as follows:
 *   - Create a control helper instance
 *   - Configure the start point
 *   - Activate the radar
 *   - Read out a number of measurements
 *   - Destroy the control helper instance
 */

#define SENSOR_ID          (1U)
#define MAX_DATA_ENTRY_LEN 15 // "-32000+-32000i" + zero termination

static void print_data(acc_int16_complex_t *data, uint16_t data_length);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	acc_control_helper_t helper;
	if (!acc_control_helper_create(&helper, SENSOR_ID))
	{
		printf("acc_control_helper_create() failed\n");
		return EXIT_FAILURE;
	}

	acc_config_start_point_set(helper.config, 200);

	if (!acc_control_helper_activate(&helper))
	{
		printf("acc_control_helper_activate() failed\n");
		acc_control_helper_destroy(&helper);
		return EXIT_FAILURE;
	}

	for (uint32_t i = 0U; i < 5U; i++)
	{
		if (!acc_control_helper_get_next(&helper))
		{
			printf("acc_control_helper_get_next failed\n");
			acc_control_helper_destroy(&helper);
			return EXIT_FAILURE;
		}

		print_data(helper.proc_result.frame, helper.proc_meta.frame_data_length);
	}

	acc_control_helper_destroy(&helper);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}


static void print_data(acc_int16_complex_t *data, uint16_t data_length)
{
	printf("Processed data:\n");
	char buffer[MAX_DATA_ENTRY_LEN];

	for (uint16_t i = 0; i < data_length; i++)
	{
		if ((i > 0) && ((i % 8) == 0))
		{
			printf("\n");
		}

		snprintf(buffer, sizeof(buffer), "%" PRIi16 "%+" PRIi16 "i", data[i].real, data[i].imag);

		printf("%14s ", buffer);
	}

	printf("\n");
}
