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
    detector_status_clr_bits(PRESENCE_REG_DETECTOR_STATUS_FIELD_BUSY_MASK);
    // Set ready pin HIGH when command processing is done
    i2c_application_system_set_ready_pin(true);

    // TODO: Return whether successful


    return errorI2CCallback();
}

// --------------------- I2C Disance Detector Functions ---------------------

int32_t XM125::distanceBegin()
{
    // 
}

int32_t XM125::getDistanceDetectorVersion(float *version, uint8_t *patch)
{
    // 
}

int32_t XM125::getDistanceDetectorError(sfe_xm125_distance_protocol_status_t *error)
{
    // 
}

int32_t XM125::getDistanceMeasureCounter(uint16_t *counter)
{
    // 
}

int32_t XM125::getDistanceDetectorStatus(sfe_xm125_distance_detector_status_t *status)
{
    // 
}

int32_t XM125::getDistanceNumberDistances(sfe_xm125_distance_result_t *distance)
{
    // 
}

int32_t XM125::getDistanceNearStartEdge(sfe_xm125_distance_result_t *edge)
{
    // 
}

int32_t XM125::getDistanceCalibrationNeeded(sfe_xm125_distance_result_t *calibrate)
{
    // 
}

int32_t XM125::getDistanceMeasureDistanceError(sfe_xm125_distance_result_t *error)
{
    // 
}

int32_t XM125::getDistanceTemperature(sfe_xm125_distance_result_t *temperature)
{
    // 
}

int32_t XM125::getDistancePeak0Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak1Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak2Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak3Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak4Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak5Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak6Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak7Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak8Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak9Distance(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak0Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak1Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak2Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak3Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak4Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak5Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak6Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak7Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak8Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistancePeak9Strength(uint16_t *peak)
{
    // 
}

int32_t XM125::getDistanceStart(uint16_t *startVal)
{
    // 
}

int32_t XM125::setDistanceStart(uint16_t start)
{
    // 
}

int32_t XM125::getDistanceEnd(uint16_t *end)
{
    // 
}

int32_t XM125::setDistanceEnd(uint16_t end)
{
    // 
}

int32_t XM125::getDistanceMaxStepLength(uint16_t *length)
{
    // 
}

int32_t XM125::setDistanceMaxStepLength(uint16_t length)
{
    // 
}

int32_t XM125::getDistanceCloseRangeLeakageCancellation(bool *range)
{
    // 
}

int32_t XM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    // 
}

int32_t XM125::getDistanceSignalQuality(uint16_t *signal)
{
    // 
}

int32_t XM125::setDistanceSignalQuality(uint16_t signal)
{
    // 
}

int32_t XM125::getDistanceMaxProfile(sfe_xm125_distance_profile_t *profile)
{
    // 
}

int32_t XM125::setDistanceMaxProfile(sfe_xm125_distance_profile_t profile)
{
    // 
}

int32_t XM125::getDistanceThresholdMethod(sfe_xm125_threshold_method_t *method)
{
    // 
}

int32_t XM125::setDistanceThresholdMethod(sfe_xm125_threshold_method_t method)
{
    // 
}

int32_t XM125::getDistancePeakSorting(sfe_xm125_distance_peak_sorting_t *peak)
{
    // 
}

int32_t XM125::setDistancePeakSorting(sfe_xm125_distance_peak_sorting_t peak)
{
    // 
}

int32_t XM125::getDistanceNumFramesRecordedThreshold(uint16_t *thresh)
{
    // 
}

int32_t XM125::setDistanceNumFramesRecordedThreshold(uint16_t thresh)
{
    // 
}

int32_t XM125::getDistanceFixedAmpThreshold(uint16_t *thresh)
{
    // 
}

int32_t XM125::setDistanceFixedAmpThreshold(uint16_t thresh)
{
    // 
}

int32_t XM125::getDistanceThresholdSensitivity(uint16_t *thresh)
{
    // 
}

int32_t XM125::setDistanceThresholdSensitivity(uint16_t thresh)
{
    // 
}

