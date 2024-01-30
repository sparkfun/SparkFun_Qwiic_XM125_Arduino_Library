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
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_VERSION, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *major = (tempVal & 0xffff0000) >> 16;
    *minor = (tempVal & 0x0000ff00) >> 8;
    *patch = tempVal & 0x000000ff;

    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceDetectorError(uint32_t *err)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PROTOCOL_STATUS, data, 4);

    if(error != kSTkErrOk)
        return error;

    *err = uint8To32(data);
    return kSTkErrOk;

}

int32_t QwDevXM125::getDistanceMeasureCounter(uint32_t *counter)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_COUNTER, data, 4);

    if(error != kSTkErrOk)
        return error;

    *counter = uint8To32(data);
    return kSTkErrOk;

}

int32_t QwDevXM125::getDistanceDetectorStatus(uint32_t *status)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_COUNTER, data, 4);

    if(error != kSTkErrOk)
        return error;

    *status = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceNumberDistances(uint32_t *distance)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *distance = (tempVal & 0x0000000f);

    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceNearStartEdge(uint32_t *edge)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *edge = (tempVal & 0x00000100) >> 8;

    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceCalibrationNeeded(uint32_t *calibrate)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *calibrate = (tempVal & 0x00000200) >> 9;

    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceMeasureDistanceError(uint32_t *err)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *err = (tempVal &  0x00000400) >> 10;

    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceTemperature(uint32_t *temperature)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_RESULT, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *temperature = (tempVal & 0xffff0000) >> 16;

    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak0Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak1Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak2Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak3Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak4Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak5Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak6Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak7Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak8Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak9Distance(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak0Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK0_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak1Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK1_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak2Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK2_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak3Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK3_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak4Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK4_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak5Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK5_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak6Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK6_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak7Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK7_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak8Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK8_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistancePeak9Strength(uint32_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK9_STRENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getDistanceStart(uint32_t *startVal)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_START, data, 4);

    if(error != kSTkErrOk)
        return error;

    *startVal = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceStart(uint32_t start)
{
    uint8_t foo[] = uint32To8(start);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_START, foo, 4);
}

int32_t QwDevXM125::getDistanceEnd(uint32_t *end)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_END, data, 4);

    if(error != kSTkErrOk)
        return error;

    *end = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceEnd(uint32_t end)
{
    uint8_t foo[] = uint32To8(end);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_END, foo, 4);
}

int32_t QwDevXM125::getDistanceMaxStepLength(uint32_t *length)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *length = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceMaxStepLength(uint32_t length)
{
    uint8_t foo[] = uint32To8(length);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, foo, 4);
}

int32_t QwDevXM125::getDistanceCloseRangeLeakageCancellation(bool *range)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *range = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    uint8_t foo[] = uint32To8(range);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, foo, 4);
}

int32_t QwDevXM125::getDistanceSignalQuality(uint32_t *signal)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, data, 4);

    if(error != kSTkErrOk)
        return error;

    *signal = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceSignalQuality(uint32_t signal)
{
    uint8_t foo[] = uint32To8(signal);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_SIGNAL_QUALITY, foo, 4);
}

int32_t QwDevXM125::getDistanceMaxProfile(sfe_xm125_distance_profile_t *profile)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *profile = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceMaxProfile(sfe_xm125_distance_profile_t profile)
{
    uint8_t foo[] = uint32To8(profile);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MAX_PROFILE, foo, 4);
}

int32_t QwDevXM125::getDistanceThresholdMethod(sfe_xm125_distance_threshold_method_t *method)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, data, 4);

    if(error != kSTkErrOk)
        return error;

    *method = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceThresholdMethod(sfe_xm125_distance_threshold_method_t method)
{
    uint8_t foo[] = uint32To8(method);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_THRESHOLD_METHOD, foo, 4);
}

