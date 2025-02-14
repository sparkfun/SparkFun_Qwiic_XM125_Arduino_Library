
#include "sfDevXM125.h"


bool sfDevXM125::begin(sfTkII2C *theBus)
{
    if(theBus == nullptr)
    {
        return ksfTkErrFail;
    }

    // Check if the provided address is valid
    if(theBus->address() != SFE_XM125_I2C_ADDRESS)
    {
        return ksfTkErrFail;
    }

    // Sets communication bus 
    _theBus = theBus;

    // Check errors from device 
    uint32_t distanceError = 0;
    uint32_t presenceError = 0;
    int32_t distFuncErr = 0;
    int32_t presFuncErr = 0;

    distFuncErr = getDistanceDetectorError(distanceError);
    presFuncErr = getDistanceDetectorError(presenceError);
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
int32_t sfDevXM125::distanceBegin()
{
    uint32_t errorStatus = 0;

    // *** Distance Sensor Setup ***
    // Reset sensor configuration to reapply configuration registers
    setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);
    sftk_delay_ms(100);

    distanceBusyWait();

    // Check error and busy bits 
    if(getDistanceDetectorErrorStatus(errorStatus) != 0)
    {
        return 1;
    }

    if(errorStatus != 0)
    {
        return 2;
    }
  
    // Set Start register 
    if(setDistanceStart(sfe_xm125_distance_start_default) != 0)
    {
        return 3;
    }
    sftk_delay_ms(100);  // give time for command to set 

    // Set End register 
    if(setDistanceEnd(sfe_xm125_distance_end_default) != 0)
    {
        return 4;
    }
    sftk_delay_ms(100);  // give time for command to set 

    // Apply configuration 
    if(setDistanceCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION) != 0)
    {
      // Check for errors
      getDistanceDetectorErrorStatus(errorStatus);
      if(errorStatus != 0)
      {
        return 5;
      }
  
      return 6;
    }

    // Poll detector status until busy bit is cleared
    if(distanceBusyWait() != 0)
    {
        return 7; 
    }

    // Check detector status 
    getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
        return 7;
    }

    // Return 0 on no error
    return 0;
}

int32_t sfDevXM125::distanceDetectorReadingSetup()
{
    uint32_t errorStatus = 0;
    uint32_t calibrateNeeded = 0;
    uint32_t measDistErr = 0;

    // Check error bits 
    getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
        return 1;
    }
    
    // Start detector 
    if(setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR) != 0)
    {
        return 2;
    }
    sftk_delay_ms(100); // give time for command to set 
    
    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(distanceBusyWait() != 0)
    {
        return 3;
    }
    
    // Verify that no error bits are set in the detector status register 
    getDistanceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
        return 4;
    }
    sftk_delay_ms(100);

    // Check MEASURE_DISTANCE_ERROR for measurement failed
    getDistanceMeasureDistanceError(measDistErr);
    if(measDistErr == 1)
    {
        return 5;
    }
    sftk_delay_ms(100);

    // Recalibrate device if calibration error is triggered 
    getDistanceCalibrationNeeded(calibrateNeeded);
    if(calibrateNeeded == 1)
    {
        setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
        return 6;
    }
    sftk_delay_ms(100);

    return 0;
}

sfTkError_t sfDevXM125::getDistanceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_VERSION, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    major = (regVal & SFE_XM125_DISTANCE_MAJOR_VERSION_MASK) >> SFE_XM125_DISTANCE_MAJOR_VERSION_MASK_SHIFT;
    minor = (regVal & SFE_XM125_DISTANCE_MINOR_VERSION_MASK) >> SFE_XM125_DISTANCE_MINOR_VERSION_MASK_SHIFT;
    patch = regVal & SFE_XM125_DISTANCE_PATCH_VERSION_MASK;

    return retVal;
}

sfTkError_t sfDevXM125::getDistanceDetectorError(uint32_t &error)
{
    // Read from 16-Bit Register    
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PROTOCOL_STATUS, (uint8_t*)&error, 4, readBytes);
    error = sftk_byte_swap(error);
    return retVal;
}

