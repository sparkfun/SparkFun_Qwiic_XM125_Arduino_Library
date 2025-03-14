/**
 * @file sfDevXM125Distance.h
 * @brief Implementation of the SparkFun Qwiic XM125  Library - Methods for the Distance app
 *
 * This file contains the implementation methods for the distance detection functionality. This
 * is done using a template, which is then paired with the core class to provide the desired
 * functionality.
 *
 * For this paying attention - yes, this is an example of Curiously Recurring Template Pattern (CRTP)
 * design pattern - https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */

#include "sfDevXM125Distance.h"

//------------------------------------------------------------------
// begin method - overrides the super class begin -
//
sfTkError_t sfDevXM125Distance::begin(sfTkII2C *theBus)
{
    // call super to get the device connection working
    sfTkError_t retVal = sfDevXM125Core::init(theBus);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Check errors from device application
    uint32_t distanceError = 0;
    retVal = getDistanceDetectorError(distanceError);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Additional checks for distance errors
    if (distanceError != 0)
        return ksfTkErrFail; // Indicating distance error

    return ksfTkErrOk;
}
//------------------------------------------------------------------
int32_t sfDevXM125Distance::distanceBegin(uint32_t startRange, uint32_t endRange)
{
    uint32_t errorStatus = 0;

    // *** Distance Sensor Setup ***
    // Reset sensor configuration to reapply configuration registers
    setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);
    sftk_delay_ms(100);

    distanceBusyWait();

    // Check error and busy bits
    if (getDistanceDetectorErrorStatus(errorStatus) != 0)
    {
        return 1;
    }

    if (errorStatus != 0)
    {
        return 2;
    }

    // Set Start register
    if (setDistanceStart(startRange) != 0)
    {
        return 3;
    }
    sftk_delay_ms(100); // give time for command to set

    // Set End register
    if (setDistanceEnd(endRange) != 0)
    {
        return 4;
    }
    sftk_delay_ms(100); // give time for command to set

    // Apply configuration and calibrate.
    if (setDistanceCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION) != 0)
    {
        // Check for errors
        getDistanceDetectorErrorStatus(errorStatus);
        if (errorStatus != 0)
        {
            return 5;
        }

        return 6;
    }

    // Poll detector status until busy bit is cleared
    if (distanceBusyWait() != 0)
    {
        return 7;
    }

    // Check detector status
    getDistanceDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        return 7;
    }

    // Return 0 on no error
    return 0;
}

