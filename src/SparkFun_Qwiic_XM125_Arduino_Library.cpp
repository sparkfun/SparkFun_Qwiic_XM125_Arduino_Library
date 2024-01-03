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

    // Replicate what is in the main() funciton (without print)

    // Turn the sensor on
    acc_hal_integration_sensor_supply_on(SENSOR_ID);

    // Create the sensor
    create_sensor(&detector_resources);

    // Setup I2C Protocol (from acconeer) - Can we just skip this and Start I2C?
    initI2CApplicationSystem();
    presence_reg_protocol_setup();

    // Handle GPIO outputs and commands
    detector_gpio_output();

    // Clear busy bit once the handler is complete 

    // Set ready pin HIGH when command processing is done



    // TODO: Return whether successful


    return errorI2CCallback();
}

// int32_t XM125::acconeerBegin()
// {
//     // Return an error if there is an issue with setting up the device 
//     return acconeer_main();
// }

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
int32_t XM125::getI2CDistanceDetectorConfig(acc_detector_distance_config_t *config)
{
    *config = i2c_distance_detector_get_config(); 
    return errorI2CCallback();
}

int32_t XM125::distanceDetectorCommand(uint32_t *cmd)
{
    i2c_distance_detector_command(&cmd)
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorStatus(uint32_t *status)
{
    *status = i2c_distance_detector_get_status();
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorResult(uint32_t *result)
{
    *result i2c_distance_detector_get_result();
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorCounter(uint32_t *count)
{
    *count = i2c_distance_detector_get_counter();
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorPeakDistance(float *distance)
{
    *distance = i2c_distance_detector_get_peak_distance();
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorPeakStrength(float *peak, uint16_t id)
{
    i2c_distance_detector_get_peak_strength(&peak)
    return errorI2CCallback();
}

int32_t XM125::setDistanceDetectorMeasureOnWakeup(bool *en)
{
    i2c_distance_detector_measure_on_wakeup(&en);
    return errorI2CCallback();
}

int32_t XM125::getDistanceDetectorMeasureOnWakeup(bool *meas)
{
    *meas = i2c_distance_detector_get_measure_on_wakeup();
    return errorI2CCallback();
}

// --------------------- I2C Presence Detector Functions ---------------------
int32_t XM125::presenceDetectorCommand(uint32_t cmd)
{
    bool err;
    err = i2c_presence_detector_command(cmd);

    int32_t i2cErr = errorI2CCallback();
    if(err || i2cErr)
    {
        return -1;
    }
}

int32_t XM125::getpresenceDetectorConfig(acc_detector_presence_config_t *config)
{
    memcpy(config, i2c_presence_detector_get_config(), sizeof(acc_detector_presence_config_t));
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorStatus(uint32_t *status)
{
    *status = i2c_presence_detector_get_status();
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorResult(uint32_t *result)
{
    *result = i2c_presence_detector_get_result();
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorDistance(float *distance)
{
    *distance = i2c_presence_detector_get_distance();
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorIntraPresenceScore(float *intraScore)
{
    *intraScore = i2c_presence_detector_get_intra_presence_score();
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorInterPresenceScore(float *interScore)
{
    *interScore = i2c_presence_detector_get_inter_presence_score();
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorCounter(uint32_t *counter)
{
    *counter = i2c_presence_detector_get_counter();
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorGPIODetection(bool *en)
{
    i2c_presence_detector_presence_detected_gpio(&en);
    return errorI2CCallback();
}

int32_t XM125::getPresenceDetectorGPIODetected(bool detected)
{
    *detected = i2c_presence_detector_get_presence_detected_gpio();
    return errorI2CCallback();
}


