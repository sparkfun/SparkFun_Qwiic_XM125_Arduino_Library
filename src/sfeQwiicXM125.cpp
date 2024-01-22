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

    // Check errors from device 
    uint32_t distanceError = 0;
    uint32_t presenceError = 0;
    int32_t distFuncErr = getDistanceDetectorError(&distanceError);
    int32_t presFuncErr = getDistanceDetectorError(&presenceError);
    if(distanceError != 0 )
    {
        return -1;
    }
    if(presenceError != 0)
    {
        return -2;
    }
    if((distFuncErr != 0)|| (presFuncErr != 0))
    {
        return -3;
    }

    // If no errors, return 0
    return 0;
}

// --------------------- I2C Disance Detector Functions ---------------------

int32_t QwDevXM125::getDistanceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_VERSION, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *major = (regVal & 0xffff0000) >> 16;
    *minor = (regVal & 0x0000ff00) >> 8;
    *patch = regVal & 0x000000ff;

    return retVal;
}

int32_t QwDevXM125::getDistanceDetectorError(uint32_t *error)
{
    // Read from 16-Bit Register
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PROTOCOL_STATUS, (uint8_t*)error, 4);

}

int32_t QwDevXM125::getDistanceMeasureCounter(uint32_t *counter)
{
    // Read from 16-Bit Register
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_COUNTER, (uint8_t*)counter, 4);
}

int32_t QwDevXM125::getDistanceDetectorStatus(sfe_xm125_distance_detector_status_t *status)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)status, 4);
}

int32_t QwDevXM125::getDistanceNumberDistances(sfe_xm125_distance_result_t *distance)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *(uint32_t*)distance = regVal & 0x0000000f;

    return retVal;
}

int32_t QwDevXM125::getDistanceNearStartEdge(sfe_xm125_distance_result_t *edge)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *(uint32_t*)edge = (regVal &  0x00000100) >> 8;

    return retVal;
}

int32_t QwDevXM125::getDistanceCalibrationNeeded(sfe_xm125_distance_result_t *calibrate)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *(uint32_t*)calibrate = (regVal &  0x00000200) >> 9;

    return retVal;
}

int32_t QwDevXM125::getDistanceMeasureDistanceError(sfe_xm125_distance_result_t *error)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *(uint32_t*)error = (regVal &  0x00000400) >> 10;

    return retVal;
}

int32_t QwDevXM125::getDistanceTemperature(sfe_xm125_distance_result_t *temperature)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *(uint32_t*)temperature = (regVal &  0xffff0000) >> 16;

    return retVal;
}

int32_t QwDevXM125::getDistancePeak0Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak1Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak2Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak3Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak4Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak5Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak6Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak7Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak8Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak9Distance(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_DISTANCE, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak0Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak1Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak2Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak3Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak4Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak5Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak6Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak7Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak8Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistancePeak9Strength(uint32_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_STRENGTH, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistanceStart(uint32_t *startVal)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_START, (uint8_t*)startVal, 4);
}

int32_t QwDevXM125::setDistanceStart(uint32_t start)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_START, (uint8_t*)start, 4);
}

int32_t QwDevXM125::getDistanceEnd(uint32_t *end)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_END, (uint8_t*)end, 4);
}

int32_t QwDevXM125::setDistanceEnd(uint32_t end)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_START, (uint8_t*)end, 4);
}

int32_t QwDevXM125::getDistanceMaxStepLength(uint32_t *length)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)length, 4);
}

int32_t QwDevXM125::setDistanceMaxStepLength(uint32_t length)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)length, 4);
}

int32_t QwDevXM125::getDistanceCloseRangeLeakageCancellation(bool *range)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)range, 4);
}

int32_t QwDevXM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)range, 4);
}

int32_t QwDevXM125::getDistanceSignalQuality(uint32_t *signal)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)signal, 4);
}

int32_t QwDevXM125::setDistanceSignalQuality(uint32_t signal)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)signal, 4);
}

int32_t QwDevXM125::getDistanceMaxProfile(sfe_xm125_distance_profile_t *profile)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)profile, 4);
}

int32_t QwDevXM125::setDistanceMaxProfile(sfe_xm125_distance_profile_t profile)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)profile, 4);
}

int32_t QwDevXM125::getDistanceThresholdMethod(sfe_xm125_distance_threshold_method_t *method)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)method, 4);
}

int32_t QwDevXM125::setDistanceThresholdMethod(sfe_xm125_distance_threshold_method_t method)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)method, 4);
}

int32_t QwDevXM125::getDistancePeakSorting(sfe_xm125_distance_peak_sorting_t *peak)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::setDistancePeakSorting(sfe_xm125_distance_peak_sorting_t peak)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)peak, 4);
}

int32_t QwDevXM125::getDistanceNumFramesRecordedThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceFixedAmpThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceThresholdSensitivity(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::setDistanceThresholdSensitivity(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t *shape)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)shape, 4);
}

int32_t QwDevXM125::setDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t shape)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)shape, 4);
}

int32_t QwDevXM125::getDistanceFixedStrengthThresholdValue(int32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getDistanceMeasureOneWakeup(bool *measure)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)measure, 4);
}

int32_t QwDevXM125::setDistanceMeasureOneWakeup(bool measure)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)measure, 4);
}

