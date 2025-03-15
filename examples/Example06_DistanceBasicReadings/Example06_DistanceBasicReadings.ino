/*
  Example 6: Distance Basic Readings

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
#include "SparkFun_Qwiic_XM125_Arduino_Library.h"
#include <Arduino.h>

SparkFunXM125Distance radarSensor;

// I2C default address
uint8_t i2cAddress = SFE_XM125_I2C_ADDRESS;

// Presence range in mm used - 300mm to 4000mm (0.3 M to 4 M)
#define MY_XM125_RANGE_START 300
#define MY_XM125_RANGE_END 4000

void setup()
{
    // Start serial
    Serial.begin(115200);

    Serial.println("");
    Serial.println("-------------------------------------------------------");
    Serial.println("XM125 Example 6: Basic Distance Readings");
    Serial.println("-------------------------------------------------------");
    Serial.println("");

    Wire.begin();

    // If begin is successful (0), then start example
    if (radarSensor.begin(i2cAddress, Wire) == false)
    {
        Serial.println("Device failed to setup - Freezing code.");
        while (1)
            ; // Runs forever
    }
    Serial.println("Starting Sensor...");
    Serial.println();
    // Start the sensor with the specified range values
    int32_t setupError = radarSensor.distanceBegin(MY_XM125_RANGE_START, MY_XM125_RANGE_END);
    if (setupError != 0)
    {
        Serial.print("Distance Detection Start Setup Error: ");
        Serial.println(setupError);
    }
    Serial.println();
    Serial.print("Distance Detection Started - range: ");
    Serial.print(MY_XM125_RANGE_START);
    Serial.print("mm to ");
    Serial.print(MY_XM125_RANGE_END);
    Serial.println("mm");
    Serial.println();
    // New-line and 0.5 second delay for easier reading
    Serial.println();
    delay(500);
}

void loop()
{
    uint32_t retCode = radarSensor.distanceDetectorReadingSetup();
    if (retCode != 0)
    {
        Serial.print("Distance Reading Setup Error: ");
        Serial.println(retCode);
    }

    // How many distance values were detected? (0-9)
    uint32_t numDistances = 0;
    radarSensor.getDistanceNumberDistances(numDistances);

    if (numDistances == 0)
        Serial.print(".");
    else
    {
        Serial.println();
        Serial.print("Number of Values Detected: ");
        Serial.println(numDistances);
    }

    uint32_t distance = 0;
    int32_t distanceStrength = 0;
    for (uint32_t i = 0; i < numDistances; i++)
    {
        if (radarSensor.getDistancePeakDistance(i, distance) != ksfTkErrOk)
        {
            Serial.print("Error retrieving Distance Peak ");
            Serial.print(i);
            Serial.println();
            continue;
        }
        Serial.print("   Distance Peak ");
        Serial.print(i);
        Serial.print(": ");
        if (distance < 100)
        {
            Serial.print(distance);
            Serial.print("mm");
        }
        else if (distance < 1000)
        {
            Serial.print(distance * 0.1);
            Serial.print("cm");
        }
        else
        {
            Serial.print(distance * 0.001);
            Serial.print("m");
        }

        if (radarSensor.getDistancePeakStrength(i, distanceStrength) != ksfTkErrOk)
        {
            Serial.print("Error retrieving Distance Peak Strength");
            Serial.print(i);
            Serial.println();
            continue;
        }
        Serial.print("     Distance Peak Strength ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(distanceStrength);
    }

    // delay before next reading
    delay(2500);
}
