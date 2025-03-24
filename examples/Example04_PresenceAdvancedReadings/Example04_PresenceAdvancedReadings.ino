/*
  Example 4: Presence Advanced Readings

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Presence Reading Mode.
  The sensor is initialized, then the presence distance, intra-presence, and
  inter-presence values will be printed to the terminal.

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
uint32_t intraScore = 0;
uint32_t interScore = 0;

// Error statuses
uint32_t errorStatus = 0;
uint32_t busyError = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 4: Presence Advanced Readings");
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

    // Start the sensor with default register values
    int32_t setupError = radarSensor.detectorStart();
    if (setupError != 0)
    {
        Serial.print("Presence Detection Start Setup Error: ");
        Serial.println(setupError);
    }

    // New line and delay for easier reading
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

    if ((presenceDetected == 1) || (presenceDetectedSticky == 1))
    {
        radarSensor.getDistance(distance);
        Serial.print("Presence Detected: ");
        Serial.print(distance);
        Serial.println("mm");

        radarSensor.getIntraPresenceScore(intraScore);
        radarSensor.getInterPresenceScore(interScore);

        Serial.print("Intra-Presence Score: ");
        Serial.println(intraScore);
        Serial.print("Inter-Presence Score: ");
        Serial.println(interScore);
    }

    // Delay 0.5 second between readings
    delay(500);
}