int32_t QwDevXM125::setDistanceCommand(sfe_xm125_distance_command_t *command)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_COMMAND, (uint8_t*)command, 4);
}

// --------------------- I2C Presence Detector Functions ---------------------

int32_t QwDevXM125::getPresenceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_VERSION, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *major = (regVal & 0xffff0000) >> 16;
    *minor = (regVal & 0x0000ff00) >> 8;
    *patch = regVal & 0x000000ff;

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorError(uint32_t *error)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_PROTOCOL_STATUS, (uint8_t*)error, 4);
}

int32_t QwDevXM125::getPresenceMeasureCounter(uint32_t *counter)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MEASURE_COUNTER, (uint8_t*)counter, 4);
}

int32_t QwDevXM125::getPresenceDetectorStatus(uint32_t *status)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)status, 4);
}

int32_t QwDevXM125::getPresenceDistanceResult(uint32_t *detected)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *detected = (regVal & 0x00000001);

    return retVal;
}

int32_t QwDevXM125::getPresenceNearStartEdge(uint32_t *nearEdge)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *nearEdge = (regVal & 0x00000002) >> 1;

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorRegError(uint32_t *error)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *error = (regVal & 0x00008000) >> 2;

    return retVal;
}

int32_t QwDevXM125::getPresenceTemperature(uint32_t *temp)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)regVal, 4);

    // Mask unused bits from register 
    *temp = (regVal & 0xffff0000) >> 16;

    return retVal;
}

int32_t QwDevXM125::getPresenceDistance(uint32_t *distance)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)distance, 4);
}

int32_t QwDevXM125::getPresenceIntraPresenceScore(uint32_t *intra)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)intra, 4);
}

int32_t QwDevXM125::getPresenceInterPresenceScore(uint32_t *inter)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_INTER_PRESENCE, (uint8_t*)inter, 4);
}

int32_t QwDevXM125::getPresenceSweepsPerFrame(uint32_t *sweeps)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)sweeps, 4);
}

int32_t QwDevXM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, (uint8_t*)sweeps, 4);
}

int32_t QwDevXM125::getPresenceInterFramePresenceTimeout(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)time, 4);
}

int32_t QwDevXM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceInterPhaseBoostEnabled(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionEnabled(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::setPresenceInterDetectionEnabled(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceFrameRate(uint32_t *rate)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)rate, 4);
}

int32_t QwDevXM125::setPresenceFrameRate(uint32_t rate)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)rate, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getPresenceInterDetectionThreshold(uint32_t *thresh)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, (uint8_t*)thresh, 4);
}

int32_t QwDevXM125::getPresenceInterFrameDeviationTime(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)time, 4);
}

int32_t QwDevXM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceInterFrameFastCutoff(uint32_t *cut)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)cut, 4);
}

int32_t QwDevXM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)cut, 4);
}

int32_t QwDevXM125::getPresenceInterFrameSlowCutoff(uint32_t *cut)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)cut, 4);
}

int32_t QwDevXM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)cut, 4);
}

int32_t QwDevXM125::getPresenceIntraFrameTimeConst(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceIntraOutputTimeConst(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::setPresenceIntraOutputTimeConst(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceInterOutputTimeConst(uint32_t *time)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)time, 4);
}

int32_t QwDevXM125::getPresenceAutoProfileEn(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::setPresenceAutoProfileEn(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceAutoStepLengthEn(bool *en)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::setPresenceAutoStepLengthEn(bool en)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)en, 4);
}

int32_t QwDevXM125::getPresenceManualProfile(sfe_xm125_presence_manual_profile_t *prof)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)prof, 4);
}

int32_t QwDevXM125::setPresenceManualProfile(sfe_xm125_presence_manual_profile_t prof)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)prof, 4);
}

int32_t QwDevXM125::getPresenceManualStepLength(uint32_t *length)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)length, 4);
}

int32_t QwDevXM125::setPresenceManualStepLength(uint32_t length)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)length, 4);
}

int32_t QwDevXM125::getPresenceStart(uint32_t *start)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_START, (uint8_t*)start, 4);
}

int32_t QwDevXM125::setPresenceStart(uint32_t start)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_START, (uint8_t*)start, 4);
}

int32_t QwDevXM125::getPresenceEnd(uint32_t *end)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_END, (uint8_t*)end, 4);
}

int32_t QwDevXM125::setPresenceEnd(uint32_t end)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_END, (uint8_t*)end, 4);
}

int32_t QwDevXM125::getPresenceResetFilters(bool *reset)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)reset, 4);
}

int32_t QwDevXM125::setPresenceResetFilters(bool reset)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)reset, 4);
}

int32_t QwDevXM125::getPresenceHWAAS(uint32_t *avg)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)avg, 4);
}

int32_t QwDevXM125::setPresenceHWAAS(uint32_t avg)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)avg, 4);
}

int32_t QwDevXM125::getPresenceDetectionOnGPIO(bool *detected)
{
    return _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)detected, 4);
}

int32_t QwDevXM125::setPresenceDetectionOnGPIO(bool detected)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)detected, 4);
}

int32_t QwDevXM125::setPresenceCommand(sfe_xm125_presence_command_t cmd)
{
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, (uint8_t*)cmd, 4);
}
