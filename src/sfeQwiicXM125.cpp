#include "SparkFun_Qwiic_XM125_Arduino_Library.h"
#include "sfeQwiicXM125.h"

QwDevXM125::QwDevXM125()
{
    // TODO: Initialize variables
}

bool QwDevXM125::begin(sfeTkII2C *theBus)
{
    if(theBus == nullptr)
    {
        return kSTkErrFail;
    }

    // Check if the provided address is valid
    if(theBus->address() != SFE_XM125_I2C_ADDRESS)
    {
        return kSTkErrFail;
    }

    // Sets communication bus 
    _theBus = theBus;

    
    // TODO: Check if device is connected

    // TODO: Initialize hardware


    // Replicate what is in the main() funciton (without print)

    // Turn the sensor on
    //acc_hal_integration_sensor_supply_on(SENSOR_ID);

    // Create the sensor
    //create_sensor(&detector_resources);



    // Handle GPIO outputs and commands

    // Clear busy bit once the handler is complete 
    // Set ready pin HIGH when command processing is done

    // TODO: Return whether successful


    //return errorI2CCallback();
}

// Include ??
// void QwDevXM125::setCommunicationBus(QwDevXM125::QwIDeviceBus &theBus, uint8_t i2cAddress)
// {
//     _sfeBus = &theBus;
//     _i2cAddress = i2cAddress;
// }


// --------------------- I2C Disance Detector Functions ---------------------

int32_t QwDevXM125::distanceBegin()
{
    // 
}

int32_t QwDevXM125::getDistanceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_VERSION, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &major = (regVal & 0xffff0000) >> 16;
    &minor = (regVal & 0x0000ff00) >> 8;
    &patch = regVal & 0x000000ff;

    return retVal;
}

int32_t QwDevXM125::getDistanceDetectorError(sfe_xm125_distance_protocol_status_t (uint8_t*)error)
{
    int32_t retVal;
    uint32_t regVal;
    uint32_t protocolError;
    uint32_t packetLengthError;
    uint32_t addressError;
    uint32_t writeFailed;
    uint32_t writeToReadOnly;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PROTOCOL_STATUS, (uint8_t*)regVal, 4, 4);

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
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_COUNTER, (uint8_t*)counter, 4, 4);
}

int32_t QwDevXM125::getDistanceDetectorStatus(sfe_xm125_distance_detector_status_t *status)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)status, 4, 4);
}

int32_t QwDevXM125::getDistanceNumberDistances(sfe_xm125_distance_result_t *distance)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &distance = regVal & 0x0000000f;

    return retVal;
}

int32_t QwDevXM125::getDistanceNearStartEdge(sfe_xm125_distance_result_t *edge)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &edge = (regVal &  0x00000100) >> 8;

    return retVal;
}

int32_t QwDevXM125::getDistanceCalibrationNeeded(sfe_xm125_distance_result_t *calibrate)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &calibrate = (regVal &  0x00000200) >> 9;

    return retVal;
}

int32_t QwDevXM125::getDistanceMeasureDistanceError(sfe_xm125_distance_result_t *error)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &error = (regVal &  0x00000400) >> 10;

    return retVal;
}

int32_t QwDevXM125::getDistanceTemperature(sfe_xm125_distance_result_t *temperature)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint32_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &temperature = (regVal &  0xffff0000) >> 16;

    return retVal;
}

int32_t QwDevXM125::getDistancePeak0Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak1Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak2Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak3Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak4Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak5Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak6Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak7Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak8Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak9Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_DISTANCE, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak0Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak1Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak2Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_STRENGTH, (uint8_t*)peak, 4, 4));
}

int32_t QwDevXM125::getDistancePeak3Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak4Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak5Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak6Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak7Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak8Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistancePeak9Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_STRENGTH, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::getDistanceStart(uint32_t *startVal)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_START, (uint8_t*)startVal, 4, 4);
}