int32_t QwDevXM125::getDistancePeakSorting(sfe_xm125_distance_peak_sorting_t *peak)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, data, 4);

    if(error != kSTkErrOk)
        return error;

    *peak = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistancePeakSorting(sfe_xm125_distance_peak_sorting_t peak)
{
    uint8_t foo[] = uint32To8(peak);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_PEAK_SORTING, foo, 4);
}

int32_t QwDevXM125::getDistanceNumFramesRecordedThreshold(uint32_t *thresh)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *thresh = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    uint8_t foo[] = uint32To8(thresh);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, foo, 4);
}

int32_t QwDevXM125::getDistanceFixedAmpThreshold(uint32_t *thresh)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, data, 4);

    if(error != kSTkErrOk)
        return error;

    *thresh = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    uint8_t foo[] = uint32To8(thresh);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, foo, 4);
}

int32_t QwDevXM125::getDistanceThresholdSensitivity(uint32_t *thresh)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, data, 4);

    if(error != kSTkErrOk)
        return error;

    *thresh = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceThresholdSensitivity(uint32_t thresh)
{
    uint8_t foo[] = uint32To8(thresh);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, foo, 4);
}

int32_t QwDevXM125::getDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t *shape)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *shape = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t shape)
{
    uint8_t foo[] = uint32To8(shape);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, foo, 4);
}

int32_t QwDevXM125::getDistanceFixedStrengthThresholdValue(int32_t *thresh)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, data, 4);

    if(error != kSTkErrOk)
        return error;

    *thresh = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    uint8_t foo[] = uint32To8(thresh);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, foo, 4);
}

int32_t QwDevXM125::getDistanceMeasureOneWakeup(bool *measure)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, data, 4);

    if(error != kSTkErrOk)
        return error;

    *measure = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setDistanceMeasureOneWakeup(bool measure)
{
    uint8_t foo[] = uint32To8(measure);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, foo, 4);
}

int32_t QwDevXM125::setDistanceCommand(sfe_xm125_distance_command_t *command)
{
    uint8_t foo[] = uint32To8(command);
    return _theBus->write16BitRegisterRegion(SFE_XM125_DISTANCE_COMMAND, foo, 4);
}

// --------------------- I2C Presence Detector Functions ---------------------

int32_t QwDevXM125::presenceDetectorStart()
{
    Serial.println("Presence Sensor Start function");
    // Set Start to 1000mm
    if(setPresenceStart(1000) != 0)
    {
      return -1;
    }

    // Set end at 5000mm
    if(setPresenceEnd(5000) != 0)
    {
      return -2;
    }

    // Apply configuration
    if(setPresenceCommand(XM125_PRESENCE_APPLY_CONFIGURATION) != 0)
    {
      return -3;
    }

    // Wait for configuration to be done
    if(presenceBusyWait() != 0)
    {
        return -4;
    }

    // Start detector 
    if(startPresenceDetector() != 0)
    {
      return -5;
    }

    // Wait for configuration to be done
    if(presenceBusyWait() != 0)
    {
      return -6;
    }

    return 0;
}

int32_t QwDevXM125::getPresenceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_VERSION, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *major = (tempVal & 0xffff0000) >> 16;
    *minor = (tempVal & 0x0000ff00) >> 8;
    *patch = tempVal & 0x000000ff;

    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceDetectorError(uint32_t *err)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_PROTOCOL_STATUS, data, 4);

    if(error != kSTkErrOk)
        return error;

    *err = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceMeasureCounter(uint32_t *counter)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MEASURE_COUNTER, data, 4);

    if(error != kSTkErrOk)
        return error;

    *counter = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceDetectorStatus(uint32_t *status)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DETECTOR_STATUS, data, 4);

    if(error != kSTkErrOk)
        return error;

    *status = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceDetectorPresenceDetected(uint32_t *detected)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *detected = (tempVal & 0x00000001);

    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceDetectorPresenceStickyDetected(uint32_t *sticky)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *sticky = (tempVal & 0x00000002) >> 1;

    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceDetectorRegError(uint32_t *err)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *err = (tempVal & 0x00008000) >> 14;

    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceTemperature(uint32_t *temp)
{
    uint8_t data[4];
    uint32_t tempVal = 0;
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    // Mask unused bits from register 
    tempVal = uint8To32(data);
    *temp = (tempVal & 0xffff0000) >> 16;

    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceDistance(uint32_t *distance)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DISTANCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *distance = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceIntraPresenceScore(uint32_t *intra)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *intra = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceInterPresenceScore(uint32_t *inter)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_INTER_PRESENCE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *inter = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::getPresenceSweepsPerFrame(uint32_t *sweeps)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_INTRA_PRESENCE_SCORE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *sweeps = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    uint8_t foo[] = uint32To8(sweeps);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, foo, 4);
}

