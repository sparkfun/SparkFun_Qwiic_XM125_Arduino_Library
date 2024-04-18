#include "SparkFun_Qwiic_XM125_Arduino_Library.h"
#include "sfeQwiicXM125.h"

QwDevXM125::QwDevXM125()
{
    // 
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
int32_t QwDevXM125::distanceBegin()
{
    uint32_t errorStatus = 0;

    // *** Distance Sensor Setup ***
    // Reset sensor configuration to reapply configuration registers
    setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);
    delay(100);

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
    delay(100);  // give time for command to set 

    // Set End register 
    if(setDistanceEnd(sfe_xm125_distance_end_default) != 0)
    {
        return 4;
    }
    delay(100);  // give time for command to set 

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

int32_t QwDevXM125::distanceDetectorReadingSetup()
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
    delay(100); // give time for command to set 
    
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
    delay(100);

    // Check MEASURE_DISTANCE_ERROR for measurement failed
    getDistanceMeasureDistanceError(measDistErr);
    if(measDistErr == 1)
    {
        return 5;
    }
    delay(100);

    // Recalibrate device if calibration error is triggered 
    getDistanceCalibrationNeeded(calibrateNeeded);
    if(calibrateNeeded == 1)
    {
        setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
        return 6;
    }
    delay(100);

    return 0;
}

int32_t QwDevXM125::getDistanceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_VERSION, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    major = (regVal & SFE_XM125_DISTANCE_MAJOR_VERSION_MASK) >> SFE_XM125_DISTANCE_MAJOR_VERSION_MASK_SHIFT;
    minor = (regVal & SFE_XM125_DISTANCE_MINOR_VERSION_MASK) >> SFE_XM125_DISTANCE_MINOR_VERSION_MASK_SHIFT;
    patch = regVal & SFE_XM125_DISTANCE_PATCH_VERSION_MASK;

    return retVal;
}

int32_t QwDevXM125::getDistanceDetectorError(uint32_t &error)
{
    // Read from 16-Bit Register    
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PROTOCOL_STATUS, (uint8_t*)&error, 4, readBytes);
    error = __builtin_bswap32(error);
    return retVal;
}

int32_t QwDevXM125::getDistanceDetectorErrorStatus(uint32_t &status)
{
    int32_t retVal;
    uint32_t regVal = 0;
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    if(retVal != 0)
    {
        return -1;
    }

    if(((regVal & SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK) >> SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK_SHIFT) != 0)
    {
        status = 1;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK) >> SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 2;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK) >> SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 3;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 5;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK_SHIFT) != 0)
    {
        status = 6;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK) >> SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK_SHIFT) != 0)
    {
        status = 7;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK) >> SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK_SHIFT) != 0)
    {
        status = 8;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK) >> SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK_SHIFT) != 0)
    {
        status = 9;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK) >> SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 10;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK_SHIFT) != 0)
    {
        status = 11;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK) >> SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK_SHIFT) != 0)
    {
        status = 12;
        return 0;
    }
    else if(((regVal & SFE_XM125_DISTANCE_BUSY_MASK) >> SFE_XM125_DISTANCE_BUSY_MASK_SHIFT) != 0)
    {
        status = 13;
        return 0;
    }

    return 0; // return 0  with no errors 
}

int32_t QwDevXM125::getDistanceMeasureCounter(uint32_t &counter)
{
    // Read from 16-Bit Register
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_MEASURE_COUNTER, (uint8_t*)&counter, 4, readBytes);
    counter = __builtin_bswap32(counter);
    return retVal;
}

int32_t QwDevXM125::getDistanceDetectorStatus(uint32_t &status)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&status, 4, readBytes);
    status = __builtin_bswap32(status);
    return retVal;
}

int32_t QwDevXM125::getDistanceNumberDistances(uint32_t &distance)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    distance = (regVal & SFE_XM125_DISTANCE_NUMBER_DISTANCES_MASK);

    return retVal;
}

