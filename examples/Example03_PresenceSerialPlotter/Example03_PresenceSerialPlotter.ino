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

  // Presence Sensor Setup
    // Reset sensor configuration to reapply configuration registers
    radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_RESET_MODULE);

    // Check error and busy bits 
    radarSensor.getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }

    delay(100);
  
    // Set Start register 
    if(radarSensor.setPresenceStart(300) != 0)
    {
      Serial.println("Presence Start Error");
    }
    radarSensor.getPresenceStart(startVal);
    
    delay(100);
    // Set End register 
    if(radarSensor.setPresenceEnd(7000) != 0)
    {
      Serial.println("Presence End Error");
    }
    radarSensor.getPresenceEnd(endVal);
    delay(100);

    // Apply configuration 
    if(radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_APPLY_CONFIGURATION) != 0)
    {
      // Check for errors
      radarSensor.getPresenceDetectorErrorStatus(errorStatus);
      if(errorStatus != 0)
      {
        Serial.print("Detector status error: ");
        Serial.println(errorStatus);
      }
  
      Serial.println("Configuration application error");
    }

    // Poll detector status until busy bit is cleared
    if(radarSensor.presenceBusyWait() != 0)
    {
      Serial.print("Busy wait error");
    }

    // Check detector status 
    radarSensor.getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }

    Serial.println();

  // Original code below: 
    // // Default start = 1000; Default stop = 5000
    // int32_t sensorStartError = radarSensor.presenceDetectorStart();
    // if(sensorStartError != 0)
    // {
    //   Serial.println("Sensor Started Successfully");
    // }
    // else
    // {
    //   Serial.println("Sensor not initialized correctly - Freezing code.");
    //   while(1); // Runs forever 
    // }
    
    delay(1000);
}

void loop()
{
  
    // Check error bits 
    radarSensor.getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }
    
    
    // Start detector 
    if(radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR) != 0)
    {
      Serial.println("Start detector error");
    }
    
    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(radarSensor.presenceBusyWait() != 0)
    {
      Serial.println("Busy wait error");
    }
    
    // Verify that no error bits are set in the detector status register 
    radarSensor.getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      Serial.print("Detector status error: ");
      Serial.println(errorStatus);
    }

    // Read detector result register and determine detection status
    radarSensor.getPresenceDetectorPresenceDetected(presenceDetected);
    radarSensor.getPresenceDetectorPresenceStickyDetected(presenceDetectedSticky);
    
    if((presenceDetected == 1) | (presenceDetectedSticky == 1))
    {
      radarSensor.getPresenceDistance(distance);
      Serial.println((distance/10));
    }

    // Delay 1 second between readings
    delay(1000);
}
