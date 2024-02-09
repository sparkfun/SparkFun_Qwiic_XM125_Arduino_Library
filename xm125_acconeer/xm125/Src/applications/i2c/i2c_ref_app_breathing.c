// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "acc_definitions_a121.h"
#include "acc_detector_presence.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_rss_a121.h"
#include "acc_version.h"

#include "acc_reg_protocol.h"
#include "i2c_application_system.h"
#include "i2c_ref_app_breathing.h"
#include "ref_app_breathing_reg_protocol.h"

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

typedef struct
{
	acc_sensor_t               *sensor;
	ref_app_breathing_config_t *config;
	ref_app_breathing_handle_t *handle;
	ref_app_breathing_result_t result;
	acc_cal_result_t           sensor_cal_result;
	void                       *buffer;
	uint32_t                   buffer_size;
} ref_app_breathing_resources_t;

static ref_app_breathing_resources_t app_resources            = { 0 };
static uint32_t                      i2c_app_command          = 0U;
static uint32_t                      i2c_app_status           = 0U;
static bool                          ref_app_breathing_active = false;
static bool                          result_ready             = false;
static bool                          result_ready_sticky      = false;
static float                         breathing_rate           = 0.0f;
static ref_app_breathing_app_state_t app_state                = REF_APP_BREATHING_APP_STATE_INIT;
static uint32_t                      breathing_frame_counter  = 0U;
static uint32_t                      breathing_last_tick_ms   = 0U;
static uint32_t                      breathing_frame_rate_mhz = 0U;
static bool                          uart_logs_enabled        = false;

#define UART_LOG_BUFFER_SIZE 100

/**
 * @brief Get the i2c_app_command values
 *
 * The i2c_app_command is cleared during this read
 * The read and clear are protected by a critical section
 *
 * @return The command sent from the host
 */
static uint32_t get_command(void);


/**
 * @brief Execute the command sent from the host
 *
 * @param[in] command The command to execute
 */
static void command_handler(uint32_t command);


/**
 * @brief Set bits in the i2c_app_status
 *
 * The i2c_app_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to set
 */
static void app_status_set_bits(uint32_t bit_mask);


/**
 * @brief Clear bits in the i2c_app_status
 *
 * The i2c_app_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to clear
 */
static void app_status_clr_bits(uint32_t bit_mask);


/**
 * @brief Test bits in the i2c_app_status
 *
 * The i2c_app_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to test
 * @return true if all the bits in bit_mask is set in i2c_app_status
 */
static bool app_status_test_bits(uint32_t bit_mask);


/**
 * @brief Create sensor
 *
 * @param[in] resources Application resources struct
 */
static void create_sensor(ref_app_breathing_resources_t *resources);


/**
 * @brief Calibrate sensor
 *
 * @param[in] resources Application resources struct
 * @return true if successful
 */
static bool calibrate_sensor(ref_app_breathing_resources_t *resources);


/**
 * @brief Apply application config
 *
 * This function will create the ref app breathing and
 * allocate the needed memory
 *
 * @param[in] resources Application resources struct
 */
static void apply_app_config(ref_app_breathing_resources_t *resources);


/**
 * @brief Activate application
 *
 * This function will activate the application
 *
 * @param[in] resources Application resources struct
 * @param[in] enable set to true to enable the application
 * @return true if successful
 */
static bool app_activate(ref_app_breathing_resources_t *resources, bool enable);


/**
 * @brief Get next breathing measurement
 *
 * @param[in] resources Application resources struct
 * @return true if successful
 */
static bool app_get_next(ref_app_breathing_resources_t *resources);


/**
 * @brief Try to set module in low power mode
 */
static void module_low_power(void);


/**
 * @brief Enter sensor hibernation state
 */
static bool enter_hibernate(acc_sensor_t *sensor);


/**
 * @brief Exit sensor hibernation state
 */
static bool exit_hibernate(acc_sensor_t *sensor);


/**
 * @brief Print the ref app breathing result
 *
 * Only available when the UART logs have been enabled with ENABLE_UART_LOGS
 *
 * @param[in] result The ref app breathing result
 */
static void print_breathing_result(ref_app_breathing_result_t *result);


/**
 * @brief UART logging function (can be enabled/disabled by command)
 */
static void uart_log(const char *format, ...);


//
// PUBLIC FUNCTIONS
//


