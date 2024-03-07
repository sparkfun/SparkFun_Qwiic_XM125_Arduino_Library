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


/** \example example_bring_up.c
 * @brief This is an example on how the assembly test can be used to ease bring-up
 * @n
 * The example executes as follows:
 *   - Register the RSS HAL
 *   - Create assembly test
 *   - Enable one assembly test at a time
 *     - Power on the sensor
 *     - Enable the sensor
 *     - Exceute assembly test
 *     - Disable the sensor
 *     - Power off the sensor
 *   - Destroy assembly test
 */


#define SENSOR_ID (1U)

#define SENSOR_TIMEOUT_MS (1000U)

static bool run_test(acc_rss_assembly_test_t *assembly_test, acc_sensor_id_t sensor_id);


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
		printf("Assembly test: Memory allocation failed\n");
		return EXIT_FAILURE;
	}

	// Create
	acc_rss_assembly_test_t *assembly_test = acc_rss_assembly_test_create(SENSOR_ID, buffer, ACC_RSS_ASSEMBLY_TEST_MIN_BUFFER_SIZE);
	if (assembly_test == NULL)
	{
		printf("Bring-up: Could not create assembly test\n");
		acc_integration_mem_free(buffer);
		return EXIT_FAILURE;
	}

	// Enable and run: Basic read test
	acc_rss_assembly_test_disable_all_tests(assembly_test);
	acc_rss_assembly_test_enable(assembly_test, ACC_RSS_ASSEMBLY_TEST_ID_BASIC_READ);
	if (!run_test(assembly_test, SENSOR_ID))
	{
		printf("Bring-up: Basic read test failed\n");
		test_ok = false;
	}

	// Enable and run: Communication test
	acc_rss_assembly_test_disable_all_tests(assembly_test);
	acc_rss_assembly_test_enable(assembly_test, ACC_RSS_ASSEMBLY_TEST_ID_COMMUNICATION);
	if (!run_test(assembly_test, SENSOR_ID))
	{
		printf("Bring-up: Communication test failed\n");
		test_ok = false;
	}

	// Enable and run: Enable test
	acc_rss_assembly_test_disable_all_tests(assembly_test);
	acc_rss_assembly_test_enable(assembly_test, ACC_RSS_ASSEMBLY_TEST_ID_ENABLE_PIN);
	if (!run_test(assembly_test, SENSOR_ID))
	{
		printf("Bring-up: Enable test failed\n");
		test_ok = false;
	}

	// Enable and run: Interrupt test
	acc_rss_assembly_test_disable_all_tests(assembly_test);
	acc_rss_assembly_test_enable(assembly_test, ACC_RSS_ASSEMBLY_TEST_ID_INTERRUPT);
	if (!run_test(assembly_test, SENSOR_ID))
	{
		printf("Bring-up: Interrupt test failed\n");
		test_ok = false;
	}

	// Enable and run: Clock and Supply test
	acc_rss_assembly_test_disable_all_tests(assembly_test);
	acc_rss_assembly_test_enable(assembly_test, ACC_RSS_ASSEMBLY_TEST_ID_CLOCK_AND_SUPPLY);
	if (!run_test(assembly_test, SENSOR_ID))
	{
		printf("Bring-up: Clock and Supply test failed\n");
		test_ok = false;
	}

	// Enable and run: Sensor calibration test
	acc_rss_assembly_test_disable_all_tests(assembly_test);
	acc_rss_assembly_test_enable(assembly_test, ACC_RSS_ASSEMBLY_TEST_ID_SENSOR_CALIBRATION);
	if (!run_test(assembly_test, SENSOR_ID))
	{
		printf("Bring-up: Calibration test failed\n");
		test_ok = false;
	}

	// Destroy
	acc_rss_assembly_test_destroy(assembly_test);

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}

	if (test_ok)
	{
		printf("Bring-up: All tests passed\n");

		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}


static bool run_test(acc_rss_assembly_test_t *assembly_test, acc_sensor_id_t sensor_id)
{
	bool all_passed = true;

	acc_hal_integration_sensor_supply_on(sensor_id);
	acc_hal_integration_sensor_enable(sensor_id);

	acc_rss_test_state_t              assembly_test_state = ACC_RSS_TEST_STATE_ONGOING;
	acc_rss_test_integration_status_t integration_status  = ACC_RSS_TEST_INTEGRATION_STATUS_OK;

	do
	{
		assembly_test_state = acc_rss_assembly_test_execute(assembly_test, integration_status);

		switch (assembly_test_state)
		{
			case ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN:
				acc_hal_integration_sensor_disable(sensor_id);
				acc_hal_integration_sensor_enable(sensor_id);
				integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_OK;
				break;
			case ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT:
				if (!acc_hal_integration_wait_for_sensor_interrupt(sensor_id, SENSOR_TIMEOUT_MS))
				{
					/* Wait for interrupt failed */
					integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_TIMEOUT;
				}
				else
				{
					integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_OK;
				}

				break;
			default:
				integration_status = ACC_RSS_TEST_INTEGRATION_STATUS_OK;
				break;
		}
	} while (assembly_test_state != ACC_RSS_TEST_STATE_COMPLETE);

	acc_hal_integration_sensor_disable(sensor_id);
	acc_hal_integration_sensor_supply_off(sensor_id);

	uint16_t nbr_of_test_results = 0U;

	const acc_rss_assembly_test_result_t *test_results = acc_rss_assembly_test_get_results(assembly_test, &nbr_of_test_results);

	for (uint16_t idx = 0; idx < nbr_of_test_results; idx++)
	{
		printf("Bring-up: '%s' [%s]\n", test_results[idx].test_name, test_results[idx].test_result ? "PASS" : "FAIL");
		if (!test_results[idx].test_result)
		{
			all_passed = false;
		}
	}

	return all_passed;
}
