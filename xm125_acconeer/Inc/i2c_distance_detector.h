// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef I2C_DISTANCE_DETECTOR_H_
#define I2C_DISTANCE_DETECTOR_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_detector_distance.h"

/**
 * @brief Get distance detector configuration handle
 *
 * @return distance detector configuration handle
 */
acc_detector_distance_config_t *i2c_distance_detector_get_config(void);


/**
 * @brief Send command to be executed to i2c distance detector
 *
 * @param[in] command The command to be executed
 * @return true if successful
 */
bool i2c_distance_detector_command(uint32_t command);


/**
 * @brief Get distance detector status
 *
 * @return status from distance detector
 */
uint32_t i2c_distance_detector_get_status(void);


/**
 * @brief Get distance detector result
 *
 * @return result from distance detector
 */
uint32_t i2c_distance_detector_get_result(void);


/**
 * @brief Get distance detector measure counter
 *
 * The counter will be increased every time the distance detector completes a measure
 *
 * @return measure counter from distance detector
 */
uint32_t i2c_distance_detector_get_counter(void);


/**
 * @brief Get the peak distance for a specific peak id
 *
 * @param[in] peak_id The id of the peak
 * @return The distance to the peak
 */
float i2c_distance_detector_get_peak_distance(uint16_t peak_id);


/**
 * @brief Get the peak strength for a specific peak id
 *
 * @param[in] peak_id The id of the peak
 * @return The strength of the peak
 */
float i2c_distance_detector_get_peak_strength(uint16_t peak_id);


/**
 * @brief Enable/Disable measure on wake up
 *
 * @param[in] enable Enable measure on wake up if set to true
 */
void i2c_distance_detector_measure_on_wakeup(bool enable);


/**
 * @brief Get measure on wake up state
 *
 * @return true if measure on wake up is enabled
 */
bool i2c_distance_detector_get_measure_on_wakeup(void);


#endif
