// Copyright (c) Acconeer AB, 2022-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_rss_a121.h"
#include "acc_version.h"


/** \example example_diagnostic_test.c
 * @brief This is an example on how to use RSS to execute the diagnostic test
 * @n
 * The example executes as follows:
 *   - Register the RSS HAL
 *   - Power on the sensor
 *   - Create diagnostic test
 *   - Enable the sensor
 *   - Execute diagnostic test
 *   - Destroy diagnostic test
 *   - Disable the sensor
 *   - Power off the sensor
 */

#define SENSOR_ID (1U)

#define SENSOR_TIMEOUT_MS (1000U)

int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	bool test_ok = true;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	void *buffer = acc_integration_mem_alloc(ACC_RSS_ASSEMBLY_TEST_MIN_BUFFER_SIZE);
	if (buffer == NULL)
	{
		printf("Diagnostic test: Memory allocation failed\n");
		return EXIT_FAILURE;
	}

	acc_rss_test_state_t              test_state         = ACC_RSS_TEST_STATE_ONGOING;
	acc_rss_test_integration_status_t integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_OK;

	acc_rss_assembly_test_t *assembly_test = acc_rss_assembly_test_create(SENSOR_ID, buffer, ACC_RSS_ASSEMBLY_TEST_MIN_BUFFER_SIZE);

	if (assembly_test != NULL)
	{
		/* Enable all tests */
		acc_rss_assembly_test_enable_all_tests(assembly_test);

		/* Enable the diagnostic logs from the assembly test */
		acc_rss_assembly_test_enable_diagnostic_logs();

		acc_hal_integration_sensor_supply_on(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);

		do
		{
			test_state = acc_rss_assembly_test_execute(assembly_test, integration_status);

			switch (test_state)
			{
				case ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN:
					acc_hal_integration_sensor_disable(SENSOR_ID);
					acc_hal_integration_sensor_enable(SENSOR_ID);
					integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_OK;
					break;
				case ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT:
					if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
					{
						/* Wait for interrupt failed */
						integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_TIMEOUT;
					}

					break;
				default:
					integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_OK;
					break;
			}
		} while (test_state != ACC_RSS_TEST_STATE_COMPLETE);

		acc_rss_assembly_test_destroy(assembly_test);

		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_supply_off(SENSOR_ID);
	}
	else
	{
		printf("Diagnostic test: Could not create assembly test\n");
		test_ok = false;
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}

	if (test_ok)
	{
		printf("Diagnostic test: All tests passed\n");

		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