sfTkError_t sfDevXM125::getDistanceDetectorErrorStatus(uint32_t &status)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // No error
    status = 0;

    if(retVal != ksfTkErrOk)
        return retVal;

    if(((regVal & SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK) >> SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK_SHIFT) != 0)
        status = 1;
    else if(((regVal & SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK) >> SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK_SHIFT) != 0)
        status = 2;
    else if(((regVal & SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK) >> SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK_SHIFT) != 0)
        status = 3;
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK_SHIFT) != 0)
        status = 5;
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK_SHIFT) != 0)
        status = 6;
    else if(((regVal & SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK) >> SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK_SHIFT) != 0)
        status = 7;
    else if(((regVal & SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK) >> SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK_SHIFT) != 0)
        status = 8;
    else if(((regVal & SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK) >> SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK_SHIFT) != 0)
        status = 9;
    else if(((regVal & SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK) >> SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK_SHIFT) != 0)
        status = 10;
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK_SHIFT) != 0)
        status = 11;
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK_SHIFT) != 0)
        status = 12;
    else if(((regVal & SFE_XM125_DISTANCE_BUSY_MASK) >> SFE_XM125_DISTANCE_BUSY_MASK_SHIFT) != 0)
        status = 13;

    return 0; // return 0  with no errors 
}

sfTkError_t sfDevXM125::getDistanceMeasureCounter(uint32_t &counter)
{
    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_MEASURE_COUNTER, (uint8_t*)&counter, 4, readBytes);
    counter = sftk_byte_swap(counter);
    return retVal;
}

sfTkError_t sfDevXM125::getDistanceDetectorStatus(uint32_t &status)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&status, 4, readBytes);
    status = sftk_byte_swap(status);
    return retVal;
}

sfTkError_t sfDevXM125::getDistanceNumberDistances(uint32_t &distance)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    distance = (regVal & SFE_XM125_DISTANCE_NUMBER_DISTANCES_MASK);

    return retVal;
}

sfTkError_t sfDevXM125::getDistanceNearStartEdge(uint32_t &edge)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    edge = (regVal &  SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK) >> SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::getDistanceCalibrationNeeded(uint32_t &calibrate)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    calibrate = (regVal & SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK) >> SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::getDistanceMeasureDistanceError(uint32_t &error)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    error = (regVal &  SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK) >> SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::getDistanceTemperature(uint32_t &temperature)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    temperature = (regVal &  SFE_XM125_DISTANCE_TEMPERATURE_MASK) >> SFE_XM125_DISTANCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak0Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK0_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak1Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK1_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak2Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK2_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak3Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK3_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak4Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK4_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak5Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK5_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak6Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK6_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak7Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK7_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak8Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK8_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak9Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK9_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak0Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK0_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak1Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK1_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak2Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK2_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak3Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK3_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak4Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK4_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak5Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK5_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak6Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK6_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak7Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK7_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak8Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK8_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistancePeak9Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK9_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    peak = peak/1000;
    return retVal;
}

sfTkError_t sfDevXM125::getDistanceStart(uint32_t &startVal)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_START, (uint8_t*)&startVal, 4, readBytes);
    startVal = sftk_byte_swap(startVal);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceStart(uint32_t start)
{
    start = sftk_byte_swap(start);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_START, (uint8_t*)&start, 4);
}

sfTkError_t sfDevXM125::getDistanceEnd(uint32_t &end)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_END, (uint8_t*)&end, 4, readBytes);
    end = sftk_byte_swap(end);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceEnd(uint32_t end)
{
    end = sftk_byte_swap(end);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_END, (uint8_t*)&end, 4);
}

sfTkError_t sfDevXM125::getDistanceMaxStepLength(uint32_t &length)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)&length, 4, readBytes);
    length = sftk_byte_swap(length);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceMaxStepLength(uint32_t length)
{
    length = sftk_byte_swap(length);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)&length, 4);
}

sfTkError_t sfDevXM125::getDistanceCloseRangeLeakageCancellation(bool &range)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)&range, 4, readBytes);
}

sfTkError_t sfDevXM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)&range, 4);
}

sfTkError_t sfDevXM125::getDistanceSignalQuality(uint32_t &signal)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)&signal, 4, readBytes);
    signal = sftk_byte_swap(signal);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceSignalQuality(uint32_t signal)
{
    signal = sftk_byte_swap(signal);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)&signal, 4);
}

