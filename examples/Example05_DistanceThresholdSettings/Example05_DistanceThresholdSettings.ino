/*
  Example 5: Distance Threshold Settings 
  
  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Distance Reading Mode.
  The sensor is initialized, then the distance amplitude, and strength , fixed 
  amplitude, and sensitivity thresholds are set. 

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

// Distance Variables 
uint32_t distancePeakStrength0 = 0;
uint32_t distancePeak0 = 0;
uint32_t distancePeakStrength1 = 0;
uint32_t distancePeak1 = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 5: Distance Threshold Settings");
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

  // Distance Sensor Setup
    // Reset sensor configuration to reapply configuration registers
    radarSensor.setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);

    radarSensor.distanceBusyWait();

    // Check error and busy bits 
    radarSensor.getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }

    delay(100);
  
    // Set Start register 
    if(radarSensor.setDistanceStart(100) != 0)
    {
      Serial.println("Distance Start Error");
    }
    radarSensor.getDistanceStart(startVal);
    Serial.print("Start Val: ");
    Serial.println(startVal);
    
    delay(100);
    // Set End register 
    if(radarSensor.setDistanceEnd(4000) != 0)
    {
      Serial.println("Distance End Error");
    }
    radarSensor.getDistanceEnd(endVal);
    Serial.print("End Val: ");
    Serial.println(endVal);
    delay(100);

    // Set Distance Threshold Settings - Threshold Sensitivity (range: 0 to 1000)
    radarSensor.setDistanceThresholdSensitivity(100);

    // Set Fixed Amplitude Threshold 
    radarSensor.setDistanceFixedAmpThreshold(100);
    

    // Apply configuration 
    if(radarSensor.setDistanceCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION) != 0)
    {
      // Check for errors
      radarSensor.getDistanceDetectorErrorStatus(errorStatus);
      if(errorStatus != 0)
      {
        Serial.print("Detector status error: ");
        Serial.println(errorStatus);
      }
  
      Serial.println("Configuration application error");
    }

    // Poll detector status until busy bit is cleared
    if(radarSensor.distanceBusyWait() != 0)
    {
      Serial.print("Busy wait error");
    }

    // Check detector status 
    radarSensor.getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
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
    radarSensor.getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }
    
    // Start detector 
    if(radarSensor.setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR) != 0)
    {
      Serial.println("Start detector error");
    }
    
    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(radarSensor.distanceBusyWait() != 0)
    {
      Serial.println("Busy wait error");
    }
    
    // Verify that no error bits are set in the detector status register 
    radarSensor.getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }

    // Check MEASURE_DISTANCE_ERROR for measurement failed
    radarSensor.getDistanceMeasureDistanceError(measDistErr);
    if(measDistErr == 1)
    {
      Serial.println("Measure Distance Error");
    }

    // Recalibrate device if calibration error is triggered 
    radarSensor.getDistanceCalibrationNeeded(calibrateNeeded);
    if(calibrateNeeded == 1)
    {
      Serial.println("Calibration Needed - Recalibrating.. ");
      // Calibrate device (write RECALIBRATE command)
      radarSensor.setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
    }

    // Read PeakX Distance and PeakX Strength registers for the number of distances detected 
    radarSensor.getDistancePeak0Distance(distancePeak0);
    radarSensor.getDistancePeak0Strength(distancePeakStrength0);
    radarSensor.getDistancePeak1Distance(distancePeak1);
    radarSensor.getDistancePeak1Strength(distancePeakStrength1);

    // Read out 2 distances and peaks with threshold settings adjusted
    if (distancePeak0 != 0) 
    {
      Serial.print("Peak0 Distance, Strength: ");
      Serial.print(distancePeak0);
      Serial.print("mm, ");
      Serial.println(distancePeakStrength0);
      Serial.println();
    }
    if (distancePeak1 != 0) 
    {
      Serial.print("Peak1 Distance, Strength: ");
      Serial.print(distancePeak1);
      Serial.print("mm, ");
      Serial.println(distancePeakStrength1);
      Serial.println();
    }

    // Half a second delay for easier readings 
    delay(500);
}
