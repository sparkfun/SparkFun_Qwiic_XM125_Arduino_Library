#include "SparkFun_Qwiic_XM125_Arduino_Library.h"
#include "sfeQwiicXM125.h"

QwDevXM125::QwDevXM125()
{
    // TODO: Initialize variables
}

bool QwDevXM125::begin()
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

int32_t QwDevXM125::getDistanceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_VERSION, regVal, 4, 4);

    // Mask unused bits from register 
    &major = (regVal & 0xffff0000) >> 16;
    &minor = (regVal & 0x0000ff00) >> 8;
    &patch = regVal & 0x000000ff;

    return retVal;
}

int32_t QwDevXM125::getDistanceDetectorError(sfe_xm125_distance_protocol_status_t *error)
{
    int32_t retVal;
    uint32_t regVal;
    uint32_t protocolError;
    uint32_t packetLengthError;
    uint32_t addressError;
    uint32_t writeFailed;
    uint32_t writeToReadOnly;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_PROTOCOL_STATUS, regVal, 4, 4);

    // Mask unused bits from register 
    protocolError = regVal & ;
    packetLengthError = regVal & ;
    addressError = regVal & ;
    writeFailed = regVal & ;
    writeToReadOnly = regVal & ;

//****************FINISH THIS****************
    &error = ;

    return retVal;
}

int32_t QwDevXM125::getDistanceMeasureCounter(uint32_t *counter)
{
    // Read from 16-Bit Register
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_COUNTER, &counter, 4, 4);
}

// Determine how to do this register 
// int32_t QwDevXM125::getDistanceDetectorStatus(sfe_xm125_distance_detector_status_t *status)
// {
//     return read16BitRegisterRegion(SFE_XM125_DISTANCE_DETECTOR_STATUS, &status, 4, 4);
// }

int32_t QwDevXM125::getDistanceNumberDistances(sfe_xm125_distance_result_t *distance)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, &regVal, 4, 4);

    // Mask unused bits from register 
    &distance = regVal & 0x0000000f;

    return retVal;
}

int32_t QwDevXM125::getDistanceNearStartEdge(sfe_xm125_distance_result_t *edge)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, &regVal, 4, 4);

    // Mask unused bits from register 
    &edge = (regVal &  0x00000100) >> 8;

    return retVal;
}

int32_t QwDevXM125::getDistanceCalibrationNeeded(sfe_xm125_distance_result_t *calibrate)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, &regVal, 4, 4);

    // Mask unused bits from register 
    &calibrate = (regVal &  0x00000200) >> 9;

    return retVal;
}

int32_t QwDevXM125::getDistanceMeasureDistanceError(sfe_xm125_distance_result_t *error)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, &regVal, 4, 4);

    // Mask unused bits from register 
    &error = (regVal &  0x00000400) >> 10;

    return retVal;
}

int32_t QwDevXM125::getDistanceTemperature(sfe_xm125_distance_result_t *temperature)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, &regVal, 4, 4);

    // Mask unused bits from register 
    &temperature = (regVal &  0xffff0000) >> 16;

    return retVal;
}

int32_t QwDevXM125::getDistancePeak0Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak1Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak2Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak3Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak4Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak5Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak6Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak7Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak8Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak9Distance(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_DISTANCE, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak0Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak1Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak2Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_STRENGTH, &peak, 4, 4));
}

int32_t QwDevXM125::getDistancePeak3Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak4Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak5Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak6Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak7Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak8Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak9Strength(uint32_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_STRENGTH, &peak, 4, 4);
}

int32_t QwDevXM125::getDistanceStart(uint32_t *startVal)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_START, &startVal, 4, 4);
}

int32_t QwDevXM125::setDistanceStart(uint32_t start)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_START, start, 4);
}

int32_t QwDevXM125::getDistanceEnd(uint32_t *end)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_END, &end, 4, 4);
}

int32_t QwDevXM125::setDistanceEnd(uint32_t end)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_START, start, 4);
}

int32_t QwDevXM125::getDistanceMaxStepLength(uint32_t *length)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, &length, 4, 4);
}

int32_t QwDevXM125::setDistanceMaxStepLength(uint32_t length)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, length, 4);
}

int32_t QwDevXM125::getDistanceCloseRangeLeakageCancellation(bool *range)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, &range, 4, 4);
}

int32_t QwDevXM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, range, 4);
}

int32_t QwDevXM125::getDistanceSignalQuality(uint32_t *signal)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, &signal, 4, 4);
}