sfTkError_t sfDevXM125::getDistanceMaxProfile(uint32_t &profile)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)&profile, 4, readBytes);
    profile = sftk_byte_swap(profile);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceMaxProfile(uint32_t profile)
{
    profile = sftk_byte_swap(profile);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)&profile, 4);
}

sfTkError_t sfDevXM125::getDistanceThresholdMethod(uint32_t &method)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)&method, 4, readBytes);
    method = sftk_byte_swap(method);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceThresholdMethod(uint32_t method)
{
    method = sftk_byte_swap(method);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)&method, 4);
}

sfTkError_t sfDevXM125::getDistancePeakSorting(uint32_t &peak)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)&peak, 4, readBytes);
    peak = sftk_byte_swap(peak);
    return retVal;
}

sfTkError_t sfDevXM125::setDistancePeakSorting(uint32_t peak)
{
    peak = sftk_byte_swap(peak);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)&peak, 4);
}

sfTkError_t sfDevXM125::getDistanceNumFramesRecordedThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)&thresh, 4, readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)&thresh, 4);
}

sfTkError_t sfDevXM125::getDistanceFixedAmpThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)&thresh, 4, readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)&thresh, 4);
}

sfTkError_t sfDevXM125::getDistanceThresholdSensitivity(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)&thresh, 4, readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceThresholdSensitivity(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)&thresh, 4);
}

sfTkError_t sfDevXM125::getDistanceReflectorShape(uint32_t &shape)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)&shape, 4, readBytes);
    shape = sftk_byte_swap(shape);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceReflectorShape(uint32_t shape)
{
    shape = sftk_byte_swap(shape);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)&shape, 4);
}

sfTkError_t sfDevXM125::getDistanceFixedStrengthThresholdValue(int32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)&thresh, 4, readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

sfTkError_t sfDevXM125::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)&thresh, 4);
}

sfTkError_t sfDevXM125::getDistanceMeasureOneWakeup(bool &measure)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)&measure, 4, readBytes);
}

sfTkError_t sfDevXM125::setDistanceMeasureOneWakeup(bool measure)
{
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)&measure, 4);
}

sfTkError_t sfDevXM125::setDistanceCommand(uint32_t command)
{
    command = sftk_byte_swap(command);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_COMMAND, (uint8_t*)&command, 4);
}

sfTkError_t sfDevXM125::distanceApplyConfiguration()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION);
}

sfTkError_t sfDevXM125::distanceStart()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR);
}

sfTkError_t sfDevXM125::distanceStop()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_STOP_DETECTOR);
}

sfTkError_t sfDevXM125::distanceCalibrate()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_CALIBRATE);
}

sfTkError_t sfDevXM125::distanceRecalibrate()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
}

sfTkError_t sfDevXM125::distanceEnableUartLogs()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_ENABLE_UART_LOGS);
}

sfTkError_t sfDevXM125::distanceDisableUartLogs()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_DISABLE_UART_LOGS);
}

sfTkError_t sfDevXM125::distanceLogConfiguration()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_LOG_CONFIGURATION);
}

sfTkError_t sfDevXM125::distanceReset()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);
}

sfTkError_t sfDevXM125::distanceBusyWait()
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;

    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Poll Detector Status until Busy bit is cleared 
    while(((regVal & SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK) >> SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK_SHIFT) != 0)
    {
        retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
        regVal = sftk_byte_swap(regVal);
    }

    // Return error code if non-zero
    if(retVal != 0)
    {
        return retVal;
    }
    return ksfTkErrOk; // 0 on success
}

// --------------------- I2C Presence Detector Functions ---------------------

