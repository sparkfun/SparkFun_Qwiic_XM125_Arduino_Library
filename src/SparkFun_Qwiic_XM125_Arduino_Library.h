#include "sfe_bus.h"
#include "sfeXM125Regs.h"

// Add any definitions hereby


class QwDevXM125
{
    public: 

        // 
        ///////////////////////////////////////////////////////////////////////
        // begin()
        //
        //
        // This method follows the standard startup pattern in SparkFun Arduino
        // libraries.
        //
        //  Parameter   Description
        //  ---------   ----------------------------
        //  wirePort    optional. The Wire port. If not provided, the default port is used
        //  address     optional. I2C Address. If not provided, the default address is used.
        //  retval      true on success, false on startup failure
        //
        // This method is overridden, implementing two versions.
        //
        // Version 1:
        // User skips passing in an I2C object which then defaults to Wire.
        bool begin(uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS)
        {
            // Setup  I2C object and pass into the superclass
            setCommunicationBus(_i2cBus, deviceAddress);

            // Initialize the I2C buss class i.e. setup default Wire port
            _i2cBus.init();

            // Initialize the system - return results
            return this->QwDevXM125::init();
        }

        //Version 2:
        // User passes in an I2C object and an address (optional).
        bool begin(TwoWire &wirePort, uint8_t deviceAddress = SFE_XM125_I2C_ADDRESS)
        {
            // Setup  I2C object and pass into the superclass
            setCommunicationBus(_i2cBus, deviceAddress);

            // Give the I2C port provided by the user to the I2C bus class.
            _i2cBus.init(wirePort, true);

            // Initialize the system - return results
            return this->QwDevXM125::init();
        }

    private: 
        
        // I2C bus class
        sfe_XM125::QwI2C _i2cBus;



}