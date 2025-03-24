
/*!
 * @file SparkFun_Qwiic_XM125_Arduino_Library.h
 * @brief Arduino header file for the SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (Qwiic)
 *
 * This file contains the class definitions for the SparkFun Pulsed Coherent Radar Sensor Arduino library,
 * including methods for initialization, distance and presence detection, and configuration.
 * It uses I2C to communicate with the device.
 *
 * @author SparkFun Electronics
 * @date 2024
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

// To make the Arduino machine happy, include the toolkit header before the core implementation for this device

// clang-format off
#include <SparkFun_Toolkit.h>
#include "sfTk/sfDevXM125Core.h"
#include "sfTk/sfDevXM125Distance.h"
#include "sfTk/sfDevXM125Presence.h"

// To support version 1.* API 
#include "sfTk/sfDevXM125DistanceV1.h"
#include "sfTk/sfDevXM125PresenceV1.h"
// clang-format on

#include <Arduino.h>
#include <Wire.h>

/**
 * @class SparkFunXM125Distance
 * @brief Arduino class for the SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (Qwiic) for distance detection.
 *
 * This class provides methods for initializing the sensor, checking connection status, and configuring the device
 * for distance detection using I2C communication.
 */
class SparkFunXM125Distance : public sfDevXM125Distance
{
  public:
    SparkFunXM125Distance()
    {
    }

    /**
     * @brief Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
     *
     * @param deviceAddress Address of the I2C device. Default is SFE_XM125_I2C_ADDRESS.
     * @param wirePort Wire port of the I2C device. Default is Wire.
     * @return True if successful, false otherwise.
     */
    bool begin(uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS, TwoWire &wirePort = Wire)
    {
        // Give the I2C port provided by the user to the I2C bus class.
        _i2cBus.init(wirePort, deviceAddress);

        // Initialize the system - return results
        return this->sfDevXM125Distance::begin(&_i2cBus) == ksfTkErrOk;
    }

    /**
     * @brief Checks if the device is connected.
     *
     * @return True if the sensor is connected, false otherwise.
     */
    bool isConnected()
    {
        return _i2cBus.ping() == ksfTkErrOk;
    }

  private:
    // I2C bus class
    sfTkArdI2C _i2cBus;
};

/**
 * @class SparkFunXM125Presence
 * @brief Arduino class for the SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (Qwiic) for presence detection.
 *
 * This class provides methods for initializing the sensor, checking connection status, and configuring the device
 * for presence detection using I2C communication.
 */
class SparkFunXM125Presence : public sfDevXM125Presence
{
  public:
    SparkFunXM125Presence()
    {
    }

    /**
     * @brief Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
     *
     * @param deviceAddress Address of the I2C device. Default is SFE_XM125_I2C_ADDRESS.
     * @param wirePort Wire port of the I2C device. Default is Wire.
     * @return True if successful, false otherwise.
     */
    bool begin(uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS, TwoWire &wirePort = Wire)
    {
        // Give the I2C port provided by the user to the I2C bus class.
        _i2cBus.init(wirePort, deviceAddress);

        // Initialize the system - return results
        return this->sfDevXM125Presence::begin(&_i2cBus) == ksfTkErrOk;
    }

    /**
     * @brief Checks if the device is connected.
     *
     * @return True if the sensor is connected, false otherwise.
     */
    bool isConnected()
    {
        return _i2cBus.ping() == ksfTkErrOk;
    }

  private:
    // I2C bus class
    sfTkArdI2C _i2cBus;
};

// Version 1 - for backward compatibility
/**
 * @class SparkFunXM125DistanceV1
 * @brief Arduino class for the SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (Qwiic) for distance detection.
 *
 * This class is to provide Version 1 compatibility for the SparkFun Pulsed Coherent Radar Sensor - distance detection.
 * 
 * This class provides methods for initializing the sensor, checking connection status, and configuring the device
 * for distance detection using I2C communication.
 */
class SparkFunXM125DistanceV1 : public sfDevXM125DistanceV1
{
  public:
    SparkFunXM125DistanceV1()
    {
    }

    /**
     * @brief Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
     *
     * @param deviceAddress Address of the I2C device. Default is SFE_XM125_I2C_ADDRESS.
     * @param wirePort Wire port of the I2C device. Default is Wire.
     * @return True if successful, false otherwise.
     */
    bool begin(uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS, TwoWire &wirePort = Wire)
    {
        // Give the I2C port provided by the user to the I2C bus class.
        _i2cBus.init(wirePort, deviceAddress);

        // Initialize the system - return results
        return this->sfDevXM125DistanceV1::begin(&_i2cBus) == ksfTkErrOk;
    }

    /**
     * @brief Checks if the device is connected.
     *
     * @return True if the sensor is connected, false otherwise.
     */
    bool isConnected()
    {
        return _i2cBus.ping() == ksfTkErrOk;
    }

  private:
    // I2C bus class
    sfTkArdI2C _i2cBus;
};

/**
 * @class SparkFunXM125Presence
 * @brief Arduino class for the SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (Qwiic) for presence detection.
 *
 *  This class is to provide Version 1 compatibility for the SparkFun Pulsed Coherent Radar Sensor - presence detection.
 * 
 * This class provides methods for initializing the sensor, checking connection status, and configuring the device
 * for presence detection using I2C communication.
 */
class SparkFunXM125PresenceV1 : public sfDevXM125PresenceV1
{
  public:
    SparkFunXM125PresenceV1()
    {
    }

    /**
     * @brief Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
     *
     * @param deviceAddress Address of the I2C device. Default is SFE_XM125_I2C_ADDRESS.
     * @param wirePort Wire port of the I2C device. Default is Wire.
     * @return True if successful, false otherwise.
     */
    bool begin(uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS, TwoWire &wirePort = Wire)
    {
        // Give the I2C port provided by the user to the I2C bus class.
        _i2cBus.init(wirePort, deviceAddress);

        // Initialize the system - return results
        return this->sfDevXM125PresenceV1::begin(&_i2cBus) == ksfTkErrOk;
    }

    /**
     * @brief Checks if the device is connected.
     *
     * @return True if the sensor is connected, false otherwise.
     */
    bool isConnected()
    {
        return _i2cBus.ping() == ksfTkErrOk;
    }

  private:
    // I2C bus class
    sfTkArdI2C _i2cBus;
};