int32_t QwDevXM125::setDistanceSignalQuality(uint32_t signal)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, signal, 4);
}

int32_t QwDevXM125::getDistanceMaxProfile(sfe_xm125_distance_profile_t *profile)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, &profile, 4, 4);
}

int32_t QwDevXM125::setDistanceMaxProfile(sfe_xm125_distance_profile_t profile)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, profile, 4);
}

int32_t QwDevXM125::getDistanceThresholdMethod(sfe_xm125_threshold_method_t *method)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, &method, 4, 4);
}

int32_t QwDevXM125::setDistanceThresholdMethod(sfe_xm125_threshold_method_t method)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, method, 4);
}

int32_t QwDevXM125::getDistancePeakSorting(sfe_xm125_distance_peak_sorting_t *peak)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, &peak, 4, 4);
}

int32_t QwDevXM125::setDistancePeakSorting(sfe_xm125_distance_peak_sorting_t peak)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, peak, 4);
}

int32_t QwDevXM125::getDistanceNumFramesRecordedThreshold(uint32_t *thresh)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, &thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, thresh, 4);
}

int32_t QwDevXM125::getDistanceFixedAmpThreshold(uint32_t *thresh)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, &thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, thresh, 4);
}

int32_t QwDevXM125::getDistanceThresholdSensitivity(uint32_t *thresh)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, &thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceThresholdSensitivity(uint32_t thresh)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, thresh, 4);
}

int32_t QwDevXM125::getDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t *shape)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, &shape, 4, 4);
}

int32_t QwDevXM125::setDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t shape)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, shape, 4);
}

int32_t QwDevXM125::getDistanceFixedStrengthThresholdValue(int32_t *thresh)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, &thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, thresh, 4);
}

int32_t QwDevXM125::getDistanceMeasureOneWakeup(bool *measure)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, &measure, 4, 4);
}

int32_t QwDevXM125::setDistanceMeasureOneWakeup(bool measure)
{
    return write16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, measure, 4);
}

int32_t QwDevXM125::setCommand(sfe_xm125_distance_command_t *command)
{
    return read16BitRegisterRegion(SFE_XM125_DISTANCE_COMMAND, &command, 4, 4);
}

// --------------------- I2C Presence Detector Functions ---------------------

int32_t QwDevXM125::presenceBegin()
{
    // 
}

int32_t QwDevXM125::getPresenceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_PRESENCE_VERSION, regVal, 4, 4);

    // Mask unused bits from register 
    &major = (regVal & 0xffff0000) >> 16;
    &minor = (regVal & 0x0000ff00) >> 8;
    &patch = regVal & 0x000000ff;

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorError(uint32_t *error)
{
    int32_t retVal;
    uint32_t regVal;

    retVal = read16BitRegisterRegion(SFE_XM125_DISTANCE_PROTOCOL_STATUS, &regVal, 4, 4);
}

int32_t QwDevXM125::getPresenceMeasureCounter(uint32_t *counter)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_MEASURE_COUNTER, &counter, 4, 4);
}

int32_t QwDevXM125::getPresenceDetectorStatus(uint32_t *status)
{
    // Determine best way to do this - bounce ideas off of Dryw
}

int32_t QwDevXM125::getPresenceDistanceResult(uint32_t *detected)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, regVal, 4, 4);

    // Mask unused bits from register 
    &detected = (regVal & 0x00000001);

    return retVal;
}

int32_t QwDevXM125::getPresenceNearStartEdge(uint32_t *nearEdge)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, regVal, 4, 4);

    // Mask unused bits from register 
    &nearEdge = (regVal & 0x00000002) >> 1;

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorRegError(uint32_t *error)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, regVal, 4, 4);

    // Mask unused bits from register 
    &error = (regVal & 0x00008000) >> 2;

    return retVal;
}

int32_t QwDevXM125::getPresenceTemperature(uint32_t *temp)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, regVal, 4, 4);

    // Mask unused bits from register 
    &temp = (regVal & 0xffff0000) >> 16;

    return retVal;
}

int32_t QwDevXM125::getPresenceDistance(uint32_t *distance)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, &distance, 4, 4);
}

int32_t QwDevXM125::getPresenceIntraPresenceScore(uint32_t *intra)
{
    return read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, &intra, 4, 4);
}

int32_t QwDevXM125::getPresenceInterPresenceScore(uint32_t *inter)
{
    return read16BitRegisterRegion(SFE_XM125_INTER_PRESENCE, &inter, 4, 4);
}