int32_t QwDevXM125::setDistanceStart(uint32_t start)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_START, (uint8_t*)start, 4);
}

int32_t QwDevXM125::getDistanceEnd(uint32_t *end)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_END, (uint8_t*)end, 4, 4);
}

int32_t QwDevXM125::setDistanceEnd(uint32_t end)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_START, (uint8_t*)start, 4);
}

int32_t QwDevXM125::getDistanceMaxStepLength(uint32_t *length)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)length, 4, 4);
}

int32_t QwDevXM125::setDistanceMaxStepLength(uint32_t length)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)length, 4);
}

int32_t QwDevXM125::getDistanceCloseRangeLeakageCancellation(bool *range)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)range, 4, 4);
}

int32_t QwDevXM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)range, 4);
}

int32_t QwDevXM125::getDistanceSignalQuality(uint32_t *signal)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)signal, 4, 4);
}

int32_t QwDevXM125::setDistanceSignalQuality(uint32_t signal)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)signal, 4);
}

int32_t QwDevXM125::getDistanceMaxProfile(sfe_xm125_distance_profile_t *profile)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)profile, 4, 4);
}

int32_t QwDevXM125::setDistanceMaxProfile(sfe_xm125_distance_profile_t profile)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)profile, 4);
}

int32_t QwDevXM125::getDistanceThresholdMethod(sfe_xm125_threshold_method_t *method)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)method, 4, 4);
}

int32_t QwDevXM125::setDistanceThresholdMethod(sfe_xm125_threshold_method_t method)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)method, 4);
}

int32_t QwDevXM125::getDistancePeakSorting(sfe_xm125_distance_peak_sorting_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)peak, 4, 4);
}

int32_t QwDevXM125::setDistancePeakSorting(sfe_xm125_distance_peak_sorting_t peak)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistanceNumFramesRecordedThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceFixedAmpThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceThresholdSensitivity(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceThresholdSensitivity(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t *shape)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)shape, 4, 4);
}

int32_t QwDevXM125::setDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t shape)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)shape, 4);
}

int32_t QwDevXM125::getDistanceFixedStrengthThresholdValue(int32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)thresh, 4, 4);
}

int32_t QwDevXM125::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceMeasureOneWakeup(bool *measure)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)measure, 4, 4);
}

int32_t QwDevXM125::setDistanceMeasureOneWakeup(bool measure)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)measure, 4);
}

int32_t QwDevXM125::setCommand(sfe_xm125_distance_command_t *command)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_COMMAND, (uint8_t*)command, 4, 4);
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
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_VERSION, (uint8_t*)regVal, 4, 4);

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

    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_PROTOCOL_STATUS, (uint8_t*)regVal, 4, 4);
}

int32_t QwDevXM125::getPresenceMeasureCounter(uint32_t *counter)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MEASURE_COUNTER, (uint8_t*)counter, 4, 4);
}

int32_t QwDevXM125::getPresenceDetectorStatus(uint32_t *status)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)status, 4, 4);
}

int32_t QwDevXM125::getPresenceDistanceResult(uint32_t *detected)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &detected = (regVal & 0x00000001);

    return retVal;
}

int32_t QwDevXM125::getPresenceNearStartEdge(uint32_t *nearEdge)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &nearEdge = (regVal & 0x00000002) >> 1;

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorRegError(uint32_t *error)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &error = (regVal & 0x00008000) >> 2;

    return retVal;
}

int32_t QwDevXM125::getPresenceTemperature(uint32_t *temp)
{
    int32_t retVal;
    uint32_t regVal;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4, 4);

    // Mask unused bits from register 
    &temp = (regVal & 0xffff0000) >> 16;

    return retVal;
}

int32_t QwDevXM125::getPresenceDistance(uint32_t *distance)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)distance, 4, 4);
}

int32_t QwDevXM125::getPresenceIntraPresenceScore(uint32_t *intra)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)intra, 4, 4);
}