ref_app_breathing_config_t *i2c_ref_app_breathing_get_config(void)
{
	return app_resources.config;
}


bool i2c_ref_app_breathing_command(uint32_t command)
{
	bool status = false;

	/* Make sure we do not have a race for i2c_app_command/i2c_app_status */
	acc_integration_critical_section_enter();

	if (i2c_app_command == 0U)
	{
		/* Set Ready PIN to LOW while processing the command */
		i2c_application_system_set_ready_pin(false);

		/* Set status BUSY bit */
		i2c_app_status |=  REF_APP_BREATHING_REG_APP_STATUS_FIELD_BUSY_MASK;
		i2c_app_command = command;
		status          = true;
	}

	acc_integration_critical_section_exit();
	return status;
}


uint32_t i2c_ref_app_breathing_get_status(void)
{
	/* Make sure we do not have a race for i2c_app_status */
	acc_integration_critical_section_enter();

	uint32_t status = i2c_app_status;

	acc_integration_critical_section_exit();

	return status;
}


uint32_t i2c_ref_app_breathing_get_result(void)
{
	uint32_t value = 0;

	/* Make sure we do not have a race for results */
	acc_integration_critical_section_enter();

	if (result_ready)
	{
		value |= REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_RESULT_READY_MASK;
	}

	if (result_ready_sticky)
	{
		value              |= REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_RESULT_READY_STICKY_MASK;
		result_ready_sticky = false;
	}

	/* Add temperature */
	uint32_t temp = (uint32_t)app_resources.result.presence_result.processing_result.temperature;

	temp = (temp << REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_TEMPERATURE_POS) &
	       REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_TEMPERATURE_MASK;
	value |= temp;

	acc_integration_critical_section_exit();

	return value;
}


float i2c_ref_app_breathing_get_breathing_rate(void)
{
	/* Make sure we do not have a race for breathing_rate */
	acc_integration_critical_section_enter();

	float value = breathing_rate;

	acc_integration_critical_section_exit();

	return value;
}


ref_app_breathing_app_state_t i2c_ref_app_breathing_get_app_state(void)
{
	/* Make sure we do not have a race for app_state */
	acc_integration_critical_section_enter();

	ref_app_breathing_app_state_t state = app_state;

	acc_integration_critical_section_exit();

	return state;
}


uint32_t i2c_ref_app_breathing_get_counter(void)
{
	/* Make sure we do not have a race for breathing_frame_counter */
	acc_integration_critical_section_enter();

	uint32_t counter = breathing_frame_counter;

	acc_integration_critical_section_exit();

	return counter;
}


uint32_t i2c_ref_app_breathing_get_frame_rate_mhz(void)
{
	acc_integration_critical_section_enter();
	uint32_t value = breathing_frame_rate_mhz;
	acc_integration_critical_section_exit();
	return value;
}


