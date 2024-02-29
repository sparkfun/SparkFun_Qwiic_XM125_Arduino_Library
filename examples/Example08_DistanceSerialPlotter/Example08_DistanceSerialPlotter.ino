/*
  Example 8: Distance Serial Plotter

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example prints out the distance values of the 0 and 1 distance 
  channels to the serial plotter tool in Arduino. 

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

// Setup Variables 
uint32_t startVal = 0;
uint32_t endVal = 0;
uint32_t numDistances = 9;
uint32_t calibrateNeeded = 0;
uint32_t measDistErr = 0;

// Error statuses 
uint32_t errorStatus = 0;
uint32_t distanceSetupError = 0;

// Distance Variables 
uint32_t distancePeak0 = 0;
uint32_t distancePeak1 = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 8: Distance Serial Plotter");
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

    int32_t setupError = radarSensor.distanceBegin();
    if(setupError != 0)
    {
      Serial.print("Distance Detection Start Setup Error: ");
      Serial.println(setupError); 
    }

    // New-line and 0.5 second delay for easier reading
    Serial.println();
    delay(500);
}

void loop()
{
    distanceSetupError = radarSensor.distanceDetectorReadingSetup();
    if(distanceSetupError != 0)
    {
      Serial.print("Distance Reading Setup Error: ");
      Serial.println(distanceSetupError);
    }

    // Read PeakX Distance and PeakX Strength registers for the number of distances detected 
    radarSensor.getDistancePeak0Distance(distancePeak0);
    radarSensor.getDistancePeak1Distance(distancePeak1);

    // If a peak distance was detected, then read out the distance and strength
    if (distancePeak0 != 0) 
    {
      Serial.println(distancePeak0);
    }
    if (distancePeak1 != 0) 
    {
      Serial.println(distancePeak1);
    }

    // Half a second delay for easier readings 
    delay(500);

}
