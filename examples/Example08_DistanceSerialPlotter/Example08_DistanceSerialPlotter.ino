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

// Distance Variables 
uint32_t distancePeak0 = 0;

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

    // Extra line and 1 second delay for easier readings
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
    delay(100);

    // Read Peak0 Distance register for the distance detected
    radarSensor.getDistancePeak0Distance(distancePeak0);

    // Read out 2 distances and peaks with threshold settings adjusted
    if (distancePeak0 != 0) 
    {
      Serial.println(distancePeak0);
    }

    // Half a second delay for easier readings 
    delay(500);
}
