/*
  Example 9: Distance Advanced Settings

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

// Setup Variables
uint32_t startVal = 0;
uint32_t endVal = 0;
uint32_t numDistances = 9;
uint32_t calibrateNeeded = 0;
uint32_t measDistErr = 0;
uint32_t beginReading = 300;
uint32_t endReading = 7000;

// Error statuses
uint32_t errorStatus = 0;

// Distance Variables
int32_t distancePeakStrength = 0;
uint32_t distancePeak = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 9: Basic Advanced Settings");
    Serial.println("");

    Wire.begin();

    // If begin is successful (0), then start example
    if (radarSensor.begin(i2cAddress, Wire) == 1)
    {
        Serial.println("Begin");
    }
    else // Otherwise, infinite loop
    {
        Serial.println("Device failed to setup - Freezing code.");
        while (1)
            ; // Runs forever
    }

    // Distance Sensor Setup
    // Reset sensor configuration to reapply configuration registers
    radarSensor.setCommand(SFE_XM125_DISTANCE_RESET_MODULE);

    radarSensor.busyWait();

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
        Serial.println("Distance Start Error");
    }
    radarSensor.getStart(startVal);
    Serial.print("Start Val: ");
    Serial.println(startVal);

    delay(100);
    // Set End register
    if (radarSensor.setEnd(endReading) != 0)
    {
        Serial.println("Distance End Error");
    }
    radarSensor.getEnd(endVal);
    Serial.print("End Val: ");
    Serial.println(endVal);
    delay(100);

    // Apply configuration
    if (radarSensor.setCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION) != 0)
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

// Handy helpful output function

void outputResults(uint sample, uint32_t distance, int32_t strength)
{
    if (distance == 0)
        return;
    Serial.print("Peak");
    Serial.print(sample);
    Serial.print("Distance, Strength: ");

    Serial.print(distance);
    Serial.print("mm, ");
    Serial.println(strength);
    Serial.println();
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
    if (radarSensor.setCommand(SFE_XM125_DISTANCE_START_DETECTOR) != 0)
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

    // Check MEASURE_DISTANCE_ERROR for measurement failed
    radarSensor.getMeasureDistanceError(measDistErr);
    if (measDistErr == 1)
    {
        Serial.println("Measure Distance Error");
    }

    // Recalibrate device if calibration error is triggered
    radarSensor.getCalibrationNeeded(calibrateNeeded);
    if (calibrateNeeded == 1)
    {
        Serial.println("Calibration Needed - Recalibrating.. ");
        // Calibrate device (write RECALIBRATE command)
        radarSensor.setCommand(SFE_XM125_DISTANCE_RECALIBRATE);
    }

    // Read PeakX Distance and PeakX Strength registers for the number of distances detected
    radarSensor.getPeak0Distance(distancePeak);
    radarSensor.getPeak0Strength(distancePeakStrength);
    outputResults(0, distancePeak, distancePeakStrength);

    radarSensor.getPeak1Distance(distancePeak);
    radarSensor.getPeak1Strength(distancePeakStrength);
    outputResults(1, distancePeak, distancePeakStrength);

    radarSensor.getPeak2Distance(distancePeak);
    radarSensor.getPeak2Strength(distancePeakStrength);
    outputResults(2, distancePeak, distancePeakStrength);

    radarSensor.getPeak3Distance(distancePeak);
    radarSensor.getPeak3Strength(distancePeakStrength);
    outputResults(3, distancePeak, distancePeakStrength);

    radarSensor.getPeak4Distance(distancePeak);
    radarSensor.getPeak4Strength(distancePeakStrength);
    outputResults(4, distancePeak, distancePeakStrength);

    radarSensor.getPeak5Distance(distancePeak);
    radarSensor.getPeak5Strength(distancePeakStrength);
    outputResults(5, distancePeak, distancePeakStrength);

    radarSensor.getPeak6Distance(distancePeak);
    radarSensor.getPeak6Strength(distancePeakStrength);
    outputResults(6, distancePeak, distancePeakStrength);

    radarSensor.getPeak7Distance(distancePeak);
    radarSensor.getPeak7Strength(distancePeakStrength);
    outputResults(7, distancePeak, distancePeakStrength);

    radarSensor.getPeak8Distance(distancePeak);
    radarSensor.getPeak8Strength(distancePeakStrength);
    outputResults(8, distancePeak, distancePeakStrength);

    radarSensor.getPeak9Distance(distancePeak);
    radarSensor.getPeak9Strength(distancePeakStrength);
    outputResults(9, distancePeak, distancePeakStrength);

    // Half a second delay for easier readings
    delay(500);
}
