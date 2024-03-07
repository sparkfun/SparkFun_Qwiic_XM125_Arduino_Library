#include "sfeQwiicXM125.h"
#include <Arduino.h>
#include <Wire.h>

class SfeXM125 : public QwDevXM125
{
    public: 

        SfeXM125()
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
            return this->QwDevXM125::begin(&_i2cBus) == 0;
        }

    private: 
        
        // I2C bus class
        sfeTkArdI2C _i2cBus;
        //QwDevXM125::QwI2C _i2cBus; // original code
};