int32_t QwDevXM125::getPresenceInterFramePresenceTimeout(uint32_t *time)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, data, 4);

    if(error != kSTkErrOk)
        return error;

    *time = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    uint8_t foo[] = uint32To8(time);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, foo, 4);
}

int32_t QwDevXM125::getPresenceInterPhaseBoostEnabled(bool *en)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, data, 4);

    if(error != kSTkErrOk)
        return error;

    *en = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    uint8_t foo[] = uint32To8(en);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, foo, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionEnabled(bool *en)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, data, 4);

    if(error != kSTkErrOk)
        return error;

    *en = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterDetectionEnabled(bool en)
{
    uint8_t foo[] = uint32To8(en);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, foo, 4);
}

int32_t QwDevXM125::getPresenceFrameRate(uint32_t *rate)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *rate = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceFrameRate(uint32_t rate)
{
    uint8_t foo[] = uint32To8(rate);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_FRAME_RATE, foo, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionThreshold(uint32_t *thresh)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, data, 4);

    if(error != kSTkErrOk)
        return error;

    *thresh = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    uint8_t foo[] = uint32To8(thresh);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, foo, 4);
}

int32_t QwDevXM125::getPresenceInterDetectionThreshold(uint32_t *thresh)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, data, 4);

    if(error != kSTkErrOk)
        return error;

    *thresh = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    uint8_t foo[] = uint32To8(thresh);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, foo, 4);
}

int32_t QwDevXM125::getPresenceInterFrameDeviationTime(uint32_t *time)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, data, 4);

    if(error != kSTkErrOk)
        return error;

    *time = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    uint8_t foo[] = uint32To8(time);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, foo, 4);
}

int32_t QwDevXM125::getPresenceInterFrameFastCutoff(uint32_t *cut)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, data, 4);

    if(error != kSTkErrOk)
        return error;

    *cut = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    uint8_t foo[] = uint32To8(cut);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, foo, 4);
}

int32_t QwDevXM125::getPresenceInterFrameSlowCutoff(uint32_t *cut)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, data, 4);

    if(error != kSTkErrOk)
        return error;

    *cut = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    uint8_t foo[] = uint32To8(cut);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, foo, 4);
}

int32_t QwDevXM125::getPresenceIntraFrameTimeConst(uint32_t *time)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, data, 4);

    if(error != kSTkErrOk)
        return error;

    *time = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    uint8_t foo[] = uint32To8(time);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, foo, 4);
}

int32_t QwDevXM125::getPresenceIntraOutputTimeConst(uint32_t *time)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, data, 4);

    if(error != kSTkErrOk)
        return error;

    *time = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceIntraOutputTimeConst(uint32_t time)
{
    uint8_t foo[] = uint32To8(time);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, foo, 4);
}

int32_t QwDevXM125::getPresenceInterOutputTimeConst(uint32_t *time)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, data, 4);

    if(error != kSTkErrOk)
        return error;

    *time = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    uint8_t foo[] = uint32To8(time);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, foo, 4);
}

