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
uint32_t distance; 
uint32_t presenceDetected;

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
    
    int32_t sensorStartError = radarSensor.presenceDetectorStart();
    Serial.println("here");
    if(sensorStartError != 0)
    {
      Serial.println("Sensor Started Successfully");
    }
    else
    {
      Serial.print("Error upon Setup: ");
      Serial.println(sensorStartError);
    }

    // // Set Start to 1000mm
    // if(radarSensor.setPresenceStart(1000) != 0)
    // {
    //   Serial.println("Presence Start Error");
    // }
    // else 
    // {
    //   Serial.println("Presence Start Set: 1000");
    // }

    // delay(500);
    
    // // Set end at 5000mm
    // if(radarSensor.setPresenceEnd(5000) != 0)
    // {
    //   Serial.println("Presence End Error");
    // }
    // else 
    // {
    //   Serial.println("Presence End Set: 5000");
    // }

    // // Apply configuration
    // if(radarSensor.setPresenceCommand(XM125_PRESENCE_APPLY_CONFIGURATION) != 0)
    // {
    //   Serial.println("Presence Command Error");
    // }
    // else 
    // {
    //   Serial.println("Presence Configuration applied to device");
    // }

    // // Wait for configuration to be done
    // if(radarSensor.presenceBusyWait() != 0)
    // {
    //   Serial.println("Device is busy");
    // }

    // // Start detector -- mask 0x02 with Presence Reg command address 
    // if(radarSensor.startPresenceDetector() != 0)
    // {
    //   Serial.println("Presence Detector Start Error");
    // }
    // else 
    // {
    //   Serial.println("Presence Detector Started!");
    // }

    // // Wait for configuration to be done
    // if(radarSensor.presenceBusyWait() != 0)
    // {
    //   Serial.println("Device is busy");
    // }

    Serial.println("Presence Setup Complete");


}

void loop()
{
    // If Presence is detected, then print out distance from device 
    radarSensor.getPresenceDistanceResult(&presenceDetected);
    
    radarSensor.getPresenceDistance(&distance);
    Serial.print("Presence detected at ");
    Serial.print(distance);
    Serial.println("mm");

    if(presenceDetected == 1)
    {
      radarSensor.getPresenceDistance(&distance);
      Serial.print("Presence detected at ");
      Serial.print(distance);
      Serial.println("mm");
    }

    


    delay(100);
}