//--------------------------------------------------------------------------------
int32_t sfDevXM125Distance::distanceDetectorReadingSetup()
{
    uint32_t errorStatus = 0;
    uint32_t calibrateNeeded = 0;
    uint32_t measDistErr = 0;

    // Check error bits
    getDistanceDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        return 1;
    }

    // Start detector
    if (setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR) != 0)
    {
        return 2;
    }
    sftk_delay_ms(100); // give time for command to set

    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if (distanceBusyWait() != 0)
    {
        return 3;
    }

    // Verify that no error bits are set in the detector status register
    getDistanceDetectorErrorStatus(errorStatus);
    if (errorStatus != 0)
    {
        return 4;
    }
    sftk_delay_ms(100);

    // Check MEASURE_DISTANCE_ERROR for measurement failed
    getDistanceMeasureDistanceError(measDistErr);
    if (measDistErr == 1)
    {
        return 5;
    }
    sftk_delay_ms(100);

    // Recalibrate device if calibration error is triggered
    getDistanceCalibrationNeeded(calibrateNeeded);
    if (calibrateNeeded == 1)
    {
        setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
        return 6;
    }
    sftk_delay_ms(100);

    return 0;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from the Register
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_VERSION, regVal);

    // Mask unused bits from register
    major = (regVal & SFE_XM125_DISTANCE_MAJOR_VERSION_MASK) >> SFE_XM125_DISTANCE_MAJOR_VERSION_MASK_SHIFT;
    minor = (regVal & SFE_XM125_DISTANCE_MINOR_VERSION_MASK) >> SFE_XM125_DISTANCE_MINOR_VERSION_MASK_SHIFT;
    patch = regVal & SFE_XM125_DISTANCE_PATCH_VERSION_MASK;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceDetectorError(uint32_t &error)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PROTOCOL_STATUS, error);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceDetectorErrorStatus(uint32_t &status)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_DETECTOR_STATUS, regVal);

    // No error
    status = 0;

    if (retVal != ksfTkErrOk)
        return retVal;

    // Any errors - if not, skip the  big bit check pachinko if-else  below
    if ((regVal & SFE_XM125_DISTANCE_ALL_ERROR_MASK) == 0)
        return ksfTkErrOk;
    // okay some bit is set...

    if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK))
        status = 1;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK))
        status = 2;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK))
        status = 3;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK))
        status = 5;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK))
        status = 6;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK))
        status = 7;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK))
        status = 8;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK))
        status = 9;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK))
        status = 10;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK))
        status = 11;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK))
        status = 12;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_DISTANCE_BUSY_MASK))
        status = 13;

    return ksfTkErrOk; // return 0  with no errors
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceMeasureCounter(uint32_t &counter)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_MEASURE_COUNTER, counter);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceDetectorStatus(uint32_t &status)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_DETECTOR_STATUS, status);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceNumberDistances(uint32_t &distance)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_RESULT, regVal);

    // Mask unused bits from register
    distance = (regVal & SFE_XM125_DISTANCE_NUMBER_DISTANCES_MASK);

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceNearStartEdge(uint32_t &edge)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_RESULT, regVal);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Mask unused bits from register
    edge = (regVal & SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK) >> SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK_SHIFT;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceCalibrationNeeded(uint32_t &calibrate)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_RESULT, regVal);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Mask unused bits from register
    calibrate =
        (regVal & SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK) >> SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK_SHIFT;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceMeasureDistanceError(uint32_t &error)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_RESULT, regVal);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Mask unused bits from register
    error = (regVal & SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK) >>
            SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK_SHIFT;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceTemperature(int16_t &temperature)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_RESULT, regVal);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Mask unused bits from register
    temperature = static_cast<int16_t>((regVal & SFE_XM125_DISTANCE_TEMPERATURE_MASK) >>
                                       SFE_XM125_DISTANCE_TEMPERATURE_MASK_SHIFT);

    return retVal;
}