int32_t XM125::getDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t *shape)
{
    // 
}

int32_t XM125::setDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t shape)
{
    // 
}

int32_t XM125::getDistanceFixedStrengthThresholdValue(int16_t *thresh)
{
    // 
}

int32_t XM125::setDistanceFixedStrengthThresholdValue(int16_t thresh)
{
    // 
}

int32_t XM125::getDistanceMeasureOneWakeup(bool *measure)
{
    // 
}

int32_t XM125::setDistanceMeasureOneWakeup(bool measure)
{
    // 
}

int32_t XM125::setCommand(sfe_xm125_distance_command_t *command)
{
    // 
}

// --------------------- I2C Presence Detector Functions ---------------------

int32_t XM125::presenceBegin()
{
    // 
}

int32_t XM125::getPresenceDetectorVersion(float *version, uint8_t *version)
{
    // 
}

int32_t XM125::getPresenceDetectorError(uint32_t *error)
{
    // 
}

int32_t XM125::getPresenceMeasureCounter(uint32_t *counter)
{
    // 
}

int32_t XM125::getPresenceDetectorStatus(uint32_t *status)
{
    // 
}

int32_t XM125::getPresenceDistanceResult(uint32_t *detected)
{
    // 
}

int32_t XM125::getPresenceNearStartEdge(uint32_t *nearEdge)
{
    // 
}

int32_t XM125::getPresenceCalibrationNeeded(uint32_t *calibrate)
{
    // 
}

int32_t XM125::getPresenceTemperature(uint32_t *temp)
{
    // 
}

int32_t XM125::getPresenceDistance(uint32_t *distance)
{
    // 
}

int32_t XM125::getPresenceIntraPresenceScore(uint32_t *intra)
{
    // 
}

int32_t XM125::getPresenceInterPresenceScore(uint32_t *inter)
{
    // 
}

int32_t XM125::getPresenceSweepsPerFrame(uint32_t *sweeps)
{
    // 
}

int32_t XM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    // 
}

int32_t XM125::getPresenceInterFramePresenceTimeout(uint32_t *time)
{
    // 
}

int32_t XM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    // 
}

int32_t XM125::getPresenceInterPhaseBoostEnabled(bool *en)
{
    // 
}

int32_t XM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    // 
}

int32_t XM125::getPresenceIntraDetectionEnabled(bool *en)
{
    // 
}

int32_t XM125::setPresenceInterDetectionEnabled(bool en)
{
    // 
}

int32_t XM125::getPresenceFrameRate(uint32_t *rate)
{
    // 
}

int32_t XM125::setPresenceFrameRate(uint32_t rate)
{
    // 
}

int32_t XM125::getPresenceIntraDetectionThreshold(uint32_t *thresh)
{
    // 
}

int32_t XM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    // 
}

int32_t XM125::getPresenceInterDetectionThreshold(uint32_t *thresh)
{
    // 
}

int32_t XM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    // 
}

int32_t XM125::getPresenceInterFrameDeviationTime(uint32_t *time)
{
    // 
}

int32_t XM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    // 
}

int32_t XM125::getPresenceInterFrameFastCutoff(uint32_t *cut)
{
    // 
}

int32_t XM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    // 
}

int32_t XM125::getPresenceInterFrameSlowCutoff(uint32_t *cut)
{
    // 
}

int32_t XM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    // 
}

int32_t XM125::getPresenceIntraFrameTimeConst(uint32_t *time)
{
    // 
}

int32_t XM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    // 
}

int32_t XM125::getPresenceIntraOutputTimeConst(uint32_t *time)
{
    // 
}

int32_t XM125::getPresenceInterOutputTimeConst(uint32_t *time)
{
    // 
}

int32_t XM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    // 
}

int32_t XM125::getPresenceAutoProfileEn(bool *en)
{
    // 
}

int32_t XM125::setPresenceAutoProfileEn(bool en)
{
    // 
}

int32_t XM125::getPresenceAutoStepLengthEn(bool *en)
{
    // 
}

