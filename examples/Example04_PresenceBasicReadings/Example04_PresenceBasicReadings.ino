/*
  Example 4: Presence Basic Readings

  Using the Acconeer XM125 A121 60GHz Pulsed Coherent Radar Sensor.

  This example shows how operate the XM125 when the device is in Presence Reading Mode.
  The sensor is initialized, then the presence values will print out to the terminal.
  For full algorithm for 

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

// Value to fill presence distance (in mm) 
uint32_t distance = 0;
uint32_t presenceDetected = 0;
uint32_t presenceDetectedSticky = 0;
uint32_t errorStatus = 0;
uint32_t regErr = 0;

// DELETE TEST VARIABLES ONCE COMPLETE
uint32_t startVal = 0;
uint32_t endVal = 0;
uint32_t counter = 0;
uint32_t intra = 0;
uint32_t regVal = 0;
uint32_t busyBit = 0;
uint32_t busyError = 0;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 4: Basic Presence Readings");
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


// Test code setup below:
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
    Serial.print("Start Val: ");
    Serial.println(startVal);
    
    delay(100);
    // Set End register 
    if(radarSensor.setPresenceEnd(5200) != 0)
    {
      Serial.println("Presence End Error");
    }
    radarSensor.getPresenceEnd(endVal);
    Serial.print("End Val: ");
    Serial.println(endVal);
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

    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(radarSensor.presenceBusyWait() != 0)
    {
      Serial.print("Busy wait error: ");
      Serial.println(radarSensor.getPresenceRegisterVal(busyError));
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
    //while(1);
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
      Serial.println("Detector status error");
    }


    // Read detector result register and determine 
    radarSensor.getPresenceDetectorPresenceDetected(presenceDetected);
    radarSensor.getPresenceDetectorPresenceStickyDetected(presenceDetectedSticky);
    radarSensor.getPresenceDetectorRegError(regErr);
    radarSensor.getPresenceRegisterVal(regVal);


    // Serial.print("Presence Detected: ");
    // Serial.println(presenceDetected);
    // Serial.print("Presence Detected Sticky: ");
    // Serial.println(presenceDetectedSticky);
    
    if((presenceDetected == 1) | (presenceDetectedSticky == 1))
    {
      radarSensor.getPresenceDistance(distance);
      Serial.print("Presence Detected: ");
      Serial.print(distance);
      Serial.println("mm");
    }

    //radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_RESET_MODULE);

    Serial.println();

    delay(1000);


    // ***** PREVIOUS CODE BELOW *****  

    // radarSensor.setPresenceCommand(XM125_PRESENCE_APPLY_CONFIGURATION);
    // // If Presence is detected, then print out distance from device 
    // radarSensor.presenceBusyWait();
    // radarSensor.getPresenceDetectorPresenceDetected(presenceDetected);
    // Serial.print("Presence Detector flag: ");
    // Serial.println(presenceDetected);

    // radarSensor.getPresenceDistance(distance);
    // Serial.print("Presence detected at ");
    // Serial.print(distance);
    // Serial.println("mm");

    // if(presenceDetected == 1)
    // {
    //   radarSensor.getPresenceDistance(distance);
    //   Serial.print("Presence detected at ");
    //   Serial.print(distance);
    //   Serial.println("mm");
    // }

    // Read out at a slower rate for easier reading (0.5 second delay)
    // delay(500);
}