//
// MAIN
//


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	bool setup_status   = true;
	bool get_next_frame = false;

	printf("I2C Ref App Breathing\n");
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (acc_rss_hal_register(hal))
	{
		app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_RSS_REGISTER_OK_MASK);
	}
	else
	{
		printf("ERROR: acc_rss_hal_register() failed\n\n");
		app_status_set_bits(
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK | REF_APP_BREATHING_REG_APP_STATUS_FIELD_RSS_REGISTER_ERROR_MASK);
		setup_status = false;
	}

	if (setup_status)
	{
		app_resources.config = ref_app_breathing_config_create();
		if (app_resources.config != NULL)
		{
			/* Config is created, write default values to registers */
			ref_app_breathing_reg_protocol_write_default();

			app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_CREATE_OK_MASK);
		}
		else
		{
			app_status_set_bits(
				REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK |
				REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_CREATE_ERROR_MASK);
			printf("ERROR: ref_app_breathing_config_create() failed\n\n");
			setup_status = false;
		}
	}

	/* Turn the sensor on */
	acc_hal_integration_sensor_supply_on(SENSOR_ID);

	if (setup_status)
	{
		/* Create sensor */
		create_sensor(&app_resources);
	}

	i2c_application_system_init();

	/* Setup i2c register protocol */
	ref_app_breathing_reg_protocol_setup();

	while (true)
	{
		/* Handle Ref App Breathing */
		if (ref_app_breathing_active &&
		    get_next_frame &&
		    !app_status_test_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK))
		{
			get_next_frame = false;
			if (app_get_next(&app_resources))
			{
				print_breathing_result(&app_resources.result);
			}
			else
			{
				printf("ERROR: Could not get next result\n");
				app_activate(&app_resources, false);
				app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK);
			}
		}

		/* Handle Commands */
		uint32_t command = get_command();

		if (command == 0)
		{
			/* Try to set module in low power mode */
			module_low_power();

			/* Test if a periodic wakeup was the wakeup reason */
			get_next_frame = i2c_application_is_periodic_wakeup();
			if (get_next_frame)
			{
				acc_integration_critical_section_enter();
				uint32_t time_ms = acc_integration_get_time();
				uint32_t diff_ms = time_ms - breathing_last_tick_ms;
				breathing_last_tick_ms   = time_ms;
				breathing_frame_rate_mhz = (diff_ms > 0) ? (1000000 / diff_ms) : 0;

				acc_integration_critical_section_exit();
			}

			continue;
		}

		/* Special command, always handle reset module command, even if error has occured */
		if (command == REF_APP_BREATHING_REG_COMMAND_ENUM_RESET_MODULE)
		{
			/* Reset system */
			i2c_application_system_reset();
			continue;
		}

		if (app_status_test_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK))
		{
			/* Do not process commands after error state */
			continue;
		}

		/* Handle command */
		command_handler(command);

		/* Command handler done, clear busy bit */
		app_status_clr_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_BUSY_MASK);

		/* Set Ready PIN to HIGH when command processing is done */
		i2c_application_system_set_ready_pin(true);
	}

	return EXIT_FAILURE;
}


//
// PRIVATE HELPER FUNCTIONS
//


static uint32_t get_command(void)
{
	/* Make sure we do not have a race for i2c_app_command */
	acc_integration_critical_section_enter();

	uint32_t command = i2c_app_command;

	i2c_app_command = 0U;

	acc_integration_critical_section_exit();

	return command;
}


static void command_handler(uint32_t command)
{
	switch (command)
	{
		case REF_APP_BREATHING_REG_COMMAND_ENUM_APPLY_CONFIGURATION:
			if (!app_status_test_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_OK_MASK))
			{
				// Apply configuration
				apply_app_config(&app_resources);
			}

			break;
		case REF_APP_BREATHING_REG_COMMAND_ENUM_START_APP:
			if (!ref_app_breathing_active)
			{
				if (app_activate(&app_resources, true))
				{
					ref_app_breathing_active = true;
				}
				else
				{
					printf("ERROR: Could not start application\n");
					app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK);
				}
			}

			break;
		case REF_APP_BREATHING_REG_COMMAND_ENUM_STOP_APP:
			if (ref_app_breathing_active)
			{
				if (app_activate(&app_resources, false))
				{
					ref_app_breathing_active = false;
				}
				else
				{
					printf("ERROR: Could not stop application\n");
					app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK);
				}
			}

			break;
		case REF_APP_BREATHING_REG_COMMAND_ENUM_ENABLE_UART_LOGS:
			uart_logs_enabled = true;
			uart_log("UART logs enabled\n");
			break;
		case REF_APP_BREATHING_REG_COMMAND_ENUM_DISABLE_UART_LOGS:
			uart_log("UART logs disabled\n");
			uart_logs_enabled = false;
			break;
		case REF_APP_BREATHING_REG_COMMAND_ENUM_LOG_CONFIGURATION:
			// Print the configuration
			acc_detector_presence_config_log(app_resources.config->presence_config);
			break;
		default:
			printf("ERROR: Unknown command: %" PRIu32 "", command);
			break;
	}
}


static void app_status_set_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_app_status */
	acc_integration_critical_section_enter();

	i2c_app_status |= bit_mask;
	uint32_t temp_app_status = i2c_app_status;

	acc_integration_critical_section_exit();

	uart_log("App Status = 0x%" PRIx32 "\n", temp_app_status);
}


static void app_status_clr_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_app_status */
	acc_integration_critical_section_enter();

	i2c_app_status &= ~bit_mask;
	uint32_t temp_app_status = i2c_app_status;

	acc_integration_critical_section_exit();

	uart_log("App Status = 0x%" PRIx32 "\n", temp_app_status);
}