sfTkError_t sfDevXM125::presenceDetectorStart()
{
    // *** Presence Sensor Setup ***
    uint32_t errorStatus = 0;

    // Reset sensor configuration to reapply configuration registers
    if(setPresenceCommand(SFE_XM125_PRESENCE_RESET_MODULE) != 0)
    {
        return 1;
    }
    sftk_delay_ms(100); // give time for command to set 

    // Check detector status error and busy bits 
    if(getPresenceDetectorErrorStatus(errorStatus) != 0)
    {
        return 2;
    }
    if(errorStatus != 0)
    {
        return 3;
    }
  
    // Set Presence Start register 
    if(setPresenceStart(300) != 0)
    {
        return 4;
    }
    sftk_delay_ms(100); // give time for command to set 

    // Set End register 
    if(setPresenceEnd(2500) != 0)
    {
        return 5;
    }
    sftk_delay_ms(100); // give time for command to set 

    // Apply configuration 
    if(setPresenceCommand(SFE_XM125_PRESENCE_APPLY_CONFIGURATION) != 0)
    {
      // Check for errors
      getPresenceDetectorErrorStatus(errorStatus);
      if(errorStatus != 0)
      {
        return 6;
      }
  
      return 7;
    }
    sftk_delay_ms(100); // give time for command to set 

    // Poll detector status until busy bit is cleared
    if(presenceBusyWait() != 0)
    {
      return 8;
    }

    // Check detector error status 
    getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
      return 9;
    }

    // If no errors, return 0
    return ksfTkErrOk;
}

sfTkError_t sfDevXM125::getPresenceDistanceValuemm(uint32_t &presenceVal)
{
    // Check error bits 
    uint32_t errorStatus = 0;
    uint32_t presenceDetected = 0;
    uint32_t presenceDetectedSticky = 0; 

    getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
        return 1;
    }
    
    // Start detector 
    if(setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR) != 0)
    {
        return 2;
    }
    sftk_delay_ms(100);
    
    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if(presenceBusyWait() != 0)
    {
        return 3;
    }
    
    // Verify that no error bits are set in the detector status register 
    getPresenceDetectorErrorStatus(errorStatus);
    if(errorStatus != 0)
    {
        return 4;
    }

    // Read detector result register and determine detection status
    getPresenceDetectorPresenceDetected(presenceDetected);
    getPresenceDetectorPresenceStickyDetected(presenceDetectedSticky);
    
    if((presenceDetected == 1) | (presenceDetectedSticky == 1))
    {
        getPresenceDistance(presenceVal);
    }
    return ksfTkErrOk;
}

sfTkError_t sfDevXM125::getPresenceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_VERSION, (uint8_t*)&regVal, 4, readBytes);

    // Mask unused bits from register 
    major = (regVal & SFE_XM125_PRESENCE_MAJOR_VERSION_MASK) >> SFE_XM125_PRESENCE_MAJOR_VERSION_MASK_SHIFT;
    minor = (regVal & SFE_XM125_PRESENCE_MINOR_VERSION_MASK) >> SFE_XM125_PRESENCE_MINOR_VERSION_MASK_SHIFT;
    patch = regVal & SFE_XM125_PRESENCE_PATCH_VERSION_MASK;

    return retVal;
}

sfTkError_t sfDevXM125::getPresenceDetectorError(uint32_t &error)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_PROTOCOL_STATUS, (uint8_t*)&error, 4, readBytes);
}

sfTkError_t sfDevXM125::getPresenceMeasureCounter(uint32_t &counter)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_MEASURE_COUNTER, (uint8_t*)&counter, 4, readBytes);
}

sfTkError_t sfDevXM125::getPresenceDetectorStatus(uint32_t &status)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&status, 4, readBytes);
}

sfTkError_t sfDevXM125::getPresenceDetectorErrorStatus(uint32_t &status)
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    if(retVal != 0)
    {
        return -1;
    }

    if(((regVal & SFE_XM125_PRESENCE_RSS_REGISTER_ERROR_MASK) >> SFE_XM125_PRESENCE_RSS_REGISTER_ERROR_MASK_SHIFT) != 0)
    {
        status = 1;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_CONFIG_CREATE_ERROR_MASK) >> SFE_XM125_PRESENCE_CONFIG_CREATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 2;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_SENSOR_CREATE_ERROR_MASK) >> SFE_XM125_PRESENCE_SENSOR_CREATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 3;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_SENSOR_CALIBRATE_ERROR_MASK) >> SFE_XM125_PRESENCE_SENSOR_CALIBRATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 4;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_DETECTOR_CREATE_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_CREATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 5;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_DETECTOR_BUFFER_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_BUFFER_ERROR_MASK_SHIFT) != 0)
    {
        status = 6;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_SENSOR_BUFFER_ERROR_MASK) >> SFE_XM125_PRESENCE_SENSOR_BUFFER_ERROR_MASK_SHIFT) != 0)
    {
        status = 7;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_CONFIG_APPLY_ERROR_MASK) >> SFE_XM125_PRESENCE_CONFIG_APPLY_ERROR_MASK_SHIFT) != 0)
    {
        status = 8;
        return 0;
    }
    else if(((regVal & SFE_XM125_PRESENCE_DETECTOR_REG_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_REG_ERROR_MASK_SHIFT) != 0)
    {
        status = 9;
        return 0;
    }

    // return 0 with no errors
    return ksfTkErrOk;  
}


