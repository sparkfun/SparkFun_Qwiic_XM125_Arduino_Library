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
#include "i2c_presence_detector.h"
#include "presence_reg_protocol.h"

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (1000U)

typedef struct
{
	acc_sensor_t                     *sensor;
	acc_detector_presence_config_t   *config;
	acc_detector_presence_handle_t   *handle;
	acc_detector_presence_metadata_t metadata;
	acc_detector_presence_result_t   result;
	acc_cal_result_t                 sensor_cal_result;
	void                             *buffer;
	uint32_t                         buffer_size;
} presence_detector_resources_t;

static presence_detector_resources_t detector_resources       = { 0 };
static uint32_t                      i2c_detector_command     = 0U;
static uint32_t                      i2c_detector_status      = 0U;
static bool                          presence_detector_active = false;
static bool                          presence_detected        = false;
static bool                          presence_detected_sticky = false;
static bool                          presence_detection_gpio  = false;
static bool                          presence_detector_error  = false;
static float                         presence_distance        = 0.0f;
static uint32_t                      presence_frame_counter   = 0U;
static uint32_t                      presence_last_tick_ms    = 0U;
static uint32_t                      presence_frame_rate_mhz  = 0U;
static float                         intra_presence_score     = 0.0f;
static float                         inter_presence_score     = 0.0f;
static bool                          uart_logs_enabed         = false;

#define UART_LOG_BUFFER_SIZE 100

/**
 * @brief Get the i2c_detector_command values
 *
 * The i2c_detector_command is cleared during this read
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
 * @brief Set bits in the i2c_detector_status
 *
 * The i2c_detector_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to set
 */
static void detector_status_set_bits(uint32_t bit_mask);


/**
 * @brief Clear bits in the i2c_detector_status
 *
 * The i2c_detector_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to clear
 */
static void detector_status_clr_bits(uint32_t bit_mask);


/**
 * @brief Test bits in the i2c_detector_status
 *
 * The i2c_detector_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to test
 * @return true if all the bits in bit_mask is set in i2c_detector_status
 */
static bool detector_status_test_bits(uint32_t bit_mask);


/**
 * @brief Create sensor
 *
 * @param[in] resources presence detector resources struct
 */
static void create_sensor(presence_detector_resources_t *resources);


/**
 * @brief Calibrate sensor
 *
 * @param[in] resources presence detector resources struct
 * @return true if successful
 */
static bool calibrate_sensor(presence_detector_resources_t *resources);


/**
 * @brief Apply detector config
 *
 * This function will create the presence detector and
 * allocate the needed memory
 *
 * @param[in] resources presence detector resources struct
 */
static void apply_detector_config(presence_detector_resources_t *resources);


/**
 * @brief Activate detector
 *
 * This function will activate the detector
 *
 * @param[in] resources presence detector resources struct
 * @param[in] enable set to true to enable the detector
 * @return true if successful
 */
static bool detector_activate(presence_detector_resources_t *resources, bool enable);


/**
 * @brief Get next presence measurement
 *
 * @param[in] resources presence detector resources struct
 * @return true if successful
 */
static bool detector_get_next(presence_detector_resources_t *resources);


/**
 * @brief Handle detector GPIO output
 */
static void detector_gpio_output(void);


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
 * @brief Print the presence detector result
 *
 * Only available when the UART logs have been enabled with ENABLE_UART_LOGS
 *
 * @param[in] result The presence detector result
 */
static void print_presence_result(acc_detector_presence_result_t *result);


/**
 * @brief UART logging function (can be enabled/disabled by command)
 */
static void uart_log(const char *format, ...);


//
// PUBLIC FUNCTIONS
//


acc_detector_presence_config_t *i2c_presence_detector_get_config(void)
{
	return detector_resources.config;
}