//--------------------------------------------------------------------------------
// Generic distance peak distance method
sfTkError_t sfDevXM125Distance::getDistancePeakDistance(uint8_t num, uint32_t &peak)
{

    switch (num)
    {
    case 0:
        return getDistancePeak0Distance(peak);
        break;

    case 1:
        return getDistancePeak1Distance(peak);
        break;
    case 2:
        return getDistancePeak2Distance(peak);
        break;
    case 3:
        return getDistancePeak3Distance(peak);
        break;
    case 4:
        return getDistancePeak4Distance(peak);
        break;
    case 5:
        return getDistancePeak5Distance(peak);
        break;
    case 6:
        return getDistancePeak6Distance(peak);
        break;
    case 7:
        return getDistancePeak7Distance(peak);
        break;
    case 8:
        return getDistancePeak8Distance(peak);
        break;
    case 9:
        return getDistancePeak9Distance(peak);
        break;
    default:
        return ksfTkErrFail;
        break;
    }
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak0Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK0_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak1Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK1_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak2Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK2_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak3Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK3_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak4Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK4_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak5Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK5_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak6Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK6_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak7Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK7_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak8Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK8_DISTANCE, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak9Distance(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK9_DISTANCE, peak);
}
//--------------------------------------------------------------------------------
// Generic distance peak strength method
sfTkError_t sfDevXM125Distance::getDistancePeakStrength(uint8_t num, int32_t &peak)
{

    switch (num)
    {
    case 0:
        return getDistancePeak0Strength(peak);
        break;

    case 1:
        return getDistancePeak1Strength(peak);
        break;
    case 2:
        return getDistancePeak2Strength(peak);
        break;
    case 3:
        return getDistancePeak3Strength(peak);
        break;
    case 4:
        return getDistancePeak4Strength(peak);
        break;
    case 5:
        return getDistancePeak5Strength(peak);
        break;
    case 6:
        return getDistancePeak6Strength(peak);
        break;
    case 7:
        return getDistancePeak7Strength(peak);
        break;
    case 8:
        return getDistancePeak8Strength(peak);
        break;
    case 9:
        return getDistancePeak9Strength(peak);
        break;
    default:
        return ksfTkErrFail;
        break;
    }
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak0Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK0_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak1Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK1_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak2Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK2_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak3Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK3_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak4Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK4_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak5Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK5_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak6Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK6_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak7Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK7_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak8Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK8_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeak9Strength(int32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK9_STRENGTH, *((uint32_t *)&peak));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceStart(uint32_t &startVal)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_START, startVal);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceStart(uint32_t start)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_START, start);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceEnd(uint32_t &end)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_END, end);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceEnd(uint32_t end)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_END, end);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceMaxStepLength(uint32_t &length)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, length);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceMaxStepLength(uint32_t length)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_MAX_STEP_LENGTH, length);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceCloseRangeLeakageCancellation(bool &range)
{
    uint8_t readVal = 0;
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, readVal);
    if (retVal != ksfTkErrOk)
        return retVal;

    range = (readVal == 0) ? false : true;
    return ksfTkErrOk;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceCloseRangeLeakageCancellation(bool range)
{
    uint8_t value = range ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE, value);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceSignalQuality(uint32_t &signal)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_SIGNAL_QUALITY, signal);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceSignalQuality(uint32_t signal)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_SIGNAL_QUALITY, signal);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceMaxProfile(uint32_t &profile)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_MAX_PROFILE, profile);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceMaxProfile(uint32_t profile)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_MAX_PROFILE, profile);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceThresholdMethod(uint32_t &method)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_THRESHOLD_METHOD, method);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceThresholdMethod(uint32_t method)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_THRESHOLD_METHOD, method);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistancePeakSorting(uint32_t &peak)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_PEAK_SORTING, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistancePeakSorting(uint32_t peak)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_PEAK_SORTING, peak);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceNumFramesRecordedThreshold(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceNumFramesRecordedThreshold(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceFixedAmpThreshold(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceFixedAmpThreshold(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceThresholdSensitivity(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceThresholdSensitivity(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceReflectorShape(uint32_t &shape)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, shape);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceReflectorShape(uint32_t shape)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_REFLECTOR_SHAPE, shape);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceFixedStrengthThresholdValue(int32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, *((uint32_t *)&thresh));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceFixedStrengthThresholdValue(int32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL, *((uint32_t *)&thresh));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::getDistanceMeasureOneWakeup(bool &measure)
{
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, value);
    measure = static_cast<bool>(value);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceMeasureOneWakeup(bool measure)
{
    uint8_t value = static_cast<uint8_t>(measure);
    return _theBus->writeRegister(SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP, value);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::setDistanceCommand(uint32_t command)
{
    return _theBus->writeRegister(SFE_XM125_DISTANCE_COMMAND, command);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceApplyConfiguration()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_APPLY_CONFIGURATION);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceStart()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_START_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceStop()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_STOP_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceCalibrate()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_CALIBRATE);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceRecalibrate()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_RECALIBRATE);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceEnableUartLogs()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_ENABLE_UART_LOGS);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceDisableUartLogs()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_DISABLE_UART_LOGS);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceLogConfiguration()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_LOG_CONFIGURATION);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceReset()
{
    return setDistanceCommand(SFE_XM125_DISTANCE_RESET_MODULE);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Distance::distanceBusyWait()
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;

    retVal = _theBus->readRegister(SFE_XM125_DISTANCE_DETECTOR_STATUS, regVal);

    // Poll Detector Status until Busy bit is cleared
    while (((regVal & SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK) >> SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK_SHIFT) != 0)
        retVal = _theBus->readRegister(SFE_XM125_DISTANCE_DETECTOR_STATUS, regVal);

    return retVal;
}