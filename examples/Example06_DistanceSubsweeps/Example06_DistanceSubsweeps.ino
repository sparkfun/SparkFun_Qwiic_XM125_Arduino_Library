/*
  Example 6: Distance Subsweeps - Power Consumption Optimization

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Distance Reading Mode.
  The sensor is initialized, then the measurement range is split into multiple subsweeeps to 
  allow for optimization of power consumption and signal quality. A shorter/longer profile can
  be change byt setting the max_profile parameter, and the step length can be limited using the 
  max_step_length parameter. Setting the high signal quality can result in a better SNR, but 
  setting it low in this application reduces power consumption and measurement time. 

  By: Madison Chodikov
  SparkFun Electronics
  Date: 2024/1/22
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Hardware Connections:
  QWIIC --> QWIIC

  Serial.print it out at 115200 baud to serial monitor.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/ - Qwiic XM125 Breakout
*/
#include <Arduino.h>
#include "SparkFun_Qwiic_XM125_Arduino_Library.h"

SfeXM125 radarSensor;

// I2C default address
uint8_t i2cAddress = SFE_XM125_I2C_ADDRESS;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 6: Distance Subsweeps - Power Consumption Optimization");
    Serial.println("");

    Wire.begin();

    // If begin is successful (0), then start example
    if(radarSensor.begin(i2cAddress, Wire) == 1)
    {
        Serial.println("Begin");
    }
    else // Otherwise, infinite loop
    {
        Serial.println("Device failed to setup - Freezing code.");
        while(1); // Runs forever
    }

    // Select a shorter profile at the start of the measurement
    radarSensor.setDistanceMaxProfile(XM125_DISTANCE_PROFILE1);

    // Choose a longer step length 
    radarSensor.setDistanceMaxStepLength(5);

    // Reduce signal quality for lower HWAAS, SNR to decrease power consumption
    radarSensor.setDistanceSignalQuality(10000);
}

void loop()
{
    // Request Distance Data from the device 

    delay(100);
}