bool i2c_presence_detector_command(uint32_t command)
{
	bool status = false;

	/* Make sure we do not have a race for i2c_detector_command/i2c_detector_status */
	acc_integration_critical_section_enter();

	if (i2c_detector_command == 0U)
	{
		/* Set Ready PIN to LOW while processing the command */
		i2c_application_system_set_ready_pin(false);

		/* Set status BUSY bit */
		i2c_detector_status |=  PRESENCE_REG_DETECTOR_STATUS_FIELD_BUSY_MASK;
		i2c_detector_command = command;
		status               = true;
	}

	acc_integration_critical_section_exit();
	return status;
}


uint32_t i2c_presence_detector_get_status(void)
{
	/* Make sure we do not have a race for i2c_detector_status */
	acc_integration_critical_section_enter();

	uint32_t status = i2c_detector_status;

	acc_integration_critical_section_exit();

	return status;
}


uint32_t i2c_presence_detector_get_result(void)
{
	uint32_t value = 0;

	/* Make sure we do not have a race for results */
	acc_integration_critical_section_enter();

	if (presence_detected)
	{
		value |= PRESENCE_REG_PRESENCE_RESULT_FIELD_PRESENCE_DETECTED_MASK;
	}

	if (presence_detected_sticky)
	{
		value |= PRESENCE_REG_PRESENCE_RESULT_FIELD_PRESENCE_DETECTED_STICKY_MASK;

		presence_detected_sticky = false;
	}

	if (presence_detector_error)
	{
		value |= PRESENCE_REG_PRESENCE_RESULT_FIELD_DETECTOR_ERROR_MASK;
	}

	/* Add temperature */
	uint32_t temp = (uint32_t)detector_resources.result.processing_result.temperature;

	temp   = (temp << PRESENCE_REG_PRESENCE_RESULT_FIELD_TEMPERATURE_POS) & PRESENCE_REG_PRESENCE_RESULT_FIELD_TEMPERATURE_MASK;
	value |= temp;

	acc_integration_critical_section_exit();

	return value;
}


float i2c_presence_detector_get_distance(void)
{
	/* Make sure we do not have a race for presence_distance */
	acc_integration_critical_section_enter();

	float value = presence_distance;

	acc_integration_critical_section_exit();

	return value;
}


float i2c_presence_detector_get_intra_presence_score(void)
{
	/* Make sure we do not have a race for intra_presence_score */
	acc_integration_critical_section_enter();

	float value = intra_presence_score;

	acc_integration_critical_section_exit();

	return value;
}


float i2c_presence_detector_get_inter_presence_score(void)
{
	/* Make sure we do not have a race for inter_presence_score */
	acc_integration_critical_section_enter();

	float value = inter_presence_score;

	acc_integration_critical_section_exit();

	return value;
}


uint32_t i2c_presence_detector_get_counter(void)
{
	/* Make sure we do not have a race for presence_frame_counter */
	acc_integration_critical_section_enter();

	uint32_t counter = presence_frame_counter;

	acc_integration_critical_section_exit();

	return counter;
}


void i2c_presence_detector_presence_detected_gpio(bool enable)
{
	/* Make sure we do not have a race for presence_detection_gpio */
	acc_integration_critical_section_enter();

	presence_detection_gpio = enable;

	acc_integration_critical_section_exit();
}


bool i2c_presence_detector_get_presence_detected_gpio(void)
{
	/* Make sure we do not have a race for presence_detection_gpio */
	acc_integration_critical_section_enter();

	bool value = presence_detection_gpio;

	acc_integration_critical_section_exit();

	return value;
}


