// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef I2C_APPLICATION_SYSTEM_H_
#define I2C_APPLICATION_SYSTEM_H_

#include <stdbool.h>
#include <stdint.h>


/**
 * @brief Init the system
 *
 * This function is called by the i2c application
 * during initialization.
 */
void i2c_application_system_init(void);


/**
 * @brief Reset the system
 *
 * This function is called by the i2c application when
 * the system should be restarted.
 */
void i2c_application_system_reset(void);


/**
 * @brief Wait for interrupt to occur
 */
void i2c_application_system_wait_for_interrupt(void);


/**
 * @brief Check if wakeup pin is high
 *
 * This function is used to test if the system should power down
 *
 * @return true if wakeup pin is high
 */
bool i2c_application_system_test_wakeup_pin(void);


/**
 * @brief Set the ready pin state
 *
 * This function is used to set the state of the ready pin
 *
 * @param[in] enable pin is set to high level when enable is true
 */
void i2c_application_system_set_ready_pin(bool enable);


/**
 * @brief Setup the generic gpio pin
 *
 * This function is used to setup the generic gpio pin
 *
 * @param[in] enable pin is set to output driver when enable is true
 */
void i2c_application_system_setup_generic_gpio_pin(bool enable);


/**
 * @brief Set the generic gpio pin output state
 *
 * This function is used to set the output state of the generic gpio pin
 *
 * @param[in] enable pin output is set to high level when enable is true
 */
void i2c_application_system_set_generic_gpio_pin(bool enable);


/**
 * @brief Make the MCU enter its low power state
 *
 * This function is called by the i2c application when
 * the MCU should power down.
 */
void i2c_application_enter_low_power_state(void);


/**
 * @brief Set up a periodic timer used to wake up the system from sleep
 *
 * If the time_msec is set to zero the periodic wakeup will be disabled.
 *
 * @param period_ms Period time in milliseconds
 */
void i2c_application_set_periodic_wakeup(uint32_t period_ms);


/**
 * @brief Test if a periodic wake up has occurred
 *
 * @return true if a periodic wake up has occurred
 */
bool i2c_application_is_periodic_wakeup(void);


#endif