sfTkError_t sfDevXM125::getPresenceDetectorPresenceDetected(uint32_t &detected)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t*)&regVal, 4, readBytes);

    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    detected = (regVal & SFE_XM125_PRESENCE_DETECTED_MASK);

    return retVal;
}

sfTkError_t sfDevXM125::getPresenceDetectorPresenceStickyDetected(uint32_t &sticky)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t*)&regVal, 4, readBytes);

    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    sticky = (regVal & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) >> 1;

    return retVal;
}

sfTkError_t sfDevXM125::getPresenceDetectorRegError(uint32_t &error)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t*)&regVal, 4, readBytes);

    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register 
    error = (regVal & SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::getPresenceTemperature(uint32_t &temp)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)&regVal, 4, readBytes);

    // Mask unused bits from register 
    temp = (regVal & SFE_XM125_PRESENCE_TEMPERATURE_MASK) >> SFE_XM125_PRESENCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::getPresenceDistance(uint32_t &distance)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)&distance, 4, readBytes);
    distance = sftk_byte_swap(distance);
    return retVal;
}

sfTkError_t sfDevXM125::getPresenceIntraPresenceScore(uint32_t &intra)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)&intra, 4, readBytes);
    intra = sftk_byte_swap(intra);
    return retVal;
}

sfTkError_t sfDevXM125::getPresenceInterPresenceScore(uint32_t &inter)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_INTER_PRESENCE, (uint8_t*)&inter, 4, readBytes);
    inter = sftk_byte_swap(inter);
    return retVal;
}

sfTkError_t sfDevXM125::getPresenceSweepsPerFrame(uint32_t &sweeps)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)&sweeps, 4, readBytes);
    sweeps = sftk_byte_swap(sweeps);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, (uint8_t*)&sweeps, 4);
}

sfTkError_t sfDevXM125::getPresenceInterFramePresenceTimeout(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)&time, 4, readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)&time, 4);
}

sfTkError_t sfDevXM125::getPresenceInterPhaseBoostEnabled(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)&en, 4, readBytes);
}

sfTkError_t sfDevXM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)&en, 4);
}

sfTkError_t sfDevXM125::getPresenceIntraDetectionEnabled(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)&en, 4, readBytes);
}

sfTkError_t sfDevXM125::setPresenceInterDetectionEnabled(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)&en, 4);
}

sfTkError_t sfDevXM125::getPresenceFrameRate(uint32_t &rate)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)&rate, 4, readBytes);
    rate = sftk_byte_swap(rate);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceFrameRate(uint32_t rate)
{
    rate = sftk_byte_swap(rate);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)&rate, 4);
}

sfTkError_t sfDevXM125::getPresenceIntraDetectionThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4, readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4);
}

sfTkError_t sfDevXM125::getPresenceInterDetectionThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4, readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4);
}

sfTkError_t sfDevXM125::getPresenceInterFrameDeviationTime(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)&time, 4, readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)&time, 4);
}

sfTkError_t sfDevXM125::getPresenceInterFrameFastCutoff(uint32_t &cut)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)&cut, 4, readBytes);
    cut = sftk_byte_swap(cut);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    cut = sftk_byte_swap(cut);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)&cut, 4);
}

sfTkError_t sfDevXM125::getPresenceInterFrameSlowCutoff(uint32_t &cut)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)&cut, 4, readBytes);
    cut = sftk_byte_swap(cut);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    cut = sftk_byte_swap(cut);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)&cut, 4);
}

sfTkError_t sfDevXM125::getPresenceIntraFrameTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)&time, 4, readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)&time, 4);
}