int32_t QwDevXM125::getDistanceNearStartEdge(uint32_t &edge)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    edge = (regVal &  SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK) >> SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::getDistanceCalibrationNeeded(uint32_t &calibrate)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    calibrate = (regVal & SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK) >> SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::getDistanceMeasureDistanceError(uint32_t &error)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    error = (regVal &  SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK) >> SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::getDistanceTemperature(uint32_t &temperature)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_RESULT, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    temperature = (regVal &  SFE_XM125_DISTANCE_TEMPERATURE_MASK) >> SFE_XM125_DISTANCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::getDistancePeak0Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK0_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak1Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK1_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak2Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK2_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak3Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK3_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak4Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK4_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak5Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK5_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak6Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK6_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak7Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK7_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak8Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK8_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak9Distance(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK9_DISTANCE, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/10;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak0Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK0_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak1Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK1_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak2Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK2_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak3Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK3_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak4Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK4_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak5Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK5_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak6Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK6_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak7Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK7_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak8Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK8_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistancePeak9Strength(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK9_STRENGTH, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    peak = peak/1000;
    return retVal;
}

int32_t QwDevXM125::getDistanceStart(uint32_t &startVal)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_START, (uint8_t*)&startVal, 4, readBytes);
    startVal = __builtin_bswap32(startVal);
    return retVal;
}

int32_t QwDevXM125::setDistanceStart(uint32_t start)
{
    start = __builtin_bswap32(start);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_START, (uint8_t*)&start, 4);
}

int32_t QwDevXM125::getDistanceEnd(uint32_t &end)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_END, (uint8_t*)&end, 4, readBytes);
    end = __builtin_bswap32(end);
    return retVal;
}

int32_t QwDevXM125::setDistanceEnd(uint32_t end)
{
    end = __builtin_bswap32(end);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_END, (uint8_t*)&end, 4);
}

int32_t QwDevXM125::getDistanceMaxStepLength(uint32_t &length)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)&length, 4, readBytes);
    length = __builtin_bswap32(length);
    return retVal;
}

int32_t QwDevXM125::setDistanceMaxStepLength(uint32_t length)
{
    length = __builtin_bswap32(length);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, (uint8_t*)&length, 4);
}

int32_t QwDevXM125::getDistanceCloseRangeLeakageCancellation(bool &range)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)&range, 4, readBytes);
}

int32_t QwDevXM125::setDistanceCloseRangeLeakageCancellation(bool range)
{
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, (uint8_t*)&range, 4);
}

int32_t QwDevXM125::getDistanceSignalQuality(uint32_t &signal)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)&signal, 4, readBytes);
    signal = __builtin_bswap32(signal);
    return retVal;
}

int32_t QwDevXM125::setDistanceSignalQuality(uint32_t signal)
{
    signal = __builtin_bswap32(signal);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_SIGNAL_QUALITY, (uint8_t*)&signal, 4);
}

int32_t QwDevXM125::getDistanceMaxProfile(uint32_t &profile)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)&profile, 4, readBytes);
    profile = __builtin_bswap32(profile);
    return retVal;
}

int32_t QwDevXM125::setDistanceMaxProfile(uint32_t profile)
{
    profile = __builtin_bswap32(profile);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_MAX_PROFILE, (uint8_t*)&profile, 4);
}

int32_t QwDevXM125::getDistanceThresholdMethod(uint32_t &method)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)&method, 4, readBytes);
    method = __builtin_bswap32(method);
    return retVal;
}

int32_t QwDevXM125::setDistanceThresholdMethod(uint32_t method)
{
    method = __builtin_bswap32(method);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_THRESHOLD_METHOD, (uint8_t*)&method, 4);
}

int32_t QwDevXM125::getDistancePeakSorting(uint32_t &peak)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)&peak, 4, readBytes);
    peak = __builtin_bswap32(peak);
    return retVal;
}

int32_t QwDevXM125::setDistancePeakSorting(uint32_t peak)
{
    peak = __builtin_bswap32(peak);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_PEAK_SORTING, (uint8_t*)&peak, 4);
}

int32_t QwDevXM125::getDistanceNumFramesRecordedThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)&thresh, 4, readBytes);
    thresh = __builtin_bswap32(thresh);
    return retVal;
}

int32_t QwDevXM125::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    thresh = __builtin_bswap32(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, (uint8_t*)&thresh, 4);
}

int32_t QwDevXM125::getDistanceFixedAmpThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)&thresh, 4, readBytes);
    thresh = __builtin_bswap32(thresh);
    return retVal;
}

int32_t QwDevXM125::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    thresh = __builtin_bswap32(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, (uint8_t*)&thresh, 4);
}

int32_t QwDevXM125::getDistanceThresholdSensitivity(uint32_t &thresh)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)&thresh, 4, readBytes);
    thresh = __builtin_bswap32(thresh);
    return retVal;
}

int32_t QwDevXM125::setDistanceThresholdSensitivity(uint32_t thresh)
{
    thresh = __builtin_bswap32(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, (uint8_t*)&thresh, 4);
}

