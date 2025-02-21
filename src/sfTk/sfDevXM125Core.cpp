/**
 * @file sfDevXM125Core.cpp
 * @brief Implementation of the SparkFun Qwiic XM125  Library - Core object
 *
 * This is the core for object that is used to control/setup/connect to the XM125 device.
 * Additional functionality - via templates - add added to this core to support the
 * various applications supported by the XM125 device.
 *
 *
 *
 * The file includes the object lifecycle methods, and Toolkit bus instance data.
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#include "sfDevXM125Core.h"

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Core::begin(sfTkII2C *theBus)
{
    if (theBus == nullptr)
        return ksfTkErrBusNotInit;

    // Check if the provided address is valid
    if (theBus->address() != SFE_XM125_I2C_ADDRESS)
        return ksfTkErrFail;

    // Sets communication bus
    _theBus = theBus;

    // set the byte order to BIG Endian - the sensor works with Big E
    _theBus->setByteOrder(sfTkByteOrder::BigEndian);

    // return the value of ping
    return theBus->ping();
}
