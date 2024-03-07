// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef I2C_PRESENCE_DETECTOR_H_
#define I2C_PRESENCE_DETECTOR_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_detector_presence.h"


/**
 * @brief Get presence detector configuration handle
 *
 * @return presence detector configuration handle
 */
acc_detector_presence_config_t *i2c_presence_detector_get_config(void);


/**
 * @brief Send command to be executed to i2c presence detector
 *
 * @param[in] command The command to be executed
 * @return true if successful
 */
bool i2c_presence_detector_command(uint32_t command);


/**
 * @brief Get presence detector status
 *
 * @return status from presence detector
 */
uint32_t i2c_presence_detector_get_status(void);


/**
 * @brief Get presence detector result
 *
 * @return result from presence detector
 */
uint32_t i2c_presence_detector_get_result(void);


/**
 * @brief Get presence detector distance
 *
 * @return The distance of the detected presence
 */
float i2c_presence_detector_get_distance(void);


/**
 * @brief Get intra presence score
 *
 * @return A measure of the amount of fast motion detected.
 */
float i2c_presence_detector_get_intra_presence_score(void);


/**
 * @brief Get inter presence score
 *
 * @return A measure of the amount of slow motion detected.
 */
float i2c_presence_detector_get_inter_presence_score(void);


/**
 * @brief Get presence detector measure counter
 *
 * The counter will be increased every time the presence detector completes a measure
 *
 * @return measure counter from presence detector
 */
uint32_t i2c_presence_detector_get_counter(void);


/**
 * @brief Enable/Disable gpio output for presence detected
 *
 * @param[in] enable Enable gpio output for presence detected if set to true
 */
void i2c_presence_detector_presence_detected_gpio(bool enable);


/**
 * @brief Get gpio output for presence detected state
 *
 * @return true if gpio output for presence detected is enabled
 */
bool i2c_presence_detector_get_presence_detected_gpio(void);


/**
 * @brief Get the actual frame rate for the current presence detector configuration
 *
 * @return The frame rate in mHz
 */
uint32_t i2c_presence_detector_get_presence_frame_rate_mhz(void);


#endif