int32_t QwDevXM125::getDistanceReflectorShape(uint32_t &shape)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)&shape, 4, readBytes);
    shape = __builtin_bswap32(shape);
    return retVal;
}

int32_t QwDevXM125::setDistanceReflectorShape(uint32_t shape)
{
    shape = __builtin_bswap32(shape);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, (uint8_t*)&shape, 4);
}

int32_t QwDevXM125::getDistanceFixedStrengthThresholdValue(int32_t &thresh)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)&thresh, 4, readBytes);
    thresh = __builtin_bswap32(thresh);
    return retVal;
}

int32_t QwDevXM125::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    thresh = __builtin_bswap32(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, (uint8_t*)&thresh, 4);
}

int32_t QwDevXM125::getDistanceMeasureOneWakeup(bool &measure)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)&measure, 4, readBytes);
}

int32_t QwDevXM125::setDistanceMeasureOneWakeup(bool measure)
{
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, (uint8_t*)&measure, 4);
}

int32_t QwDevXM125::setDistanceCommand(uint32_t command)
{
    command = __builtin_bswap32(command);
    return _theBus->writeRegister16Region(SFE_XM125_DISTANCE_COMMAND, (uint8_t*)&command, 4);
}

int32_t QwDevXM125::distanceApplyConfiguration()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION);
}

int32_t QwDevXM125::distanceStart()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR);
}

int32_t QwDevXM125::distanceStop()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_STOP_DETECTOR);
}

int32_t QwDevXM125::distanceCalibrate()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_CALIBRATE);
}

int32_t QwDevXM125::distanceRecalibrate()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
}

int32_t QwDevXM125::distanceEnableUartLogs()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_ENABLE_UART_LOGS);
}

int32_t QwDevXM125::distanceDisableUartLogs()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_DISABLE_UART_LOGS);
}

int32_t QwDevXM125::distanceLogConfiguration()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_LOG_CONFIGURATION);
}

int32_t QwDevXM125::distanceReset()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);
}

int32_t QwDevXM125::distanceBusyWait()
{
    int32_t retVal = 0;
    uint32_t regVal = 0;

    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Poll Detector Status until Busy bit is cleared 
    while(((regVal & SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK) >> SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK_SHIFT) != 0)
    {
        retVal = _theBus->readRegister16Region(SFE_XM125_DISTANCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
        regVal = __builtin_bswap32(regVal);
    }

    // Return error code if non-zero
    if(retVal != 0)
    {
        return retVal;
    }
    return 0; // 0 on success
}

// --------------------- I2C Presence Detector Functions ---------------------

int32_t QwDevXM125::presenceDetectorStart()
{
    // *** Presence Sensor Setup ***
    uint32_t errorStatus = 0;

    // Reset sensor configuration to reapply configuration registers
    if(setPresenceCommand(SFE_XM125_PRESENCE_RESET_MODULE) != 0)
    {
        return 1;
    }
    delay(100); // give time for command to set 

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
    delay(100); // give time for command to set 

    // Set End register 
    if(setPresenceEnd(2500) != 0)
    {
        return 5;
    }
    delay(100); // give time for command to set 

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
    delay(100); // give time for command to set 

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
    return 0;
}

int32_t QwDevXM125::getPresenceDistanceValuemm(uint32_t &presenceVal)
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
    delay(100);
    
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
    return 0;
}

int32_t QwDevXM125::getPresenceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
{
    int32_t retVal;
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

int32_t QwDevXM125::getPresenceDetectorError(uint32_t &error)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_PROTOCOL_STATUS, (uint8_t*)&error, 4, readBytes);
}

int32_t QwDevXM125::getPresenceMeasureCounter(uint32_t &counter)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_MEASURE_COUNTER, (uint8_t*)&counter, 4, readBytes);
}

int32_t QwDevXM125::getPresenceDetectorStatus(uint32_t &status)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&status, 4, readBytes);
}

int32_t QwDevXM125::getPresenceDetectorErrorStatus(uint32_t &status)
{
    int32_t retVal = 0;
    uint32_t regVal = 0;
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

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
    return 0;  
}


int32_t QwDevXM125::getPresenceDetectorPresenceDetected(uint32_t &detected)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t*)&regVal, 4, readBytes);

    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    detected = (regVal & SFE_XM125_PRESENCE_DETECTED_MASK);

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorPresenceStickyDetected(uint32_t &sticky)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t*)&regVal, 4, readBytes);

    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    sticky = (regVal & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) >> 1;

    return retVal;
}