static bool app_status_test_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_app_status */
	acc_integration_critical_section_enter();

	bool status = (i2c_app_status & bit_mask) == bit_mask;

	acc_integration_critical_section_exit();

	return status;
}


static void create_sensor(ref_app_breathing_resources_t *resources)
{
	acc_hal_integration_sensor_enable(SENSOR_ID);

	resources->sensor = acc_sensor_create(SENSOR_ID);

	acc_hal_integration_sensor_disable(SENSOR_ID);

	if (resources->sensor != NULL)
	{
		app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CREATE_OK_MASK);
	}
	else
	{
		app_status_set_bits(
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK |
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK
			);
		printf("ERROR: acc_sensor_create() failed\n");
	}
}


static bool calibrate_sensor(ref_app_breathing_resources_t *resources)
{
	app_status_clr_bits(
		REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK |
		REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK
		);

	bool           status              = false;
	bool           cal_complete        = false;
	const uint16_t calibration_retries = 1U;

	// Random disturbances may cause the calibration to fail. At failure, retry at least once.
	for (uint16_t i = 0; !status && (i <= calibration_retries); i++)
	{
		acc_hal_integration_sensor_enable(SENSOR_ID);

		do
		{
			status = acc_sensor_calibrate(resources->sensor,
			                              &cal_complete,
			                              &resources->sensor_cal_result,
			                              resources->buffer,
			                              resources->buffer_size);
			if (status && !cal_complete)
			{
				status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
			}
		} while (status && !cal_complete);

		/* Reset sensor after calibration by disabling it */
		acc_hal_integration_sensor_disable(SENSOR_ID);
	}

	if (status)
	{
		app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK);
	}
	else
	{
		app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK);
		printf("ERROR: acc_sensor_calibrate() failed\n");
	}

	return status;
}


static void apply_app_config(ref_app_breathing_resources_t *resources)
{
	bool status = true;

	acc_detector_presence_config_frame_rate_app_driven_set(resources->config->presence_config, true);

	/* Always use DEEP_SLEEP for inter_frame_idle_state to save power */
	acc_detector_presence_config_inter_frame_idle_state_set(resources->config->presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);

	resources->handle = ref_app_breathing_create(resources->config);
	if (resources->handle != NULL)
	{
		app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_CREATE_OK_MASK);
	}
	else
	{
		app_status_set_bits(
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK |
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_CREATE_ERROR_MASK
			);
		printf("ERROR: ref_app_breathing_create() failed\n");
		status = false;
	}

	if (status)
	{
		if (ref_app_breathing_get_buffer_size(resources->handle, &(resources->buffer_size)))
		{
			app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_BUFFER_OK_MASK);
		}
		else
		{
			app_status_set_bits(
				REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK |
				REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_BUFFER_ERROR_MASK
				);
			printf("ERROR: ref_app_breathing_get_buffer_size() failed\n");
			status = false;
		}
	}

	if (status)
	{
		resources->buffer = acc_integration_mem_alloc(resources->buffer_size);
		if (resources->buffer != NULL)
		{
			app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_BUFFER_OK_MASK);
		}
		else
		{
			app_status_set_bits(
				REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK |
				REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK
				);
			printf("ERROR: sensor buffer allocation failed\n");
			status = false;
		}
	}

	if (status)
	{
		status = calibrate_sensor(&app_resources);
	}

	if (status)
	{
		app_status_set_bits(REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_OK_MASK);
	}
	else
	{
		printf("ERROR: apply application config failed\n");
		app_status_set_bits(
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK |
			REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK
			);
	}
}


static bool app_activate(ref_app_breathing_resources_t *resources, bool enable)
{
	bool status = true;

	if (enable)
	{
		uart_log("Start ref app breathing\n");
		acc_hal_integration_sensor_enable(SENSOR_ID);

		if (!ref_app_breathing_prepare(resources->handle,
		                               resources->config,
		                               resources->sensor,
		                               &resources->sensor_cal_result,
		                               resources->buffer,
		                               resources->buffer_size))
		{
			status = false;
			printf("ERROR: ref_app_breathing_prepare() failed\n");
		}

		if (status)
		{
			status = enter_hibernate(resources->sensor);
		}

		if (status)
		{
			uint32_t sleep_time_ms =
				(uint32_t)(1000.0f / acc_detector_presence_config_frame_rate_get(resources->config->presence_config));
			i2c_application_set_periodic_wakeup(sleep_time_ms);
		}
	}
	else
	{
		uart_log("Stop ref app breathing\n");

		status = exit_hibernate(resources->sensor);

		acc_hal_integration_sensor_disable(SENSOR_ID);
		i2c_application_set_periodic_wakeup(0);
	}

	return status;
}


