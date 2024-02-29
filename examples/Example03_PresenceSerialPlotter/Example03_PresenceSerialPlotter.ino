/*
  Example 3: Presence Serial Plotter

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Presence Reading Mode.
  The sensor is initialized, then the presence values will print out to the terminal and
  the serial monitor.

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

// Presence distance values 
uint32_t distance = 0;
uint32_t presenceDetected = 0;
uint32_t presenceDetectedSticky = 0;
uint32_t startVal = 0;
uint32_t endVal = 0;

// Error statuses
uint32_t errorStatus = 0;
uint32_t busyError = 0;
uint32_t presValError = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 3: Serial Plotter Presence Readings");
    Serial.println("");

    Wire.begin();

    // If begin is successful (0), then start example
    int startErr = radarSensor.begin(i2cAddress, Wire);
    if(startErr == 1)
    {
        Serial.println("Begin");
    }
    else // Otherwise, infinite loop
    {
        Serial.print("Start Error Code: ");
        Serial.println(startErr);
        Serial.println("Device failed to setup - Freezing code.");
        while(1); // Runs forever
    }

    delay(200);

    // Start the sensor with default register values
    int32_t setupError = radarSensor.presenceDetectorStart();
    if(setupError != 0)
    {
      Serial.print("Presence Detection Start Setup Error: ");
      Serial.println(setupError); 
    }

    // New line and delay for easier reading
    Serial.println();
    delay(500);
}

void loop()
{
    // Busy wait loop until data is ready
    radarSensor.presenceBusyWait();

    // Get the presence distance value and print out if no errors
    presValError = radarSensor.getPresenceDistanceValuemm(distance);

    if(presValError == 0)
    {
      radarSensor.getPresenceDistance(distance);
      Serial.println(distance);
    }

    // Delay 0.5 seconds between readings
    delay(500);
}
