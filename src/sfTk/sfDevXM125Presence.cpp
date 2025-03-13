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
    retVal = getPresenceDetectorError(presenceError);
    if (retVal != ksfTkErrOk)
        return retVal;

    // Additional checks for presence errors
    if (presenceError != 0)
        return ksfTkErrFail; // Indicating presence error

    return ksfTkErrOk;
}
//-------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceDetectorStart()
{
    // *** Presence Sensor Setup ***
    uint32_t errorStatus = 0;

    // Reset sensor configuration to reapply configuration registers
    if (setPresenceCommand(SFE_XM125_PRESENCE_RESET_MODULE) != ksfTkErrOk)
        return 1;

    sftk_delay_ms(100); // give time for command to set

    // Check detector status error and busy bits
    if (getPresenceDetectorErrorStatus(errorStatus) != ksfTkErrOk)
        return 2;

    if (errorStatus != 0)
        return 3;

    // Set Presence Start register
    if (setPresenceStart(300) != ksfTkErrOk)
        return 4;

    sftk_delay_ms(100); // give time for command to set

    // Set End register
    if (setPresenceEnd(2500) != ksfTkErrOk)
        return 5;

    sftk_delay_ms(100); // give time for command to set

    // Apply configuration
    if (setPresenceCommand(SFE_XM125_PRESENCE_APPLY_CONFIGURATION) != ksfTkErrOk)
    {
        // Check for errors
        getPresenceDetectorErrorStatus(errorStatus);
        return errorStatus != 0 ? 6 : 7;
    }

    sftk_delay_ms(100); // give time for command to set

    // Poll detector status until busy bit is cleared
    if (presenceBusyWait() != ksfTkErrOk)
        return 8;

    // Check detector error status
    sfTkError_t retVal = getPresenceDetectorErrorStatus(errorStatus);
    if (retVal != ksfTkErrOk || errorStatus != 0)
        return 9;

    // If no errors, return 0
    return ksfTkErrOk;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDistanceValuemm(uint32_t &presenceVal)
{
    // Check error bits
    uint32_t errorStatus = 0;
    uint32_t presenceStatus = 0;

    sfTkError_t retVal = getPresenceDetectorErrorStatus(errorStatus);
    if (retVal != ksfTkErrOk || errorStatus != 0)
        return 1;

    // Start detector
    if (setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR) != ksfTkErrOk)
        return 2;

    sftk_delay_ms(100);

    // Poll detector status until busy bit is cleared - CHECK ON THIS!
    if (presenceBusyWait() != ksfTkErrOk)
        return 3;

    // Verify that no error bits are set in the detector status register
    retVal = getPresenceDetectorErrorStatus(errorStatus);
    if (retVal != ksfTkErrOk || errorStatus != 0)
        return 4;

    // Read from 16-Bit Register to get the presence detection status
    if (_theBus->readRegister(SFE_XM125_PRESENCE_RESULT, presenceStatus) != ksfTkErrOk)
        return 5;

    // Presence detected NOW or since last check (sticky)
    bool bPresenceDetected = ((presenceStatus & SFE_XM125_PRESENCE_DETECTED_MASK) != 0) ||
                             ((presenceStatus & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) != 0);

    // If presence or a sticky presence is detected, get the distance and return
    if (bPresenceDetected)
        return getPresenceDistance(presenceVal);

    return ksfTkErrOk;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
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
sfTkError_t sfDevXM125Presence::getPresenceDetectorError(uint32_t &error)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_PROTOCOL_STATUS, error);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceMeasureCounter(uint32_t &counter)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_MEASURE_COUNTER, counter);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorStatus(uint32_t &status)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, status);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorErrorStatus(uint32_t &status)
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
sfTkError_t sfDevXM125Presence::getPresenceDetectorPresenceDetected(uint32_t &detected)
{
    // Read from 16-Bit Register
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_RESULT, detected);

    // Mask unused bits from register
    if (retVal == ksfTkErrOk)
        detected = (detected & SFE_XM125_PRESENCE_DETECTED_MASK);

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorPresenceStickyDetected(uint32_t &sticky)
{

    // Read from 16-Bit Register
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_RESULT, sticky);

    // Mask unused bits from register
    if (retVal == ksfTkErrOk)
        sticky = (sticky & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) >> 1;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorRegError(uint32_t &error)
{
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_RESULT, error);

    // Mask unused bits from register
    if (retVal == ksfTkErrOk)
        error = (error & SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK_SHIFT;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceTemperature(uint32_t &temp)
{
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DISTANCE, temp);

    // Mask unused bits from register
    temp = (temp & SFE_XM125_PRESENCE_TEMPERATURE_MASK) >> SFE_XM125_PRESENCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDistance(uint32_t &distance)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_DISTANCE, distance);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraPresenceScore(uint32_t &intra)
{
    return _theBus->readRegister(SFE_XM125_INTRA_PRESENCE_SCORE, intra);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterPresenceScore(uint32_t &inter)
{
    return _theBus->readRegister(SFE_XM125_INTER_PRESENCE, inter);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceSweepsPerFrame(uint32_t &sweeps)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, sweeps);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, sweeps);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFramePresenceTimeout(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterPhaseBoostEnabled(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t *)&value,
                                               sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterPhaseBoostEnabled(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegisterUInt8(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, value);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraDetectionEnabled(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegisterUInt8(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, value);

    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterDetectionEnabled(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceFrameRate(uint32_t &rate)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_FRAME_RATE, rate);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceFrameRate(uint32_t rate)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_FRAME_RATE, rate);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraDetectionThreshold(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterDetectionThreshold(uint32_t &thresh)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, thresh);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFrameDeviationTime(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFrameDeviationTime(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFrameFastCutoff(uint32_t &cut)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFrameSlowCutoff(uint32_t &cut)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, cut);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraFrameTimeConst(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceIntraFrameTimeConst(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraOutputTimeConst(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceIntraOutputTimeConst(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterOutputTimeConst(uint32_t &time)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterOutputTimeConst(uint32_t time)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, time);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceAutoProfileEn(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal =
        _theBus->readRegister(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t *)&value, sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceAutoProfileEn(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceAutoStepLengthEn(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t *)&value,
                                               sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceAutoStepLengthEn(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceManualProfile(uint32_t &prof)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_MANUAL_PROFILE, prof);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceManualProfile(uint32_t prof)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_MANUAL_PROFILE, prof);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceManualStepLength(uint32_t &length)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, length);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceManualStepLength(uint32_t length)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, length);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceStart(uint32_t &start)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_START, start);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceStart(uint32_t start)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_START, start);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceEnd(uint32_t &end)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_END, end);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceEnd(uint32_t end)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_END, end);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceResetFilters(bool &reset)
{

    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal =
        _theBus->readRegister(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &value, sizeof(uint8_t), readBytes);
    reset = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceResetFilters(bool reset)
{
    uint8_t value = reset ? 1 : 0;
    return _theBus->writeRegister(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceHWAAS(uint32_t &avg)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_HWAAS, avg);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceHWAAS(uint32_t avg)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_HWAAS, avg);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectionOnGPIO(uint32_t &detected)
{
    return _theBus->readRegister(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, detected);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceDetectionOnGPIO(uint32_t detected)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, detected);
}

sfTkError_t sfDevXM125Presence::setPresenceCommand(uint32_t cmd)
{
    return _theBus->writeRegister(SFE_XM125_PRESENCE_COMMAND, cmd);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceApplyConfiguration()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceStart()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_START_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceStop()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_STOP_DETECTOR);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceEnableUartLogs()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_ENABLE_UART_LOGS);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceDisableUartLogs()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_DISABLE_UART_LOGS);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceLogConfiguration()
{
    return setPresenceCommand(SFE_XM125_PRESENCE_LOG_CONFIGURATION);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceBusy(uint32_t &busy)
{
    // Read from 16-Bit Register

    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, busy);

    // Mask unused bits from register
    busy = (busy & SFE_XM125_PRESENCE_BUSY_MASK) >> SFE_XM125_PRESENCE_BUSY_MASK_SHIFT;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceBusyWait()
{
    uint32_t regVal = 0;

    sfTkError_t retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, regVal);

    // Poll Detector Status until Busy bit is cleared
    while (((regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> 30) != 0)
        retVal = _theBus->readRegister(SFE_XM125_PRESENCE_DETECTOR_STATUS, regVal);

    return retVal;
}