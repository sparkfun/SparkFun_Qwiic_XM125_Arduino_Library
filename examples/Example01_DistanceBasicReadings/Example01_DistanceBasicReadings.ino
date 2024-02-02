/*
  Example 1: Distance Basic Readings

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Distance Reading Mode.
  The sensor is initialized, then the distance values will print out to the terminal in 
  mm. 

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

uint32_t distanceVal = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 1: Basic Distance Readings");
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

    // Default start = 1000; Default stop = 5000
    if(radarSensor.distanceBegin() != 0)
    {
      Serial.println("Sensor started successfully");
    }
    else
    {
      Serial.println("Sensor not initialized correctly - Freezing code.");
      while(1);
    }


    // Test code below - delete once complete 
    uint32_t startVal = 0;
    uint32_t endVal = 0;
    radarSensor.getDistanceStart(startVal);
    radarSensor.getDistanceEnd(endVal);
    Serial.print("Start Val: ");
    Serial.println(startVal);
    Serial.print("End Val: ");
    Serial.println(endVal);

    delay(1000);


}

void loop()
{
    // Request Distance Data from the device
    radarSensor.setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR);
    radarSensor.distanceBusyWait();
    radarSensor.getDistancePeak0Distance(distanceVal);

    if(distanceVal != 0)
    {
      Serial.print("Peak Distance Found: ");
      Serial.print(distanceVal);
      Serial.println(" mm");
    }

    delay(100);
}
