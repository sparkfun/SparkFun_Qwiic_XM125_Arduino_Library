/*
  Example 1: Presence Basic Readings

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Presence Reading Mode.
  The sensor is initialized, then the presence distance values will print out
  to the terminal.

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

SparkFunXM125Presence radarSensor;

// I2C default address
uint8_t i2cAddress = SFE_XM125_I2C_ADDRESS;

// Presence distance values
uint32_t distance = 0;

// Error statuses
int32_t errorStatus = 0;
int32_t busyError = 0;
int32_t setupError = 0;
int32_t presValError = 0;
int32_t detectorError = 0;

// Presence range in mm used
#define MY_XM125_RANGE_START 200
#define MY_XM125_RANGE_END 1000
void setup()
{
    // Start serial
    Serial.begin(115200);

    Serial.println("");
    Serial.println("-------------------------------------------------------");
    Serial.println("XM125 Example 1: Basic Presence Readings");
    Serial.println("-------------------------------------------------------");
    Serial.println("");

    Wire.begin();

    // If begin is successful (1), then start example
    bool success = radarSensor.begin(i2cAddress, Wire);
    if (success == false)
    {
        Serial.println("Device failed to setup - Freezing code.");
        while (1)
            ; // Runs forever
    }

    // Start the sensor with default register values
    int32_t setupError = radarSensor.detectorStart(MY_XM125_RANGE_START, MY_XM125_RANGE_END);
    if (setupError != 0)
    {
        Serial.print("Presence Detection Start Setup Error: ");
        Serial.println(setupError);
    }
    Serial.print("Presense Detection Started - range: ");
    Serial.print(MY_XM125_RANGE_START);
    Serial.print("mm to ");
    Serial.print(MY_XM125_RANGE_END);
    Serial.println("mm");
    Serial.println();

    delay(500);
}

void loop()
{
    // Busy wait loop until data is ready
    radarSensor.busyWait();

    // Get the presence distance value and print out if no errors.
    // Note - this returns if Presence is detected now, or since last check (sticky)
    presValError = radarSensor.getDistanceValuemm(distance);

    if (presValError == 0)
    {
        Serial.print("Presence Detected: ");
        // if distance is > 0, presence is detected, else it is not
        if (distance > 0)
        {
            Serial.print("YES  -  Distance: ");
            Serial.print(distance);
            Serial.print("mm, ");
            Serial.print(distance * .1);
            Serial.print("cm, ");
            Serial.print(distance * .001);
            Serial.print("m, ");
            Serial.print(distance * .03937008);
            Serial.println("In");
        }
        else
            Serial.println("NO");
    }
    else
        Serial.println("Error returning presence distance value");

    // Delay 2.5 seconds between readings
    delay(2500);
}