sfTkError_t sfDevXM125::getPresenceIntraOutputTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)&time, 4, readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceIntraOutputTimeConst(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)&time, 4);
}

sfTkError_t sfDevXM125::getPresenceInterOutputTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)&time, 4, readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)&time, 4);
}

sfTkError_t sfDevXM125::getPresenceAutoProfileEn(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)&en, 4, readBytes);
}

sfTkError_t sfDevXM125::setPresenceAutoProfileEn(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)&en, 4);
}

sfTkError_t sfDevXM125::getPresenceAutoStepLengthEn(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)&en, 4, readBytes);
}

sfTkError_t sfDevXM125::setPresenceAutoStepLengthEn(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)&en, 4);
}

sfTkError_t sfDevXM125::getPresenceManualProfile(uint32_t &prof)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)&prof, 4, readBytes);
    prof = sftk_byte_swap(prof);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceManualProfile(uint32_t prof)
{
    prof = sftk_byte_swap(prof);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)&prof, 4);
}

sfTkError_t sfDevXM125::getPresenceManualStepLength(uint32_t &length)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)&length, 4, readBytes);
    length = sftk_byte_swap(length);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceManualStepLength(uint32_t length)
{
    length = sftk_byte_swap(length);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)&length, 4);
}

sfTkError_t sfDevXM125::getPresenceStart(uint32_t &start)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_START, (uint8_t*)&start, 4, readBytes);
    start = sftk_byte_swap(start);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceStart(uint32_t start)
{
    start = sftk_byte_swap(start);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_START, (uint8_t*)&start, 4);
}

sfTkError_t sfDevXM125::getPresenceEnd(uint32_t &end)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_END, (uint8_t*)&end, 4, readBytes);
    end = sftk_byte_swap(end);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceEnd(uint32_t end)
{
    end = sftk_byte_swap(end);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_END, (uint8_t*)&end, 4);
}

sfTkError_t sfDevXM125::getPresenceResetFilters(bool &reset)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)&reset, 4, readBytes);
}

sfTkError_t sfDevXM125::setPresenceResetFilters(bool reset)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)&reset, 4);
}

sfTkError_t sfDevXM125::getPresenceHWAAS(uint32_t &avg)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)&avg, 4, readBytes);
}

sfTkError_t sfDevXM125::setPresenceHWAAS(uint32_t avg)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)&avg, 4);
}

sfTkError_t sfDevXM125::getPresenceDetectionOnGPIO(uint32_t &detected)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, (uint8_t*)&detected, 4, readBytes);
    detected = sftk_byte_swap(detected);
    return retVal;
}

sfTkError_t sfDevXM125::setPresenceDetectionOnGPIO(uint32_t detected)
{
    detected = sftk_byte_swap(detected);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, (uint8_t*)&detected, 4);
}

sfTkError_t sfDevXM125::setPresenceCommand(uint32_t cmd)
{
    cmd = sftk_byte_swap(cmd);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_COMMAND, (uint8_t*)&cmd, 4);
}

sfTkError_t sfDevXM125::presenceApplyConfiguration()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

sfTkError_t sfDevXM125::presenceStart()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

sfTkError_t sfDevXM125::presencestop()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_STOP_DETECTOR);
}

sfTkError_t sfDevXM125::presenceEnableUartLogs()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_ENABLE_UART_LOGS);
}

sfTkError_t sfDevXM125::presenceDisableUartLogs()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_DISABLE_UART_LOGS);
}

sfTkError_t sfDevXM125::presenceLogConfiguration()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_LOG_CONFIGURATION);
}

sfTkError_t sfDevXM125::getPresenceBusy(uint32_t &busy)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);

    busy = sftk_byte_swap(busy);
    // Mask unused bits from register 
    busy = (regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> SFE_XM125_PRESENCE_BUSY_MASK_SHIFT;

    return retVal;
}

sfTkError_t sfDevXM125::presenceBusyWait()
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;

    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = sftk_byte_swap(regVal);

    // Poll Detector Status until Busy bit is cleared 
    while(((regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> 30) != 0)
    {
        retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
        regVal = sftk_byte_swap(regVal);
    }

    if(retVal != 0)
    {
        return retVal;
    }
    return ksfTkErrOk; // 0 on success
}
