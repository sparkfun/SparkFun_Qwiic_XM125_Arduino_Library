#include "SparkFun_Qwiic_XM125_Arduino_Library.h"

XM125::XM125()
{
    // TODO: Initialize variables
}

bool XM125::begin()
{
    // TODO: Check if device is connected

    // TODO: Initialize hardware
    // - Initialize STM32 I2C
    // - Initialize Presence and Distance Detection

    initI2CApplicationSystem();

    acconeerBegin();


    // TODO: Return whether successful
    
    return errorI2CCallback();
}

int32_t XM125::acconeerBegin()
{
    // Return an error if there is an issue with setting up the device 
    return acconeer_main();
}

int32_t XM125::disableInterrupts()
{
    disable_interrupts();
    return errorI2CCallback();
}

int32_t XM125::enableInterrupts()
{
    enable_interrupts();
    return errorI2CCallback();
}

int32_t XM125::preparePowerDown()
{
    prepare_power_down();
    return errorI2CCallback();
}

int32_t XM125::resumePowerDown()
{
    resume_power_down();
    return errorI2CCallback();
}

int32_t XM125::getRTCTick()
{
    get_rtc_tick();
    return errorI2CCallback();
}

int32_t XM125::rtcTickToTime()
{
    rtc_tick_to_time();
    return errorI2CCallback();
}

int32_t XM125::setRTCWakeupTime()
{
    rtc_set_next_wakeup_time();
    return errorI2CCallback();
}

int32_t XM125::waitForI2CIdle()
{
    wait_for_i2c_idle();
    return errorI2CCallback();
}

int32_t XM125::prepareRegisterData()
{
    prepare_register_data();
    return errorI2CCallback();
}

int32_t XM125::initI2CApplicationSystem()
{
    i2c_application_system_init();
    return errorI2CCallback();
}

int32_t XM125::i2cApplicationSystemWaitForInterrupt()
{
    i2c_application_system_wait_for_interrupt();
    return errorI2CCallback();
}

int32_t XM125::resetI2CApplicationSystem()
{
    //
    return errorI2CCallback();
}

int32_t XM125::i2cApplicationSystemTestWakeupPin()
{
    i2c_application_system_test_wakeup_pin();
    return errorI2CCallback();
}

int32_t XM125::setI2CApplicationSystemReadyPin(bool en)
{
    i2c_application_system_set_ready_pin(en);
    return errorI2CCallback();
}

int32_t XM125::configureI2CApplicationGPIO0Pin(bool en)
{
    i2c_application_system_setup_generic_gpio_pin(en);
    return errorI2CCallback();
}

int32_t XM125::setI2CApplicationGPIO0Pin(bool en)
{
    i2c_application_system_set_generic_gpio_pin(en);
    return errorI2CCallback();
}

int32_t XM125::enterI2CApplicationLowerPowerState()
{
    i2c_application_enter_low_power_state();
    return errorI2CCallback();
}

int32_t XM125::setI2CApplicationPeriodicWakeup()
{
    //
    return errorI2CCallback();
}

int32_t XM125::errorI2CCallback()
{
    I2C_HandleTypeDef error;

    HAL_I2C_ErrorCallback(&error);

    return error; 
}

// --------------------- I2C Disance Detector Functions ---------------------
int32_t XM125::getI2CDistanceDetectorConfig()
{
    // 
    return errorI2CCallback();
}

int32_t XM125::distanceDetectorCommand()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorStatus()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorResult()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorCounter()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorPeakDistance()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorPeakStrength()
{
    //
    return errorI2CCallback();
}

int32_t XM125::setDistanceDetectorMeasureOnWakeup()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorMeasureOnWakeup()
{
    //
    return errorI2CCallback();
}

// --------------------- I2C Presence Detector Functions ---------------------
int32_t XM125::presenceDetectorCommand()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getpresenceDetectorConfig()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorStatus()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorResult()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorDistance()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorIntraPresenceScore()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorInterPresenceScore()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorCounter()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorGPIODetection()
{
    //
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorPresenceDetection()
{
    //
    return errorI2CCallback();
}