int32_t QwDevXM125::getPresenceInterPresenceScore(uint32_t *inter)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_INTER_PRESENCE, (uint8_t*)inter, 4, 4);
}

int32_t QwDevXM125::getPresenceSweepsPerFrame(uint32_t *sweeps)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)sweeps, 4, 4);
}

int32_t QwDevXM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, (uint8_t*)sweeps, 4);
}

int32_t QwDevXM125::getPresenceInterFramePresenceTimeout(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)time, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceInterPhaseBoostEnabled(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)en, 4, 4);
}

int32_t QwDevXM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionEnabled(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)en, 4, 4);
}

int32_t QwDevXM125::setPresenceInterDetectionEnabled(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceFrameRate(uint32_t *rate)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)rate, 4, 4);
}

int32_t QwDevXM125::setPresenceFrameRate(uint32_t rate)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)rate, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)thresh, 4, 4);
}

int32_t QwDevXM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getPresenceInterDetectionThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)thresh, 4, 4);
}

int32_t QwDevXM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getPresenceInterFrameDeviationTime(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)time, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceInterFrameFastCutoff(uint32_t *cut)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)cut, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)cut, 4);
}

int32_t QwDevXM125::getPresenceInterFrameSlowCutoff(uint32_t *cut)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)cut, 4, 4);
}

int32_t QwDevXM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)cut, 4);
}

int32_t QwDevXM125::getPresenceIntraFrameTimeConst(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)time, 4, 4);
}

int32_t QwDevXM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceIntraOutputTimeConst(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)time, 4, 4);
}

int32_t QwDevXM125::setPresenceIntraOutputTimeConst(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceInterOutputTimeConst(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)time, 4, 4);
}

int32_t QwDevXM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceAutoProfileEn(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)en, 4, 4);
}

int32_t QwDevXM125::setPresenceAutoProfileEn(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceAutoStepLengthEn(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)en, 4, 4);
}

int32_t QwDevXM125::setPresenceAutoStepLengthEn(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceManualProfile(sfe_xm125_presence_manual_profile_t *prof)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)prof, 4, 4);
}

int32_t QwDevXM125::setPresenceManualProfile(sfe_xm125_presence_manual_profile_t prof)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)prof, 4);
}

int32_t QwDevXM125::getPresenceManualStepLength(uint32_t *length)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)length, 4, 4);
}

int32_t QwDevXM125::setPresenceManualStepLength(uint32_t length)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)length, 4);
}

int32_t QwDevXM125::getPresenceStart(uint32_t *start)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_START, (uint8_t*)start, 4, 4);
}

int32_t QwDevXM125::setPresenceStart(uint32_t start)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_START, (uint8_t*)start, 4);
}

int32_t QwDevXM125::getPresenceEnd(uint32_t *end)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_END, (uint8_t*)end, 4, 4);
}

int32_t QwDevXM125::setPresenceEnd(uint32_t end)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_END, (uint8_t*)end, 4);
}

int32_t QwDevXM125::getPresenceResetFilters(bool *reset)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)reset, 4, 4);
}

int32_t QwDevXM125::setPresenceResetFilters(bool reset)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)reset, 4);
}

int32_t QwDevXM125::getPresenceHWAAS(uint32_t *avg)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)avg, 4, 4);
}

int32_t QwDevXM125::setPresenceHWAAS(uint32_t avg)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)avg, 4);
}

int32_t QwDevXM125::getPresenceDetectionOnGPIO(bool *detected)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)detected, 4, 4);
}

int32_t QwDevXM125::setPresenceDetectionOnGPIO(bool detected)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)detected, 4);
}

int32_t QwDevXM125::setPresenceCommand(sfe_xm125_presence_command_t cmd)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, (uint8_t*)cmd, 4);
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
//     i2c_distance_detector_get_peak_strength((uint8_t*)peak)
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


