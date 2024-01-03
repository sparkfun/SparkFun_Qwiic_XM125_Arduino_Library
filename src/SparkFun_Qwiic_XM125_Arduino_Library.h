#pragma once

// TODO: Add includes as needed (e.g. #include <Wire.h>, #include <SPI.h>)
#include "Arduino.h"
#include "xm125/Src/applications/i2c/i2c_distance_detector.c"
#include "xm125/Src/applications/i2c/i2c_presence_detector.c"


// TODO: Add constants

// TODO: Implement class
class XM125
{
    public:
        /// @brief TODO
        XM125();

        /// @brief This function begins the examples/communication.
        /// @return Error code (0 no error)
        bool begin();

        // --------------------- I2C Application System functions ---------------------
        /// @brief This function starts the Acconeer main() function to Initialize
        ///  the device correctly
        /// @return Error code (0 no error)
        int32_t acconeerBegin();
        
        /// @brief Disable interrupts
        /// @return Error code (0 no error)
        int32_t disableInterrupts(); // disable_interrupts

        /// @brief Enable interrupts
        /// @return Error code (0 no error) 
        int32_t enableInterrupts(); // enable_interrupts

        /// @brief Prepare power down state
        /// @return Error code (0 no error)
        int32_t preparePowerDown(); // prepare_power_down

        /// @brief Resume power down state
        /// @return Error code (0 no error)
        int32_t resumePowerDown(); // resume_power_down

        /// @brief Get RTC ticks based on current RTC time
        /// @return Error code (0 no error)
        int32_t getRTCTick(); // get_rtc_tick

        /// @brief Convert RTC ticks to RTC time 
        /// @param tick rtc ticks in ms
        /// @param time RTC time
        /// @return Error code (0 no error)
        int32_t rtcTickToTime(uint32_t tick, RTC_TimeTypeDef *time); // rtc_tick_to_time

        /// @brief Function for setting the next wakeup time from the 
        ///  RTC interrupt
        /// @return Error code (0 no error)
        int32_t setRTCWakeupTime();

        /// @brief Wait for I2C interface to be idle
        /// @return Error code (0 no error)
        int32_t waitForI2CIdle(); // wait_for_i2c_idle

        /// @brief Helper function to prepare transmit of register data
        /// @return Error code (0 no error)
        int32_t prepareRegisterData(); // prepare_register_data

        /// @brief Initializes the STM32 I2C communication 
        /// @return Error code (0 no error)
        int32_t initI2CApplicationSystem(); // i2c_application_system_init

        /// @brief Wait for I2C to idle, then reset the system
        /// @return Error code (0 no error)
        int32_t resetI2CApplicationSystem(); // i2c_application_system_reset

        /// @brief 
        /// @return Error code (0 no error)
        int32_t i2cApplicationSystemWaitForInterrupt(); // i2c_application_system_wait_for_interrupt

        /// @brief 
        /// @return Error code (0 no error)
        int32_t i2cApplicationSystemTestWakeupPin(); // i2c_application_system_test_wakeup_pin
        
        /// @brief Function to enable the Reset pin state
        /// @param en 0 disable, 1 enable
        /// @return Error code (0 no error)
        int32_t setI2CApplicationSystemReadyPin(bool en); // i2c_application_system_set_ready_pin
        
        /// @brief Function to set GPIO0 to pull or no pull
        /// @param en 0 disable, 1 enable
        /// @return Error code (0 no error)
        int32_t configureI2CApplicationGPIO0Pin(bool en); // i2c_application_system_setup_generic_gpio_pin
        
        /// @brief Function to enable the GPIO0 state 
        /// @param en 0 disable, 1 enable
        /// @return Error code (0 no error)
        int32_t setI2CApplicationGPIO0Pin(bool en); // i2c_application_system_set_generic_gpio_pin
 
        /// @brief 
        /// @return Error code (0 no error)
        int32_t enterI2CApplicationLowerPowerState(); // i2c_application_enter_low_power_state

        /// @brief 
        /// @return Error code (0 no error)
        int32_t setI2CApplicationPeriodicWakeup(); // i2c_application_set_periodic_wakeup

        /// @brief Returns an error code for the I2C Handler
        /// @return Error code (0 no error)
        int32_t errorI2CCallback(); // HAL_I2C_ErrorCallback

        // --------------------- I2C Disance Detector Functions ---------------------
        /// @brief 
        /// @return Error code (0 no error)
        int32_t getI2CDistanceDetectorConfig(acc_detector_distance_config_t *config);

        /// @brief 
        /// @return Error code (0 no error)
        int32_t distanceDetectorCommand(uint32_t *cmd);

        /// @brief 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorStatus(uint32_t *status);

        /// @brief 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorResult(uint32_t *result);

        /// @brief 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorCounter(uint32_t *count);

        /// @brief 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorPeakDistance(float *distance);

        /// @brief 
        /// @return Error code (0 no error) 
        int32_t getDistanceDetectorPeakStrength(float *peak, uint16_t id);

        /// @brief 
        /// @return Error code (0 no error) 
        int32_t setDistanceDetectorMeasureOnWakeup(bool *en);

        /// @brief 
        /// @return Error code (0 no error) 
        int32_t getDistanceDetectorMeasureOnWakeup(bool *meas);

        // --------------------- I2C Presence Detector Functions ---------------------
        /// @brief 
        /// @return Error code (0 no error)  
        int32_t presenceDetectorCommand(uint32_t cmd);

        /// @brief 
        /// @return Error code (0 no error)  
        int32_t getpresenceDetectorConfig(acc_detector_presence_config_t *config);

        /// @brief 
        /// @return Error code (0 no error) 
        int32_t getPresenceDetectorStatus(uint32_t *status);

        /// @brief 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorResult(uint32_t *result);

        /// @brief the distance, in millimeters, for the detected presence
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorDistance(float *distance);

        /// @brief Returns the measure of the amount of fast motion detected
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorIntraPresenceScore(float *intraScore);

        /// @brief Returns the measure of the amount of slow motion detected
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorInterPresenceScore(float *interScore);

        /// @brief Get the measure counter, the number of measurements performed
        ///  since restart. 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorCounter(uint32_t *counter);

        /// @brief 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorGPIODetection(bool *en);

        /// @brief 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorGPIODetected(bool detected);

};