int32_t XM125::setPresenceAutoStepLengthEn(bool en)
{
    // 
}

int32_t XM125::getPresenceManualProfile(sfe_xm125_presence_manual_profile_t *prof)
{
    // 
}

int32_t XM125::setPresenceManualProfile(sfe_xm125_presence_manual_profile_t prof)
{
    // 
}

int32_t XM125::getPresenceManualStepLength(uint32_t *length)
{
    // 
}

int32_t XM125::setPresenceManualStepLength(uint32_t length)
{
    // 
}

int32_t XM125::getPresenceStart(uint32_t *start)
{
    // 
}

int32_t XM125::setPresenceStart(uint32_t start)
{
    // 
}

int32_t XM125::getPresenceEnd(uint32_t *end)
{
    // 
}

int32_t XM125::setPresenceEnd(uint32_t end)
{
    // 
}

int32_t XM125::getPresenceResetFilters(bool *reset)
{
    // 
}

int32_t XM125::setPresenceResetFilters(bool reset)
{
    // 
}

int32_t XM125::getPresenceHWAAS(uint32_t *avg)
{
    // 
}

int32_t XM125::setPresenceHWAAS(uint32_t avg)
{
    // 
}

int32_t XM125::getPresenceDetectionOnGPIO(bool *detected)
{
    // 
}