int32_t QwDevXM125::getPresenceAutoProfileEn(bool *en)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, data, 4);

    if(error != kSTkErrOk)
        return error;

    *en = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceAutoProfileEn(bool en)
{
    uint8_t foo[] = uint32To8(en);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, foo, 4);
}

int32_t QwDevXM125::getPresenceAutoStepLengthEn(bool *en)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, data, 4);

    if(error != kSTkErrOk)
        return error;

    *en = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceAutoStepLengthEn(bool en)
{
    uint8_t foo[] = uint32To8(en);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, foo, 4);
}

int32_t QwDevXM125::getPresenceManualProfile(uint8_t *prof)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *prof = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceManualProfile(sfe_xm125_presence_manual_profile_t prof)
{
    uint8_t foo[] = uint32To8(prof);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_PROFILE, foo, 4);
}

int32_t QwDevXM125::getPresenceManualStepLength(uint32_t *length)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, data, 4);

    if(error != kSTkErrOk)
        return error;

    *length = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceManualStepLength(uint32_t length)
{
    uint8_t foo[] = uint32To8(length);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, foo, 4);
}

int32_t QwDevXM125::getPresenceStart(uint32_t *start)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_START, data, 4);

    if(error != kSTkErrOk)
        return error;

    *start = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceStart(uint32_t start)
{
    uint8_t foo[] = uint32To8(start);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_START, foo, 4);
}

int32_t QwDevXM125::getPresenceEnd(uint32_t *end)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_END, data, 4);

    if(error != kSTkErrOk)
        return error;

    *end = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceEnd(uint32_t end)
{
    uint8_t foo[] = uint32To8(end);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_END, foo, 4);
}

int32_t QwDevXM125::getPresenceResetFilters(bool *reset)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, data, 4);

    if(error != kSTkErrOk)
        return error;

    *reset = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceResetFilters(bool reset)
{
    uint8_t foo[] = uint32To8(reset);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, foo, 4);
}

int32_t QwDevXM125::getPresenceHWAAS(uint32_t *avg)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, data, 4);

    if(error != kSTkErrOk)
        return error;

    *avg = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceHWAAS(uint32_t avg)
{
    uint8_t foo[] = uint32To8(avg);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, foo, 4);
}

int32_t QwDevXM125::getPresenceDetectionOnGPIO(bool *detected)
{
    uint8_t data[4];
    sfeTkError_t error = _theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, data, 4);

    if(error != kSTkErrOk)
        return error;

    *detected = uint8To32(data);
    return kSTkErrOk;
}

int32_t QwDevXM125::setPresenceDetectionOnGPIO(bool detected)
{
    uint8_t foo[] = uint32To8(detected);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_HWAAS, foo, 4);
}

int32_t QwDevXM125::setPresenceCommand(sfe_xm125_presence_command_t cmd)
{
    uint8_t foo[] = uint32To8(cmd);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, foo, 4);
}

int32_t QwDevXM125::presenceBusyWait()
{
    uint32_t stat = 0;
    do
    {

        if(_theBus->read16BitRegisterRegion(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)stat, 4) != 0)
        {
            return -1;
        }
    } 
    while ((stat & SFE_XM125_PRESENCE_DETECTOR_STATUS_MASK) != 0);

    return 0; // 0 on success
}

int32_t QwDevXM125::startPresenceDetector()
{
    uint8_t foo[] = uint32To8(XM125_PRESENCE_START_DETECTOR);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, foo, 4);
}

int32_t QwDevXM125::stopPresenceDetector()
{
    uint8_t foo[] = uint32To8(XM125_PRESENCE_STOP_DETECTOR);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, foo, 4);

}

int32_t QwDevXM125::presenceReset()
{
    uint8_t foo[] = uint32To8(XM125_PRESENCE_RESET_MODULE);
    return _theBus->write16BitRegisterRegion(SFE_XM125_PRESENCE_COMMAND, foo, 4);
}