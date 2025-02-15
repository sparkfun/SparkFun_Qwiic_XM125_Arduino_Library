/**
 * @file sfDevXM125.h
 * @brief Header of the SparkFun Qwiic XM125  Library.
 *
 * This file contains the header declares of the functions for interfacing with
 * the SparkFun Qwiic XM125 distance and presence detector using I2C communication.
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <stdint.h>

#include <sfTk/sfToolkit.h>
// Bus interfaces
#include <sfTk/sfTkII2C.h>

// The I2C address for the device
const uint16_t SFE_XM125_I2C_ADDRESS = 0x52;

class sfDevXM125Core
{
  public:
    /// @brief Initializer
    sfDevXM125Core() : _theBus{nullptr} {};

    /// @brief This function begins the examples/communication.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    virtual sfTkError_t begin(sfTkII2C *theBus = nullptr);

    virtual ~sfDevXM125Core() {};

  protected:
    // our toolkit bus
    sfTkII2C *_theBus;
};