int32_t QwDevXM125::getPresenceSweepsPerFrame(uint32_t *sweeps)
{
    return read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, &sweeps, 4, 4);
}

int32_t QwDevXM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, sweeps, 4);
}

int32_t QwDevXM125::getPresenceInterFramePresenceTimeout(uint32_t *time)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, &time, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, time, 4);
}

int32_t QwDevXM125::getPresenceInterPhaseBoostEnabled(bool *en)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, &en, 4, 4);
}

int32_t QwDevXM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, en, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionEnabled(bool *en)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, &en, 4, 4);
}

int32_t QwDevXM125::setPresenceInterDetectionEnabled(bool en)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, en, 4);
}

int32_t QwDevXM125::getPresenceFrameRate(uint32_t *rate)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, &rate, 4, 4);
}

int32_t QwDevXM125::setPresenceFrameRate(uint32_t rate)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, rate, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionThreshold(uint32_t *thresh)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, &thresh, 4, 4);
}

int32_t QwDevXM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, thresh, 4);
}

int32_t QwDevXM125::getPresenceInterDetectionThreshold(uint32_t *thresh)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, &thresh, 4, 4);
}

int32_t QwDevXM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, thresh, 4);
}

int32_t QwDevXM125::getPresenceInterFrameDeviationTime(uint32_t *time)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, &time, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, time, 4);
}

int32_t QwDevXM125::getPresenceInterFrameFastCutoff(uint32_t *cut)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, &cut, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, cut, 4);
}

int32_t QwDevXM125::getPresenceInterFrameSlowCutoff(uint32_t *cut)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, &cut, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, cut, 4);
}

int32_t QwDevXM125::getPresenceIntraFrameTimeConst(uint32_t *time)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, &time, 4, 4);
}

int32_t QwDevXM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, time, 4);
}

int32_t QwDevXM125::getPresenceIntraOutputTimeConst(uint32_t *time)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, &time, 4, 4);
}

int32_t QwDevXM125::setPresenceIntraOutputTimeConst(uint32_t time)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, time, 4);
}

int32_t QwDevXM125::getPresenceInterOutputTimeConst(uint32_t *time)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, &time, 4, 4);
}

int32_t QwDevXM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, time, 4);
}

int32_t QwDevXM125::getPresenceAutoProfileEn(bool *en)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, &en, 4, 4);
}

int32_t QwDevXM125::setPresenceAutoProfileEn(bool en)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, en, 4);
}

int32_t QwDevXM125::getPresenceAutoStepLengthEn(bool *en)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, &en, 4, 4);
}

int32_t QwDevXM125::setPresenceAutoStepLengthEn(bool en)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, en, 4);
}

int32_t QwDevXM125::getPresenceManualProfile(sfe_xm125_presence_manual_profile_t *prof)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, &prof, 4, 4);
}

int32_t QwDevXM125::setPresenceManualProfile(sfe_xm125_presence_manual_profile_t prof)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, prof, 4);
}

int32_t QwDevXM125::getPresenceManualStepLength(uint32_t *length)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, &length, 4, 4);
}

int32_t QwDevXM125::setPresenceManualStepLength(uint32_t length)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, length, 4);
}

int32_t QwDevXM125::getPresenceStart(uint32_t *start)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_START, &start, 4, 4);
}

int32_t QwDevXM125::setPresenceStart(uint32_t start)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_START, start, 4);
}

int32_t QwDevXM125::getPresenceEnd(uint32_t *end)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_END, &end, 4, 4);
}

int32_t QwDevXM125::setPresenceEnd(uint32_t end)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_END, end, 4);
}

int32_t QwDevXM125::getPresenceResetFilters(bool *reset)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &reset, 4, 4);
}

int32_t QwDevXM125::setPresenceResetFilters(bool reset)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, reset, 4);
}

int32_t QwDevXM125::getPresenceHWAAS(uint32_t *avg)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, &avg, 4, 4);
}

int32_t QwDevXM125::setPresenceHWAAS(uint32_t avg)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, avg, 4);
}

int32_t QwDevXM125::getPresenceDetectionOnGPIO(bool *detected)
{
    return read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, &detected, 4, 4);
}

int32_t QwDevXM125::setPresenceDetectionOnGPIO(bool detected)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, detected, 4);
}
int32_t QwDevXM125::setPresenceCommand(sfe_xm125_presence_command_t cmd)
{
    return write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, cmd, 4);
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