uint32_t i2c_presence_detector_get_presence_frame_rate_mhz(void)
{
	acc_integration_critical_section_enter();
	uint32_t value = presence_frame_rate_mhz;
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

	printf("I2C Presence Detector\n");
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (acc_rss_hal_register(hal))
	{
		detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_OK_MASK);
	}
	else
	{
		printf("ERROR: acc_rss_hal_register() failed\n\n");
		detector_status_set_bits(
			PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK | PRESENCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_ERROR_MASK);
		setup_status = false;
	}

	if (setup_status)
	{
		detector_resources.config = acc_detector_presence_config_create();
		if (detector_resources.config != NULL)
		{
			/* Config is created, write default values to registers */
			presence_reg_protocol_write_default();

			detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_OK_MASK);
		}
		else
		{
			detector_status_set_bits(
				PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK | PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_ERROR_MASK);
			printf("ERROR: acc_detector_presence_config_create() failed\n\n");
			setup_status = false;
		}
	}

	/* Turn the sensor on */
	acc_hal_integration_sensor_supply_on(SENSOR_ID);

	if (setup_status)
	{
		/* Create sensor */
		create_sensor(&detector_resources);
	}

	i2c_application_system_init();

	/* Setup i2c register protocol */
	presence_reg_protocol_setup();

	while (true)
	{
		/* Handle Presence Detector */
		if (presence_detector_active &&
		    get_next_frame &&
		    !detector_status_test_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK))
		{
			get_next_frame = false;
			if (detector_get_next(&detector_resources))
			{
				print_presence_result(&detector_resources.result);
			}
			else
			{
				printf("ERROR: Could not get next result\n");
				detector_activate(&detector_resources, false);
				presence_detector_error = true;
				detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK);
			}
		}

		/* Handle Generic GPIO output */
		detector_gpio_output();

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
				uint32_t diff_ms = time_ms - presence_last_tick_ms;
				presence_last_tick_ms   = time_ms;
				presence_frame_rate_mhz = (diff_ms > 0) ? (1000000 / diff_ms) : 0;

				acc_integration_critical_section_exit();
			}

			continue;
		}

		/* Special command, always handle reset module command, even if error has occured */
		if (command == PRESENCE_REG_COMMAND_ENUM_RESET_MODULE)
		{
			/* Reset system */
			i2c_application_system_reset();
			continue;
		}

		if (detector_status_test_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK))
		{
			/* Do not process commands after error state */
			continue;
		}

		/* Handle command */
		command_handler(command);

		/* Command handler done, clear busy bit */
		detector_status_clr_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_BUSY_MASK);

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
	/* Make sure we do not have a race for i2c_detector_command */
	acc_integration_critical_section_enter();

	uint32_t command = i2c_detector_command;

	i2c_detector_command = 0U;

	acc_integration_critical_section_exit();

	return command;
}


static void command_handler(uint32_t command)
{
	switch (command)
	{
		case PRESENCE_REG_COMMAND_ENUM_APPLY_CONFIGURATION:
			if (!detector_status_test_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_OK_MASK))
			{
				// Apply configuration
				apply_detector_config(&detector_resources);
			}

			break;
		case PRESENCE_REG_COMMAND_ENUM_START_DETECTOR:
			if (!presence_detector_active)
			{
				if (detector_activate(&detector_resources, true))
				{
					presence_detector_active = true;
				}
				else
				{
					printf("ERROR: Could not start detector\n");
					presence_detector_error = true;
					detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK);
				}
			}

			break;
		case PRESENCE_REG_COMMAND_ENUM_STOP_DETECTOR:
			if (presence_detector_active)
			{
				if (detector_activate(&detector_resources, false))
				{
					presence_detector_active = false;
				}
				else
				{
					printf("ERROR: Could not stop detector\n");
					presence_detector_error = true;
					detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK);
				}
			}

			break;
		case PRESENCE_REG_COMMAND_ENUM_ENABLE_UART_LOGS:
			uart_logs_enabed = true;
			uart_log("UART logs enabled\n");
			break;
		case PRESENCE_REG_COMMAND_ENUM_DISABLE_UART_LOGS:
			uart_log("UART logs disabled\n");
			uart_logs_enabed = false;
			break;
		case PRESENCE_REG_COMMAND_ENUM_LOG_CONFIGURATION:
			// Print the configuration
			acc_detector_presence_config_log(detector_resources.config);
			break;
		default:
			printf("ERROR: Unknown command: %" PRIu32 "", command);
			break;
	}
}


