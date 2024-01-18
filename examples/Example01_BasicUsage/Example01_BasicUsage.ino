#include "SparkFun_Qwiic_XM125_Arduino_Library.h"

SfeXM125 sensor;

// I2C default address
uint8_t i2cAddress = SFE_XM125_I2C_ADDRESS;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 1: Basic Distance Readings");
    Serial.println("");

    // TODO: Check return value of begin
    sensor.begin();

    // If begin is successful (0), then start example
    if(sensor.begin(i2cAddress, Wire) == 1)
    {
        Serial.println("Begin");
    }
    else // Otherwise, infinite loop
    {
        Serial.println("Device failed to setup - Freezing code.");
        while(1); // Runs forever
    }
}

void loop()
{
    // Request Distance Data from the device 

    delay(100);
}