int32_t XM125::setPresenceDetectionOnGPIO(bool detected)
{
    // 
}
int32_t XM125::setPresenceCommand(sfe_xm125_presence_command_t cmd)
{
    // 
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxx Bad code below xxxxxxxxxxxxxxxxxxxxxxxxxxx

// int32_t XM125::disableInterrupts()
// {
//     disable_interrupts();
//     return errorI2CCallback();
// }

// int32_t XM125::enableInterrupts()
// {
//     enable_interrupts();
//     return errorI2CCallback();
// }

// int32_t XM125::preparePowerDown()
// {
//     prepare_power_down();
//     return errorI2CCallback();
// }

// int32_t XM125::resumePowerDown()
// {
//     resume_power_down();
//     return errorI2CCallback();
// }

// int32_t XM125::getRTCTick()
// {
//     get_rtc_tick();
//     return errorI2CCallback();
// }

// int32_t XM125::rtcTickToTime(uint32_t tick, RTC_TimeTypeDef *time)
// {
//     rtc_tick_to_time(tick, time);
//     return errorI2CCallback();
// }

// int32_t XM125::setRTCWakeupTime()
// {
//     rtc_set_next_wakeup_time();
//     return errorI2CCallback();
// }

// int32_t XM125::waitForI2CIdle()
// {
//     wait_for_i2c_idle();
//     return errorI2CCallback();
// }

// int32_t XM125::prepareRegisterData(I2C_HandleTypeDef *hi2c)
// {
//     prepare_register_data(*hi2c);
//     return errorI2CCallback();
// }

// int32_t XM125::initI2CApplicationSystem()
// {
//     i2c_application_system_init();
//     return errorI2CCallback();
// }

// int32_t XM125::i2cApplicationSystemWaitForInterrupt()
// {
//     i2c_application_system_wait_for_interrupt();
//     return errorI2CCallback();
// }

// // int32_t XM125::resetI2CApplicationSystem()
// // {
// //     //
// //     return errorI2CCallback();
// // }

// int32_t XM125::i2cApplicationSystemTestWakeupPin()
// {
//     i2c_application_system_test_wakeup_pin();
//     return errorI2CCallback();
// }

// int32_t XM125::setI2CApplicationSystemReadyPin(bool en)
// {
//     bool err = i2c_application_system_set_ready_pin(en);

//     if(err == true)
//     {
//         return -1;
//     }
//     else 
//     {
//         return errorI2CCallback();
//     }
    
// }

// int32_t XM125::configureI2CApplicationGPIO0Pin(bool en)
// {
//     i2c_application_system_setup_generic_gpio_pin(en);
//     return errorI2CCallback();
// }

// int32_t XM125::setI2CApplicationGPIO0Pin(bool en)
// {
//     i2c_application_system_set_generic_gpio_pin(en);
//     return errorI2CCallback();
// }

// int32_t XM125::enterI2CApplicationLowerPowerState()
// {
//     i2c_application_enter_low_power_state();
//     return errorI2CCallback();
// }

// int32_t XM125::setI2CApplicationPeriodicWakeup(uint32_t period)
// {
//     i2c_application_set_periodic_wakeup(period);
//     return errorI2CCallback();
// }

// int32_t XM125::errorI2CCallback()
// {
//     I2C_HandleTypeDef error;

//     HAL_I2C_ErrorCallback(&error);

//     return error; 
// }

// // --------------------- I2C Disance Detector Functions ---------------------
// int32_t XM125::getI2CDistanceDetectorConfig(acc_detector_distance_config_t *config)
// {
//     *config = i2c_distance_detector_get_config(); 
//     return errorI2CCallback();
// }

// int32_t XM125::distanceDetectorCommand(uint32_t *cmd)
// {
//     i2c_distance_detector_command(&cmd)
//     return errorI2CCallback();
// }

// int32_t XM125::getDistanceDetectorStatus(uint32_t *status)
// {
//     *status = i2c_distance_detector_get_status();
//     return errorI2CCallback();
// }

// int32_t XM125::getDistanceDetectorResult(uint32_t *result)
// {
//     *result i2c_distance_detector_get_result();
//     return errorI2CCallback();
// }

// int32_t XM125::getDistanceDetectorCounter(uint32_t *count)
// {
//     *count = i2c_distance_detector_get_counter();
//     return errorI2CCallback();
// }

// int32_t XM125::getDistanceDetectorPeakDistance(float *distance)
// {
//     *distance = i2c_distance_detector_get_peak_distance();
//     return errorI2CCallback();
// }

// int32_t XM125::getDistanceDetectorPeakStrength(float *peak, uint16_t id)
// {
//     i2c_distance_detector_get_peak_strength(&peak)
//     return errorI2CCallback();
// }

// int32_t XM125::setDistanceDetectorMeasureOnWakeup(bool *en)
// {
//     i2c_distance_detector_measure_on_wakeup(&en);
//     return errorI2CCallback();
// }

// int32_t XM125::getDistanceDetectorMeasureOnWakeup(bool *meas)
// {
//     *meas = i2c_distance_detector_get_measure_on_wakeup();
//     return errorI2CCallback();
// }

// // --------------------- I2C Presence Detector Functions ---------------------
// int32_t XM125::presenceDetectorCommand(uint32_t cmd)
// {
//     bool err;
//     err = i2c_presence_detector_command(cmd);

//     int32_t i2cErr = errorI2CCallback();
//     if(err || i2cErr)
//     {
//         return -1;
//     }
// }

// int32_t XM125::getpresenceDetectorConfig(acc_detector_presence_config_t *config)
// {
//     memcpy(config, i2c_presence_detector_get_config(), sizeof(acc_detector_presence_config_t));
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorStatus(uint32_t *status)
// {
//     *status = i2c_presence_detector_get_status();
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorResult(uint32_t *result)
// {
//     *result = i2c_presence_detector_get_result();
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorDistance(float *distance)
// {
//     *distance = i2c_presence_detector_get_distance();
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorIntraPresenceScore(float *intraScore)
// {
//     *intraScore = i2c_presence_detector_get_intra_presence_score();
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorInterPresenceScore(float *interScore)
// {
//     *interScore = i2c_presence_detector_get_inter_presence_score();
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorCounter(uint32_t *counter)
// {
//     *counter = i2c_presence_detector_get_counter();
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorGPIODetection(bool *en)
// {
//     i2c_presence_detector_presence_detected_gpio(&en);
//     return errorI2CCallback();
// }

// int32_t XM125::getPresenceDetectorGPIODetected(bool detected)
// {
//     *detected = i2c_presence_detector_get_presence_detected_gpio();
//     return errorI2CCallback();
// }