int32_t QwDevXM125::getPresenceDetectorRegError(uint32_t &error)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t*)&regVal, 4, readBytes);

    regVal = __builtin_bswap32(regVal);

    // Mask unused bits from register 
    error = (regVal & SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::getPresenceTemperature(uint32_t &temp)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)&regVal, 4, readBytes);

    // Mask unused bits from register 
    temp = (regVal & SFE_XM125_PRESENCE_TEMPERATURE_MASK) >> SFE_XM125_PRESENCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::getPresenceDistance(uint32_t &distance)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DISTANCE, (uint8_t*)&distance, 4, readBytes);
    distance = __builtin_bswap32(distance);
    return retVal;
}

int32_t QwDevXM125::getPresenceIntraPresenceScore(uint32_t &intra)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)&intra, 4, readBytes);
    intra = __builtin_bswap32(intra);
    return retVal;
}

int32_t QwDevXM125::getPresenceInterPresenceScore(uint32_t &inter)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_INTER_PRESENCE, (uint8_t*)&inter, 4, readBytes);
    inter = __builtin_bswap32(inter);
    return retVal;
}

int32_t QwDevXM125::getPresenceSweepsPerFrame(uint32_t &sweeps)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t*)&sweeps, 4, readBytes);
    sweeps = __builtin_bswap32(sweeps);
    return retVal;
}

int32_t QwDevXM125::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, (uint8_t*)&sweeps, 4);
}

int32_t QwDevXM125::getPresenceInterFramePresenceTimeout(uint32_t &time)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)&time, 4, readBytes);
    time = __builtin_bswap32(time);
    return retVal;
}

int32_t QwDevXM125::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t*)&time, 4);
}

int32_t QwDevXM125::getPresenceInterPhaseBoostEnabled(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)&en, 4, readBytes);
}

int32_t QwDevXM125::setPresenceInterPhaseBoostEnabled(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t*)&en, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionEnabled(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)&en, 4, readBytes);
}

int32_t QwDevXM125::setPresenceInterDetectionEnabled(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t*)&en, 4);
}

int32_t QwDevXM125::getPresenceFrameRate(uint32_t &rate)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)&rate, 4, readBytes);
    rate = __builtin_bswap32(rate);
    return retVal;
}

int32_t QwDevXM125::setPresenceFrameRate(uint32_t rate)
{
    rate = __builtin_bswap32(rate);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t*)&rate, 4);
}

int32_t QwDevXM125::getPresenceIntraDetectionThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4, readBytes);
    thresh = __builtin_bswap32(thresh);
    return retVal;
}

int32_t QwDevXM125::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    thresh = __builtin_bswap32(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4);
}

int32_t QwDevXM125::getPresenceInterDetectionThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4, readBytes);
    thresh = __builtin_bswap32(thresh);
    return retVal;
}

int32_t QwDevXM125::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    thresh = __builtin_bswap32(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, (uint8_t*)&thresh, 4);
}

int32_t QwDevXM125::getPresenceInterFrameDeviationTime(uint32_t &time)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)&time, 4, readBytes);
    time = __builtin_bswap32(time);
    return retVal;
}

int32_t QwDevXM125::setPresenceInterFrameDeviationTime(uint32_t time)
{
    time = __builtin_bswap32(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t*)&time, 4);
}

int32_t QwDevXM125::getPresenceInterFrameFastCutoff(uint32_t &cut)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)&cut, 4, readBytes);
    cut = __builtin_bswap32(cut);
    return retVal;
}

int32_t QwDevXM125::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    cut = __builtin_bswap32(cut);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t*)&cut, 4);
}

int32_t QwDevXM125::getPresenceInterFrameSlowCutoff(uint32_t &cut)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)&cut, 4, readBytes);
    cut = __builtin_bswap32(cut);
    return retVal;
}

int32_t QwDevXM125::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    cut = __builtin_bswap32(cut);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t*)&cut, 4);
}

int32_t QwDevXM125::getPresenceIntraFrameTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)&time, 4, readBytes);
    time = __builtin_bswap32(time);
    return retVal;
}

int32_t QwDevXM125::setPresenceIntraFrameTimeConst(uint32_t time)
{
    time = __builtin_bswap32(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t*)&time, 4);
}

int32_t QwDevXM125::getPresenceIntraOutputTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)&time, 4, readBytes);
    time = __builtin_bswap32(time);
    return retVal;
}

int32_t QwDevXM125::setPresenceIntraOutputTimeConst(uint32_t time)
{
    time = __builtin_bswap32(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t*)&time, 4);
}

int32_t QwDevXM125::getPresenceInterOutputTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)&time, 4, readBytes);
    time = __builtin_bswap32(time);
    return retVal;
}