static bool app_get_next(ref_app_breathing_resources_t *resources)
{
	bool status = true;

	/* Exit from hibernation */
	status = exit_hibernate(resources->sensor);

	if (status)
	{
		if (!acc_sensor_measure(resources->sensor))
		{
			printf("ERROR: acc_sensor_measure() failed\n");
			status = false;
		}
	}

	if (status)
	{
		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("ERROR: Sensor interrupt timeout\n");
			status = false;
		}
	}

	if (status)
	{
		if (!acc_sensor_read(resources->sensor,
		                     resources->buffer,
		                     resources->buffer_size))
		{
			printf("ERROR: acc_sensor_read() failed\n");
			status = false;
		}
	}

	if (status)
	{
		/* Enter hibernation */
		status = enter_hibernate(resources->sensor);
	}

	if (status)
	{
		if (!ref_app_breathing_process(resources->handle,
		                               resources->buffer,
		                               &resources->result))
		{
			printf("ERROR: ref_app_breathing_process() failed\n");
			status = false;
		}
	}

	if (status)
	{
		/* Make sure we do not have a race for breathing_frame_counter */
		acc_integration_critical_section_enter();

		breathing_frame_counter++;

		acc_integration_critical_section_exit();

		if (resources->result.presence_result.processing_result.calibration_needed)
		{
			uart_log("Recalibration\n");

			status = app_activate(resources, false);

			if (status && calibrate_sensor(resources))
			{
				status = app_activate(resources, true);
			}
		}
	}

	if (status)
	{
		/* Make sure we do not have a race for results */
		acc_integration_critical_section_enter();

		app_state    = resources->result.app_state;
		result_ready = resources->result.result_ready;

		if (result_ready)
		{
			breathing_rate      = resources->result.breathing_rate;
			result_ready_sticky = true;
		}
		else if (app_state != REF_APP_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE)
		{
			breathing_rate = 0.0f;
		}

		acc_integration_critical_section_exit();
	}

	return status;
}


static void module_low_power(void)
{
	if (i2c_application_system_test_wakeup_pin())
	{
		i2c_application_system_wait_for_interrupt();
	}
	else
	{
		/* Set ready pin LOW, we are about to power down */
		i2c_application_system_set_ready_pin(false);

		uart_log("Enter low power state\n");
		i2c_application_enter_low_power_state();
		uart_log("Exit low power state\n");
	}

	if (i2c_application_system_test_wakeup_pin())
	{
		/* Set ready pin HIGH, we are ready for a command */
		i2c_application_system_set_ready_pin(true);
	}
}


static bool enter_hibernate(acc_sensor_t *sensor)
{
	bool status = true;

	if (!acc_sensor_hibernate_on(sensor))
	{
		printf("ERROR: acc_sensor_hibernate_on failed\n");
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
		printf("ERROR: acc_sensor_hibernate_off failed\n");
		status = false;
	}

	return status;
}


static void print_breathing_result(ref_app_breathing_result_t *result)
{
	if (result->result_ready)
	{
		uart_log("Breaths: %" PRIu16 " bpm\n", (uint16_t)result->breathing_rate);
	}
}


static void uart_log(const char *format, ...)
{
	char log_buffer[UART_LOG_BUFFER_SIZE];

	va_list ap;

	va_start(ap, format);

	if (uart_logs_enabled)
	{
		int ret = vsnprintf(log_buffer, UART_LOG_BUFFER_SIZE, format, ap);

		if (ret >= UART_LOG_BUFFER_SIZE)
		{
			log_buffer[UART_LOG_BUFFER_SIZE - 4] = '.';
			log_buffer[UART_LOG_BUFFER_SIZE - 3] = '.';
			log_buffer[UART_LOG_BUFFER_SIZE - 2] = '.';
			log_buffer[UART_LOG_BUFFER_SIZE - 1] = 0;
		}

		printf("%s", log_buffer);
	}

	va_end(ap);
}
