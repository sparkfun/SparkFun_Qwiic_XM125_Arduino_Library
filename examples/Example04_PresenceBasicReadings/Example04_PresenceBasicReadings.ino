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

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("XM125 Example 4: Basic Presence Readings");
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

    delay(200);



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


    // // Test code below - delete once complete 

    // radarSensor.getPresenceStart(startVal);
    // radarSensor.getPresenceEnd(endVal);
    // Serial.print("Start Val: ");
    // Serial.println(startVal);
    // Serial.print("End Val: ");
    // Serial.println(endVal);

    delay(1000);
    //while(1);
}

void loop()
{
    // Set Start register 
    if(radarSensor.setPresenceStart(1000) != 0)
    {
      Serial.println("Presence Start Error");
    }
    radarSensor.getPresenceStart(startVal);
    Serial.print("Start Val: ");
    Serial.println(startVal);
    
    delay(100);
    // Set End register 
    if(radarSensor.setPresenceEnd(5000) != 0)
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
      Serial.println("Configuration application error");
    }
    
    delay(100);
    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(radarSensor.presenceBusyWait() != 0)
    {
      Serial.println("Busy wait error");
    }
    
    delay(100);
    // Check error bits 
    

    delay(100);
    // Start detector 
    if(radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR) != 0)
    {
      Serial.println("Start detector error");
    }
    
    delay(100);
    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(radarSensor.presenceBusyWait() != 0)
    {
      Serial.println("Busy wait error");
    }
    
    delay(100);
    // Verify that no error bits are set in the detector status register 
    radarSensor.getPresenceDetectorError(errorStatus);
    if((errorStatus & SFE_XM125_PRESENCE_DETECTOR_STATUS_MASK) != 0)
    {
      Serial.println("Detector status error");
    }

    delay(100);
    // Read detector result register and determine 
    radarSensor.getPresenceDetectorPresenceDetected(presenceDetected);
    radarSensor.getPresenceDetectorPresenceStickyDetected(presenceDetectedSticky);
    radarSensor.getPresenceDetectorRegError(regErr);
    
    if((presenceDetected == 1) & (presenceDetectedSticky == 1))
    {
      radarSensor.getPresenceDistance(distance);
      Serial.print("Presence Detected: ");
      Serial.print(distance);
      Serial.println("mm");
    }

    // Reset sensor configuration to reapply configuration registers
    //radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_RESET_MODULE);

    Serial.println("loop completed");

    delay(2000);


    // ***** PREVIOUS CODE BELOW *****  

    // radarSensor.setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
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
