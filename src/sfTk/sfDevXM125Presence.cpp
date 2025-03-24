/**
 * @file sfDevXM125Presence.cpp
 * @brief Implementation of the SparkFun Qwiic XM125  Library.
 *
 * This file contains the implementation of the class for the presence detection functionality.
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#include "sfDevXM125Presence.h"

sfTkError_t sfDevXM125Presence::begin(sfTkII2C *theBus)
{
    // call super to get the device connection working
    sfTkError_t retVal = sfDevXM125Core::begin(theBus);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Check errors from device application
    uint32_t presenceError = 0;
    retVal = getDetectorError(presenceError);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Additional checks for presence errors
    if (presenceError != 0)
        return ksfTkErrFail; // Indicating presence error

    return ksfTkErrOk;
}
//-------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::detectorStart(uint32_t startValue, uint32_t endValue)
{
    // *** Presence Sensor Setup ***
    uint32_t errorStatus = 0;

    // Reset sensor configuration to reapply configuration registers
    if (setCommand(SFE_XM125_PRESENCE_RESET_MODULE) != ksfTkErrOk)
        return 1;

    sftk_delay_ms(100); // give time for command to set

    // Check detector status error and busy bits
    if (getDetectorErrorStatus(errorStatus) != ksfTkErrOk)
        return 2;

    if (errorStatus != 0)
        return 3;

    // Set Presence Start register
    if (setStart(startValue) != ksfTkErrOk)
        return 4;

    sftk_delay_ms(100); // give time for command to set

    // Set End register
    if (setEnd(endValue) != ksfTkErrOk)
        return 5;

    sftk_delay_ms(100); // give time for command to set

    // Apply configuration
    if (setCommand(SFE_XM125_PRESENCE_APPLY_CONFIGURATION) != ksfTkErrOk)
    {
        // Check for errors
        getDetectorErrorStatus(errorStatus);
        return errorStatus != 0 ? 6 : 7;
    }

    sftk_delay_ms(100); // give time for command to set

    // Poll detector status until busy bit is cleared
    if (busyWait() != ksfTkErrOk)
        return 8;

    // Check detector error status
    sfTkError_t retVal = getDetectorErrorStatus(errorStatus);
    if (retVal != ksfTkErrOk || errorStatus != 0)
        return 9;

    // If no errors, return 0
    return ksfTkErrOk;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDistanceValuemm(uint32_t &presenceVal)
{
    // Check error bits
    uint32_t errorStatus = 0;
    uint32_t presenceStatus = 0;

    sfTkError_t retVal = getDetectorErrorStatus(errorStatus);
    if (retVal != ksfTkErrOk || errorStatus != 0)
        return ksfTkErrFail;

    // Start detector
    if (setCommand(SFE_XM125_PRESENCE_START_DETECTOR) != ksfTkErrOk)
        return ksfTkErrFail;

    sftk_delay_ms(100);

    // Poll detector status until busy bit is cleared
    if (busyWait() != ksfTkErrOk)
        return ksfTkErrFail;

    // Verify that no error bits are set in the detector status register
    retVal = getDetectorErrorStatus(errorStatus);
    if (retVal != ksfTkErrOk || errorStatus != 0)
        return ksfTkErrFail;

    // Read from 16-Bit Register to get the presence detection status
    if (_theBus->readRegister(SFE_XM125_PRESENCE_RESULT, presenceStatus) != ksfTkErrOk)
        return ksfTkErrFail;

    // Presence detected NOW or since last check (sticky)
    bool bPresenceDetected = ((presenceStatus & SFE_XM125_PRESENCE_DETECTED_MASK) != 0) ||
                             ((presenceStatus & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) != 0);

    // If presence or a sticky presence is detected, get the distance and return
    if (bPresenceDetected)
        return getDistance(presenceVal);

    // If no presence detected, return 0
    presenceVal = 0;
    return ksfTkErrOk;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
{
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister(SFE_XM125_PRESENCE_VERSION, (uint8_t *)&regVal, sizeof(regVal), readBytes);

    // Mask unused bits from register
    major = (regVal & SFE_XM125_PRESENCE_MAJOR_VERSION_MASK) >> SFE_XM125_PRESENCE_MAJOR_VERSION_MASK_SHIFT;
    minor = (regVal & SFE_XM125_PRESENCE_MINOR_VERSION_MASK) >> SFE_XM125_PRESENCE_MINOR_VERSION_MASK_SHIFT;
    patch = regVal & SFE_XM125_PRESENCE_PATCH_VERSION_MASK;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorError(uint32_t &error)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_PROTOCOL_STATUS, error);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getMeasureCounter(uint32_t &counter)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_MEASURE_COUNTER, counter);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorStatus(uint32_t &status)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, status);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorErrorStatus(uint32_t &status)
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;
    retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, regVal);

    // clear out status
    status = 0;

    if (retVal != ksfTkErrOk)
        return retVal;
    // Any errors - if not, skip the  big bit check pachinko if-else  below
    if ((regVal & SFE_XM125_PRESENCE_ALL_ERROR_MASK) == 0)
        return ksfTkErrOk;

    // Check for errors
    if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_RSS_REGISTER_ERROR_MASK))
        status = 1;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_CONFIG_CREATE_ERROR_MASK))
        status = 2;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_SENSOR_CREATE_ERROR_MASK))
        status = 3;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_SENSOR_CALIBRATE_ERROR_MASK))
        status = 4;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_DETECTOR_CREATE_ERROR_MASK))
        status = 5;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_DETECTOR_BUFFER_ERROR_MASK))
        status = 6;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_SENSOR_BUFFER_ERROR_MASK))
        status = 7;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_CONFIG_APPLY_ERROR_MASK))
        status = 8;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_DETECTOR_REG_ERROR_MASK))
        status = 9;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK))
        status = 10;

    else if (SFTK_CHECK_BITS_SET(regVal, SFE_XM125_PRESENCE_BUSY_MASK))
        status = 11;

    // return 0 with no errors
    return ksfTkErrOk;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorPresenceDetected(uint32_t &detected)
{
    // Read from 16-Bit Register
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_RESULT, detected);

    // Mask unused bits from register
    if (retVal == ksfTkErrOk)
        detected = (detected & SFE_XM125_PRESENCE_DETECTED_MASK);

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorPresenceStickyDetected(uint32_t &sticky)
{

    // Read from 16-Bit Register
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_RESULT, sticky);

    // Mask unused bits from register
    if (retVal == ksfTkErrOk)
        sticky = (sticky & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) >> 1;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectorRegError(uint32_t &error)
{
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_RESULT, error);

    // Mask unused bits from register
    if (retVal == ksfTkErrOk)
        error = (error & SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK_SHIFT;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getTemperature(uint32_t &temp)
{
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DISTANCE, temp);

    // Mask unused bits from register
    temp = (temp & SFE_XM125_PRESENCE_TEMPERATURE_MASK) >> SFE_XM125_PRESENCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDistance(uint32_t &distance)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_DISTANCE, distance);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getIntraPresenceScore(uint32_t &intra)
{
    return _theBus->readRegister(SFE_XM125_INTRA_PRESENCE_SCORE, intra);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterPresenceScore(uint32_t &inter)
{
    return _theBus->readRegister(SFE_XM125_INTER_PRESENCE, inter);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getSweepsPerFrame(uint32_t &sweeps)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, sweeps);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setSweepsPerFrame(uint32_t sweeps)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, sweeps);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterFramePresenceTimeout(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterFramePresenceTimeout(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterPhaseBoostEnabled(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t *)&value,
                                               sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterPhaseBoostEnabled(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegisterUInt8(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, value);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getIntraDetectionEnabled(bool &en)
{
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegisterUInt8(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, value);

    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterDetectionEnabled(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getFrameRate(uint32_t &rate)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_FRAME_RATE, rate);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setFrameRate(uint32_t rate)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_FRAME_RATE, rate);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getIntraDetectionThreshold(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setIntraDetectionThreshold(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterDetectionThreshold(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterDetectionThreshold(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterFrameDeviationTime(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterFrameDeviationTime(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterFrameFastCutoff(uint32_t &cut)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterFrameFastCutoff(uint32_t cut)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterFrameSlowCutoff(uint32_t &cut)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterFrameSlowCutoff(uint32_t cut)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getIntraFrameTimeConst(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setIntraFrameTimeConst(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getIntraOutputTimeConst(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setIntraOutputTimeConst(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getInterOutputTimeConst(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setInterOutputTimeConst(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getAutoProfileEn(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal =
        _theBus->readRegister(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t *)&value, sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setAutoProfileEn(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getAutoStepLengthEn(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t *)&value,
                                               sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setAutoStepLengthEn(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getManualProfile(uint32_t &prof)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_MANUAL_PROFILE, prof);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setManualProfile(uint32_t prof)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_MANUAL_PROFILE, prof);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getManualStepLength(uint32_t &length)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, length);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setManualStepLength(uint32_t length)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, length);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getStart(uint32_t &start)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_START, start);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setStart(uint32_t start)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_START, start);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getEnd(uint32_t &end)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_END, end);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setEnd(uint32_t end)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_END, end);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getResetFilters(bool &reset)
{

    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal =
        _theBus->readRegister(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &value, sizeof(uint8_t), readBytes);
    reset = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setResetFilters(bool reset)
{
    uint8_t value = reset ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getHWAAS(uint32_t &avg)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_HWAAS, avg);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setHWAAS(uint32_t avg)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_HWAAS, avg);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getDetectionOnGPIO(uint32_t &detected)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, detected);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setDetectionOnGPIO(uint32_t detected)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, detected);
}

sfTkError_t sfDevXM125Presence::setCommand(uint32_t cmd)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_COMMAND, cmd);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::applyConfiguration()
{
    return setCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::start()
{
    return setCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::stop()
{
    return setCommand(SFE_XM125_PRESENCE_STOP_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::enableUartLogs()
{
    return setCommand(SFE_XM125_PRESENCE_ENABLE_UART_LOGS);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::disableUartLogs()
{
    return setCommand(SFE_XM125_PRESENCE_DISABLE_UART_LOGS);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::logConfiguration()
{
    return setCommand(SFE_XM125_PRESENCE_LOG_CONFIGURATION);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getBusy(uint32_t &busy)
{
    // Read from 16-Bit Register

    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, busy);

    // Mask unused bits from register
    busy = (busy & SFE_XM125_PRESENCE_BUSY_MASK) >> SFE_XM125_PRESENCE_BUSY_MASK_SHIFT;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::busyWait()
{
    uint32_t regVal = 0;

    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, regVal);

    // Poll Detector Status until Busy bit is cleared
    while (((regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> 30) != 0)
        retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, regVal);

    return retVal;
}