static void detector_status_set_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_detector_status */
	acc_integration_critical_section_enter();

	i2c_detector_status |= bit_mask;
	uint32_t temp_detector_status = i2c_detector_status;

	acc_integration_critical_section_exit();

	uart_log("Detector Status = 0x%" PRIx32 "\n", temp_detector_status);
}


static void detector_status_clr_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_detector_status */
	acc_integration_critical_section_enter();

	i2c_detector_status &= ~bit_mask;
	uint32_t temp_detector_status = i2c_detector_status;

	acc_integration_critical_section_exit();

	uart_log("Detector Status = 0x%" PRIx32 "\n", temp_detector_status);
}


static bool detector_status_test_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_detector_status */
	acc_integration_critical_section_enter();

	bool status = (i2c_detector_status & bit_mask) == bit_mask;

	acc_integration_critical_section_exit();

	return status;
}


static void create_sensor(presence_detector_resources_t *resources)
{
	acc_hal_integration_sensor_enable(SENSOR_ID);

	resources->sensor = acc_sensor_create(SENSOR_ID);

	acc_hal_integration_sensor_disable(SENSOR_ID);

	if (resources->sensor != NULL)
	{
		detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_OK_MASK);
	}
	else
	{
		detector_status_set_bits(
			PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK |
			PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK
			);
		printf("ERROR: acc_sensor_create() failed\n");
	}
}


static bool calibrate_sensor(presence_detector_resources_t *resources)
{
	acc_hal_integration_sensor_enable(SENSOR_ID);

	detector_status_clr_bits(
		PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK |
		PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK
		);

	bool status;
	bool cal_complete = false;

	do
	{
		status = acc_sensor_calibrate(resources->sensor,
		                              &cal_complete,
		                              &resources->sensor_cal_result,
		                              resources->buffer,
		                              resources->buffer_size);
	} while (status && !cal_complete && acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS));

	if (status)
	{
		detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK);
	}
	else
	{
		detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK);
		printf("ERROR: acc_sensor_calibrate() failed\n");
	}

	/* Reset sensor after calibration by disabling it */
	acc_hal_integration_sensor_disable(SENSOR_ID);

	return status;
}


static void apply_detector_config(presence_detector_resources_t *resources)
{
	bool status = true;

	acc_detector_presence_config_frame_rate_app_driven_set(resources->config, true);

	/* Always use DEEP_SLEEP for inter_frame_idle_state to save power */
	acc_detector_presence_config_inter_frame_idle_state_set(resources->config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);

	resources->handle = acc_detector_presence_create(resources->config, &(resources->metadata));
	if (resources->handle != NULL)
	{
		detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_OK_MASK);
	}
	else
	{
		detector_status_set_bits(
			PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK |
			PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_ERROR_MASK
			);
		printf("ERROR: acc_detector_presence_create() failed\n");
		status = false;
	}

	if (status)
	{
		if (acc_detector_presence_get_buffer_size(resources->handle, &(resources->buffer_size)))
		{
			detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_OK_MASK);
		}
		else
		{
			detector_status_set_bits(
				PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK |
				PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_ERROR_MASK
				);
			printf("ERROR: acc_detector_presence_get_buffer_size() failed\n");
			status = false;
		}
	}

	if (status)
	{
		resources->buffer = acc_integration_mem_alloc(resources->buffer_size);
		if (resources->buffer != NULL)
		{
			detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_OK_MASK);
		}
		else
		{
			detector_status_set_bits(
				PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK |
				PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK
				);
			printf("ERROR: sensor buffer allocation failed\n");
			status = false;
		}
	}

	if (status)
	{
		status = calibrate_sensor(&detector_resources);
	}

	if (status)
	{
		detector_status_set_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_OK_MASK);
	}
	else
	{
		printf("ERROR: apply detector config failed\n");
		detector_status_set_bits(
			PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK |
			PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK
			);
	}
}


