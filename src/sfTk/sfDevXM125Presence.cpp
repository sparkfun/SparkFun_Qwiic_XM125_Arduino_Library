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
    uint32_t presenceDetected = 0;
    uint32_t presenceDetectedSticky = 0;

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

    // Read detector result register and determine detection status
    if (getPresenceDetectorPresenceDetected(presenceDetected) != ksfTkErrOk)
        return 5;

    if (getPresenceDetectorPresenceStickyDetected(presenceDetectedSticky) != ksfTkErrOk)
        return 6;

    if (presenceDetected == 1 || presenceDetectedSticky == 1)
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
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_VERSION, (uint8_t *)&regVal, sizeof(regVal), readBytes);

    // Mask unused bits from register
    major = (regVal & SFE_XM125_PRESENCE_MAJOR_VERSION_MASK) >> SFE_XM125_PRESENCE_MAJOR_VERSION_MASK_SHIFT;
    minor = (regVal & SFE_XM125_PRESENCE_MINOR_VERSION_MASK) >> SFE_XM125_PRESENCE_MINOR_VERSION_MASK_SHIFT;
    patch = regVal & SFE_XM125_PRESENCE_PATCH_VERSION_MASK;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorError(uint32_t &error)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_PROTOCOL_STATUS, (uint8_t *)&error, sizeof(uint32_t),
                                         readBytes);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceMeasureCounter(uint32_t &counter)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_MEASURE_COUNTER, (uint8_t *)&counter, sizeof(uint32_t),
                                         readBytes);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorStatus(uint32_t &status)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t *)&status, sizeof(uint32_t),
                                         readBytes);
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorErrorStatus(uint32_t &status)
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t *)&regVal, sizeof(uint32_t),
                                           readBytes);
    regVal = sftk_byte_swap(regVal);

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
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t *)&regVal, sizeof(uint32_t), readBytes);

    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register
    detected = (regVal & SFE_XM125_PRESENCE_DETECTED_MASK);

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorPresenceStickyDetected(uint32_t &sticky)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t *)&regVal, sizeof(uint32_t), readBytes);

    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register
    sticky = (regVal & SFE_XM125_PRESENCE_DETECTED_STICKY_MASK) >> 1;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectorRegError(uint32_t &error)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESULT, (uint8_t *)&regVal, sizeof(uint32_t), readBytes);

    regVal = sftk_byte_swap(regVal);

    // Mask unused bits from register
    error = (regVal & SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK) >> SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK_SHIFT;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceTemperature(uint32_t &temp)
{
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_DISTANCE, (uint8_t *)&regVal, sizeof(uint32_t), readBytes);

    // Mask unused bits from register
    temp = (regVal & SFE_XM125_PRESENCE_TEMPERATURE_MASK) >> SFE_XM125_PRESENCE_TEMPERATURE_MASK_SHIFT;

    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDistance(uint32_t &distance)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_DISTANCE, (uint8_t *)&distance, sizeof(uint32_t), readBytes);
    distance = sftk_byte_swap(distance);
    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraPresenceScore(uint32_t &intra)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t *)&intra, sizeof(uint32_t), readBytes);
    intra = sftk_byte_swap(intra);
    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterPresenceScore(uint32_t &inter)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_INTER_PRESENCE, (uint8_t *)&inter, sizeof(uint32_t), readBytes);
    inter = sftk_byte_swap(inter);
    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceSweepsPerFrame(uint32_t &sweeps)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_INTRA_PRESENCE_SCORE, (uint8_t *)&sweeps, sizeof(uint32_t), readBytes);
    sweeps = sftk_byte_swap(sweeps);
    return retVal;
}
//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceSweepsPerFrame(uint32_t sweeps)
{
    sweeps = sftk_byte_swap(sweeps);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_SWEEPS_PER_FRAME, (uint8_t *)&sweeps, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFramePresenceTimeout(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t *)&time,
                                                       sizeof(uint32_t), readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFramePresenceTimeout(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT, (uint8_t *)&time, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterPhaseBoostEnabled(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t *)&value,
                                                       sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterPhaseBoostEnabled(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED, (uint8_t *)&value,
                                          sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraDetectionEnabled(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t *)&value,
                                                       sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterDetectionEnabled(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED, (uint8_t *)&value,
                                          sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceFrameRate(uint32_t &rate)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t *)&rate, sizeof(uint32_t), readBytes);
    rate = sftk_byte_swap(rate);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceFrameRate(uint32_t rate)
{
    rate = sftk_byte_swap(rate);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_FRAME_RATE, (uint8_t *)&rate, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraDetectionThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t *)&thresh,
                                                       sizeof(uint32_t), readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceIntraDetectionThreshold(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD, (uint8_t *)&thresh,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterDetectionThreshold(uint32_t &thresh)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, (uint8_t *)&thresh,
                                                       sizeof(uint32_t), readBytes);
    thresh = sftk_byte_swap(thresh);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterDetectionThreshold(uint32_t thresh)
{
    thresh = sftk_byte_swap(thresh);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD, (uint8_t *)&thresh,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFrameDeviationTime(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t *)&time,
                                                       sizeof(uint32_t), readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFrameDeviationTime(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION, (uint8_t *)&time, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFrameFastCutoff(uint32_t &cut)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t *)&cut,
                                                       sizeof(uint32_t), readBytes);
    cut = sftk_byte_swap(cut);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFrameFastCutoff(uint32_t cut)
{
    cut = sftk_byte_swap(cut);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF, (uint8_t *)&cut,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterFrameSlowCutoff(uint32_t &cut)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t *)&cut,
                                                       sizeof(uint32_t), readBytes);
    cut = sftk_byte_swap(cut);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterFrameSlowCutoff(uint32_t cut)
{
    cut = sftk_byte_swap(cut);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF, (uint8_t *)&cut,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraFrameTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t *)&time,
                                                       sizeof(uint32_t), readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceIntraFrameTimeConst(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST, (uint8_t *)&time,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceIntraOutputTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t *)&time,
                                                       sizeof(uint32_t), readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceIntraOutputTimeConst(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST, (uint8_t *)&time,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceInterOutputTimeConst(uint32_t &time)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t *)&time,
                                                       sizeof(uint32_t), readBytes);
    time = sftk_byte_swap(time);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceInterOutputTimeConst(uint32_t time)
{
    time = sftk_byte_swap(time);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST, (uint8_t *)&time,
                                          sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceAutoProfileEn(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t *)&value,
                                                       sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceAutoProfileEn(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED, (uint8_t *)&value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceAutoStepLengthEn(bool &en)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t *)&value,
                                                       sizeof(uint8_t), readBytes);
    en = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceAutoStepLengthEn(bool en)
{
    uint8_t value = en ? 1 : 0;
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED, (uint8_t *)&value,
                                          sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceManualProfile(uint32_t &prof)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t *)&prof, sizeof(uint32_t), readBytes);
    prof = sftk_byte_swap(prof);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceManualProfile(uint32_t prof)
{
    prof = sftk_byte_swap(prof);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_MANUAL_PROFILE, (uint8_t *)&prof, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceManualStepLength(uint32_t &length)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t *)&length,
                                                       sizeof(uint32_t), readBytes);
    length = sftk_byte_swap(length);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceManualStepLength(uint32_t length)
{
    length = sftk_byte_swap(length);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH, (uint8_t *)&length, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceStart(uint32_t &start)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_START, (uint8_t *)&start, sizeof(uint32_t), readBytes);
    start = sftk_byte_swap(start);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceStart(uint32_t start)
{
    start = sftk_byte_swap(start);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_START, (uint8_t *)&start, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceEnd(uint32_t &end)
{
    size_t readBytes = 0;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_END, (uint8_t *)&end, sizeof(uint32_t), readBytes);
    end = sftk_byte_swap(end);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceEnd(uint32_t end)
{
    end = sftk_byte_swap(end);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_END, (uint8_t *)&end, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceResetFilters(bool &reset)
{
    size_t readBytes = 0;
    uint8_t value;
    sfTkError_t retVal =
        _theBus->readRegister16Region(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &value, sizeof(uint8_t), readBytes);
    reset = (value != 0);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceResetFilters(bool reset)
{
    uint8_t value = reset ? 1 : 0;
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE, &value, sizeof(uint8_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceHWAAS(uint32_t &avg)
{
    size_t readBytes = 0;
    return _theBus->readRegister16Region(SFE_XM125_PRESENCE_HWAAS, (uint8_t *)&avg, sizeof(uint32_t), readBytes);
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceHWAAS(uint32_t avg)
{
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_HWAAS, (uint8_t *)&avg, sizeof(uint32_t));
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::getPresenceDetectionOnGPIO(uint32_t &detected)
{
    size_t readBytes = 0;
    sfTkError_t retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, (uint8_t *)&detected,
                                                       sizeof(uint32_t), readBytes);
    detected = sftk_byte_swap(detected);
    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::setPresenceDetectionOnGPIO(uint32_t detected)
{
    detected = sftk_byte_swap(detected);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_DETECTION_ON_GPIO, (uint8_t *)&detected, sizeof(uint32_t));
}

sfTkError_t sfDevXM125Presence::setPresenceCommand(uint32_t cmd)
{
    cmd = sftk_byte_swap(cmd);
    return _theBus->writeRegister16Region(SFE_XM125_PRESENCE_COMMAND, (uint8_t *)&cmd, sizeof(uint32_t));
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
    sfTkError_t retVal;
    uint32_t regVal = 0;

    // Read from 16-Bit Register
    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t *)&regVal, sizeof(uint32_t),
                                           readBytes);

    regVal = sftk_byte_swap(regVal);
    // Mask unused bits from register
    busy = (regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> SFE_XM125_PRESENCE_BUSY_MASK_SHIFT;

    return retVal;
}

//--------------------------------------------------------------------------------
sfTkError_t sfDevXM125Presence::presenceBusyWait()
{
    sfTkError_t retVal = 0;
    uint32_t regVal = 0;

    size_t readBytes = 0;
    retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t *)&regVal, sizeof(uint32_t),
                                           readBytes);
    regVal = sftk_byte_swap(regVal);

    // Poll Detector Status until Busy bit is cleared
    while (((regVal & SFE_XM125_PRESENCE_BUSY_MASK) >> 30) != 0)
    {
        retVal = _theBus->readRegister16Region(SFE_XM125_PRESENCE_DETECTOR_STATUS, (uint8_t *)&regVal, sizeof(uint32_t),
                                               readBytes);
        regVal = sftk_byte_swap(regVal);
    }

    return retVal;
}