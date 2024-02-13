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

// Distance Values 
uint32_t distanceVal = 0;
uint32_t startVal = 0;
uint32_t endVal = 0;
uint32_t numDistances = 9;
uint32_t calibrateNeeded = 0;
uint32_t measDistErr = 0;

// Error statuses 
uint32_t errorStatus = 0;
uint32_t busyError = 0;


// TEST Values
uint32_t distanceRegVal = 0;
uint32_t distancePeak0 = 0;
uint32_t distancePeakStrength0 = 0;


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
    if(radarSensor.setDistanceStart(300) != 0)
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

    // Read Detector Result register
    radarSensor.getDistanceReg(distanceRegVal);
// DELETE THIS AFTER TESTING
    Serial.print("Distance Error Register: 0b");
    Serial.println(distanceRegVal, BIN);

    // Check MEASURE_DISTANCE_ERROR for measurement failed
    radarSensor.getDistanceMeasureDistanceError(measDistErr);
    if(measDistErr == 1)
    {
      Serial.println("Measure Distance Error");
    }

    // If CALIBRATION_NEEDED is set, then write RECALIBRATE command
    radarSensor.getDistanceCalibrationNeeded(calibrateNeeded);
    if(calibrateNeeded == 1)
    {
      Serial.println("Calibration Needed - Recalibrating.. ");
      // Calibrate device (write RECALIBRATE command)
      radarSensor.setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
    }

    // Read how many peak distances can be detected 
    
    radarSensor.getDistancePeak0Distance(distancePeak0);
    radarSensor.getDistancePeak0Strength(distancePeakStrength0);

    Serial.print("Peak 0 Distance: ");
    Serial.println(distancePeak0);
    Serial.print("Peak 0 Strength: ");
    Serial.println(distancePeakStrength0);
    // Read PeakX Distance and PeakX Strength registers for the number of distances detected 
    // for(int i = 0; i <= numDistances; i++)
    // {
    //   // Check thru each peak distance detected 
    // }

    // Half a second delay for easier readings 
    delay(500);

    // Empty space for easier readings
    Serial.println();
}