static bool detector_activate(presence_detector_resources_t *resources, bool enable)
{
	bool status = true;

	if (enable)
	{
		uart_log("Start presence detector\n");
		acc_hal_integration_sensor_enable(SENSOR_ID);

		if (!acc_detector_presence_prepare(resources->handle,
		                                   resources->config,
		                                   resources->sensor,
		                                   &resources->sensor_cal_result,
		                                   resources->buffer,
		                                   resources->buffer_size))
		{
			status = false;
			printf("ERROR: acc_detector_presence_prepare() failed\n");
		}

		if (status)
		{
			status = enter_hibernate(resources->sensor);
		}

		if (status)
		{
			uint32_t sleep_time_ms = (uint32_t)(1000.0f / acc_detector_presence_config_frame_rate_get(resources->config));
			i2c_application_set_periodic_wakeup(sleep_time_ms);
		}
	}
	else
	{
		uart_log("Stop presence detector\n");

		status = exit_hibernate(resources->sensor);

		acc_hal_integration_sensor_disable(SENSOR_ID);
		i2c_application_set_periodic_wakeup(0);
	}

	return status;
}


static bool detector_get_next(presence_detector_resources_t *resources)
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
		if (!acc_detector_presence_process(resources->handle,
		                                   resources->buffer,
		                                   &resources->result))
		{
			printf("ERROR: acc_detector_presence_process() failed\n");
			status = false;
		}
	}

	if (status)
	{
		/* Make sure we do not have a race for presence_frame_counter */
		acc_integration_critical_section_enter();

		presence_frame_counter++;

		acc_integration_critical_section_exit();

		if (resources->result.processing_result.calibration_needed)
		{
			uart_log("Recalibration\n");

			status = detector_activate(resources, false);

			if (status && calibrate_sensor(resources))
			{
				status = detector_activate(resources, true);
			}
		}
	}

	if (status)
	{
		/* Make sure we do not have a race for results */
		acc_integration_critical_section_enter();

		/* Copy result if a presence was detected */
		if (resources->result.presence_detected)
		{
			presence_detected        = true;
			presence_detected_sticky = true;
			presence_distance        = resources->result.presence_distance;
			intra_presence_score     = resources->result.intra_presence_score;
			inter_presence_score     = resources->result.inter_presence_score;
		}
		else
		{
			presence_detected = false;
		}

		acc_integration_critical_section_exit();
	}

	return status;
}


static void detector_gpio_output(void)
{
	/* Make sure we do not have a race for presence_detection_gpio */
	acc_integration_critical_section_enter();
	bool detection_gpio = presence_detection_gpio;
	acc_integration_critical_section_exit();

	if (detection_gpio)
	{
		/* Setup generic gpio as output */
		i2c_application_system_setup_generic_gpio_pin(true);

		if (presence_detector_active)
		{
			/* Drive high signal if presence is detected */
			i2c_application_system_set_generic_gpio_pin(presence_detected);
		}
		else
		{
			/* Drive low signal on generic gpio */
			i2c_application_system_set_generic_gpio_pin(false);
		}
	}
	else
	{
		/* Disable generic gpio */
		i2c_application_system_setup_generic_gpio_pin(false);
	}
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


static void print_presence_result(acc_detector_presence_result_t *result)
{
	uart_log("Presence detected: %s ", result->presence_detected ? "True" : "False");
	if (result->presence_detected)
	{
		uint32_t distance_mm = (uint32_t)(1000 * result->presence_distance);
		uart_log("(%" PRIu32 "mm)\n", distance_mm);
	}
	else
	{
		uart_log("\n");
	}
}


static void uart_log(const char *format, ...)
{
	char log_buffer[UART_LOG_BUFFER_SIZE];

	va_list ap;

	va_start(ap, format);

	if (uart_logs_enabed)
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