int32_t QwDevXM125::setPresenceInterOutputTimeConst(uint32_t time)
{
    time = __builtin_bswap32(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t*)&time, 4);
}

int32_t QwDevXM125::getPresenceAutoProfileEn(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)&en, 4, readBytes);
}

int32_t QwDevXM125::setPresenceAutoProfileEn(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t*)&en, 4);
}

int32_t QwDevXM125::getPresenceAutoStepLengthEn(bool &en)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)&en, 4, readBytes);
}

int32_t QwDevXM125::setPresenceAutoStepLengthEn(bool en)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t*)&en, 4);
}

int32_t QwDevXM125::getPresenceManualProfile(uint32_t &prof)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)&prof, 4, readBytes);
    prof = __builtin_bswap32(prof);
    return retVal;
}

int32_t QwDevXM125::setPresenceManualProfile(uint32_t prof)
{
    prof = __builtin_bswap32(prof);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t*)&prof, 4);
}

int32_t QwDevXM125::getPresenceManualStepLength(uint32_t &length)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)&length, 4, readBytes);
    length = __builtin_bswap32(length);
    return retVal;
}

int32_t QwDevXM125::setPresenceManualStepLength(uint32_t length)
{
    length = __builtin_bswap32(length);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t*)&length, 4);
}

int32_t QwDevXM125::getPresenceStart(uint32_t &start)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_START, (uint8_t*)&start, 4, readBytes);
    start = __builtin_bswap32(start);
    return retVal;
}

int32_t QwDevXM125::setPresenceStart(uint32_t start)
{
    start = __builtin_bswap32(start);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_START, (uint8_t*)&start, 4);
}

int32_t QwDevXM125::getPresenceEnd(uint32_t &end)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_END, (uint8_t*)&end, 4, readBytes);
    end = __builtin_bswap32(end);
    return retVal;
}

int32_t QwDevXM125::setPresenceEnd(uint32_t end)
{
    end = __builtin_bswap32(end);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_END, (uint8_t*)&end, 4);
}

int32_t QwDevXM125::getPresenceResetFilters(bool &reset)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)&reset, 4, readBytes);
}

int32_t QwDevXM125::setPresenceResetFilters(bool reset)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, (uint8_t*)&reset, 4);
}

int32_t QwDevXM125::getPresenceHWAAS(uint32_t &avg)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)&avg, 4, readBytes);
}

int32_t QwDevXM125::setPresenceHWAAS(uint32_t avg)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_HWAAS, (uint8_t*)&avg, 4);
}

int32_t QwDevXM125::getPresenceDetectionOnGPIO(uint32_t &detected)
{
    size_t readBytes = 0;
    int32_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, (uint8_t*)&detected, 4, readBytes);
    detected = __builtin_bswap32(detected);
    return retVal;
}

int32_t QwDevXM125::setPresenceDetectionOnGPIO(uint32_t detected)
{
    detected = __builtin_bswap32(detected);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, (uint8_t*)&detected, 4);
}

int32_t QwDevXM125::setPresenceCommand(uint32_t cmd)
{
    cmd = __builtin_bswap32(cmd);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_COMMAND, (uint8_t*)&cmd, 4);
}

int32_t QwDevXM125::presenceApplyConfiguration()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

int32_t QwDevXM125::presenceStart()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

int32_t QwDevXM125::presencestop()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_STOP_DETECTOR);
}

int32_t QwDevXM125::presenceEnableUartLogs()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_ENABLE_UART_LOGS);
}

int32_t QwDevXM125::presenceDisableUartLogs()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_DISABLE_UART_LOGS);
}

int32_t QwDevXM125::presenceLogConfiguration()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_LOG_CONFIGURATION);
}

int32_t QwDevXM125::getPresenceBusy(uint32_t &busy)
{
    int32_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);

    busy = __builtin_bswap32(busy);
    // Mask unused bits from register 
    busy = (regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> SFE_XM125_PRESENCE_BUSY_MASK_SHIFT;

    return retVal;
}

int32_t QwDevXM125::presenceBusyWait()
{
    int32_t retVal = 0;
    uint32_t regVal = 0;

    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
    regVal = __builtin_bswap32(regVal);

    // Poll Detector Status until Busy bit is cleared 
    while(((regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> 30) != 0)
    {
        retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t*)&regVal, 4, readBytes);
        regVal = __builtin_bswap32(regVal);
    }

    if(retVal != 0)
    {
        return retVal;
    }
    return 0; // 0 on success
}
