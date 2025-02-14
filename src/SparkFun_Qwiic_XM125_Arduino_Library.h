
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
#include <SparkFun_Toolkit.h>
#include "sfTk/sfDevXM125.h"

#include <Arduino.h>
#include <Wire.h>

class SparkFunXM125 : public sfDevXM125
{
    public: 

    SparkFunXM125()
        {
        }

        /// @brief  Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
        /// @param address Address of the I2C device.
        /// @param wirePort Wire port of the I2C device.
        /// @return True if successful, false otherwise.
        bool begin(uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS, TwoWire &wirePort = Wire)
        {
            // Give the I2C port provided by the user to the I2C bus class.
            _i2cBus.init(wirePort, deviceAddress);

            // Initialize the system - return results
            return this->sfDevXM125::begin(&_i2cBus) == 0;
        }

    private: 
        
        // I2C bus class
        sfTkArdI2C _i2cBus;

};

// for backward compatibility -  
class SfeXM125 : public SparkFunXM125
{  };
