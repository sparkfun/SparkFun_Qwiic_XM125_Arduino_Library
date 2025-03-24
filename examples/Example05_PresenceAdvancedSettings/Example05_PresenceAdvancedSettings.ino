/*
  Example 5: Presence Advanced Settings

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Presence Reading Mode.
  The sensor is initialized, then the presence distance values will print out
  to the terminal just as they do in example one. If you wish to change the settings
  of the device, do so before applying the configuration.

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
uint32_t presenceDetected = 0;
uint32_t presenceDetectedSticky = 0;
uint32_t startVal = 0;
uint32_t endVal = 0;
uint32_t gpioUsage = 0;
uint32_t beginReading = 300;
uint32_t endReading = 7000;

// Error statuses
uint32_t errorStatus = 0;
uint32_t busyError = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 5: Presence Advanced Settings");
    Serial.println("");

    Wire.begin();

    // If begin is successful (1), then start example
    int startErr = radarSensor.begin(i2cAddress, Wire);
    if (startErr == 1)
    {
        Serial.println("Begin");
    }
    else // Otherwise, infinite loop
    {
        Serial.print("Start Error Code: ");
        Serial.println(startErr);
        Serial.println("Device failed to setup - Freezing code.");
        while (1)
            ; // Runs forever
    }

    delay(200);

    // *** Presence Sensor Setup ***
    // Reset sensor configuration to reapply configuration registers
    radarSensor.setCommand(SFE_XM125_PRESENCE_RESET_MODULE);

    // Check error and busy bits
    radarSensor.getDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        Serial.print("Detector status error: ");
        Serial.println(errorStatus);
    }

    delay(100);

    // Set Start register
    if (radarSensor.setStart(beginReading) != 0)
    {
        Serial.println("Presence Start Error");
    }
    radarSensor.getStart(startVal);
    Serial.print("Start Val: ");
    Serial.println(startVal);

    delay(100);
    // Set End register
    if (radarSensor.setEnd(endReading) != 0)
    {
        Serial.println("Presence End Error");
    }
    radarSensor.getEnd(endVal);
    Serial.print("End Val: ");
    Serial.println(endVal);
    delay(100);

    // Apply configuration
    if (radarSensor.setCommand(SFE_XM125_PRESENCE_APPLY_CONFIGURATION) != 0)
    {
        // Check for errors
        radarSensor.getDetectorErrorStatus(errorStatus);
        if (errorStatus != 0)
        {
            Serial.print("Detector status error: ");
            Serial.println(errorStatus);
        }

        Serial.println("Configuration application error");
    }

    // Poll detector status until busy bit is cleared
    if (radarSensor.busyWait() != 0)
    {
        Serial.print("Busy wait error");
    }

    // Check detector status
    radarSensor.getDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        Serial.print("Detector status error: ");
        Serial.println(errorStatus);
    }

    Serial.println();

    delay(1000);
}

void loop()
{
    // Check error bits
    radarSensor.getDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        Serial.print("Detector status error: ");
        Serial.println(errorStatus);
    }

    // Start detector
    if (radarSensor.setCommand(SFE_XM125_PRESENCE_START_DETECTOR) != 0)
    {
        Serial.println("Start detector error");
    }

    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if (radarSensor.busyWait() != 0)
    {
        Serial.println("Busy wait error");
    }

    // Verify that no error bits are set in the detector status register
    radarSensor.getDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        Serial.print("Detector status error: ");
        Serial.println(errorStatus);
    }

    // Read detector result register and determine detection status
    radarSensor.getDetectorPresenceDetected(presenceDetected);
    radarSensor.getDetectorPresenceStickyDetected(presenceDetectedSticky);

    if ((presenceDetected == 1) | (presenceDetectedSticky == 1))
    {
        radarSensor.getDistance(distance);
        Serial.print("Presence Detected: ");
        Serial.print(distance);
        Serial.println("mm");
    }

    // Delay 1 second between readings
    delay(1000);
}
