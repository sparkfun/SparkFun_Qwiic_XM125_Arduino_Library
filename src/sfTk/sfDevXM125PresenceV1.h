/**
 * @file sfDevXM125Presence.h
 * @brief Header of the SparkFun Qwiic XM125  Library.
 *
 * This file contains the header the Presence Application object
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "sfDevXM125Presence.h"


// Presence class definition

class sfDevXM125PresenceV1 : public sfDevXM125Presence
{
  public:
    /**
     * @brief Initializes the Presence detector device.
     *
     * This function sets up the I2C communication and performs initial checks
     * to ensure the device is ready for operation.
     *
     * @param theBus Pointer to the I2C bus object. If nullptr, the default bus is used.
     * @return ksfTkErrOk on success, or error code (value < -1) on failure.
     */
    sfTkError_t begin(sfTkII2C *theBus = nullptr) 
    {
        return sfDevXM125Presence::begin(theBus);
    }
    /// @brief This function sets all the beginning values for a basic I2C
    ///  example to be run on the device for presence sensing.
    /// @param start Start value for presence sensing in mm  - default value is 1000
    /// @param end End value for presence sensing in mm - default value is 5000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceDetectorStart(uint32_t start = 1000, uint32_t end = 5000)
    {
        return sfDevXM125Presence::detectorStart(start, end);
    }

    /// @brief This function returns the presence value of the register
    ///  with all the checks in place as per the I2C Datasheet.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDistanceValuemm(uint32_t &presenceVal)
    {
        return sfDevXM125Presence::getDistanceValuemm(presenceVal);
    }

    /// @brief This function returns the RSS version number
    /// @param version Version number
    /// @param patch Patch version number
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
    {
        return sfDevXM125Presence::getDetectorVersion(major, minor, patch);
    }

    /// @brief This function returns the protocol state error
    /// @param state Error code for device
    ///   0 = Protocol state error
    ///   1 = Packet length error
    ///   2 = Address error
    ///   3 = Write failed
    ///   4 = Write to read only
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorError(uint32_t &error)
    {
        return sfDevXM125Presence::getDetectorError(error);
    }

    /// @brief This function returns the measure counter; the number of
    ///  measurements performed since restart.
    /// @param counter number of measurements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceMeasureCounter(uint32_t &counter)
    {
        return sfDevXM125Presence::getMeasureCounter(counter);
    }

    /// @brief This function returns the detector status flags. See page 17, section
    ///   6.2.4 Detector Status
    /// @param status Status detector flag
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorStatus(uint32_t &status)
    {
        return sfDevXM125Presence::getDetectorStatus(status);
    }

    /// @brief This function returns the error status according to the bit
    ///  mask value for the presence devices errors and busy bit
    /// @param status Error status of device (see function for exact error)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorErrorStatus(uint32_t &status)
    {
        return sfDevXM125Presence::getDetectorErrorStatus(status);
    }

    /// @brief This function returns if there was presence detected
    /// @param detected Presence Detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorPresenceDetected(uint32_t &detected)
    {
        return sfDevXM125Presence::getDetectorPresenceDetected(detected);
    }  

    /// @brief This function returns if there was presence detected, sticky bit with
    ///  clear on read.
    /// @param sticky Sticky Presence Detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorPresenceStickyDetected(uint32_t &sticky)
    {
        return sfDevXM125Presence::getDetectorPresenceStickyDetected(sticky);
    }

    /// @brief This function returns the indication that the presence detector
    ///  failed.
    /// @param calibrate Indication of sensor calibration
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectorRegError(uint32_t &error)
    {
        return sfDevXM125Presence::getDetectorRegError(error);
    }

    /// @brief This function returns the temperature in sensor during measurement (in
    ///   degree Celsius). Note that it has poor absolute accuracy and should only
    ///   be used for relative temperature measurements.
    /// @param temp Relative temperature
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceTemperature(uint32_t &temp)
    {
        return sfDevXM125Presence::getTemperature(temp);
    }

    /// @brief This function returns the distance, in millimeters, for the detected
    ///  presence.
    /// @param distance distance in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDistance(uint32_t &distance)
    {
        return sfDevXM125Presence::getDistance(distance);
    }

    /// @brief This function returns the measure of amount of fast motion detected.
    /// @param intra Measure of fast motion
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceIntraPresenceScore(uint32_t &intra)
    {
        return sfDevXM125Presence::getIntraPresenceScore(intra);
    }

    /// @brief This function returns the measure of amount of slow motion detected.
    /// @param inter Measure of slow motion
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterPresenceScore(uint32_t &inter)
    {
        return sfDevXM125Presence::getInterPresenceScore(inter);
    }

    /// @brief This function returns the number of sweeps that will be
    ///   captured in each frame (measurement).
    ///   Default Value: 16 seconds
    /// @param sweeps Number of sweeps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceSweepsPerFrame(uint32_t &sweeps)
    {
        return sfDevXM125Presence::getSweepsPerFrame(sweeps);
    }

    /// @brief This function sets the number of sweeps that will be
    ///   captured in each frame (measurement).
    ///   Default Value: 16 seconds
    /// @param sweeps Number of sweeps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceSweepsPerFrame(uint32_t sweeps)
    {
        return sfDevXM125Presence::setSweepsPerFrame(sweeps);
    }

    /// @brief Returns the number of seconds the inter-frame presence score needs to decrease before
    ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
    ///  means no timeout.
    ///   Default Value: 3 seconds
    /// @param time Number of seconds
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterFramePresenceTimeout(uint32_t &time)
    {
        return sfDevXM125Presence::getInterFramePresenceTimeout(time);
    }

    /// @brief Sets the number of seconds the inter-frame presence score needs to decrease before
    ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
    ///  means no timeout.
    ///   Default Value: 3 seconds
    /// @param time Number of seconds
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterFramePresenceTimeout(uint32_t time)
    {
        return sfDevXM125Presence::setInterFramePresenceTimeout(time);
    }

    /// @brief This function returns the enable to increase detection of slow
    ///  motions by utilizing the phase information in the Sparse IQ data
    /// @param en Enables increased detection
    ///   Default Value: false
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterPhaseBoostEnabled(bool &en)
    {
        return sfDevXM125Presence::getInterPhaseBoostEnabled(en);
    }

    /// @brief This function sets the enable to increase detection of slow
    ///  motions by utilizing the phase information in the Sparse IQ data
    /// @param en Enables increased detection
    ///   Default Value: false
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterPhaseBoostEnabled(bool en)
    {
        return sfDevXM125Presence::setInterPhaseBoostEnabled(en);
    }

    /// @brief This function returns the bit to enable to detect faster
    ///  movements inside frames.
    ///   Default Value: true
    /// @param en Enables faster detection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceIntraDetectionEnabled(bool &en)
    {
        return sfDevXM125Presence::getIntraDetectionEnabled(en);
    }

    /// @brief This function returns the bit to enable to detect slower
    ///  movements between frames.
    ///   Default Value: true
    /// @param en Enables faster detection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterDetectionEnabled(bool en)
    {
        return sfDevXM125Presence::setInterDetectionEnabled(en);
    }

    /// @brief This function returns the presence detector frame rate.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 12000 mHz
    /// @param rate Frame rate, in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceFrameRate(uint32_t &rate)
    {
        return sfDevXM125Presence::getFrameRate(rate);
    }

    /// @brief This function sets the presence detector frame rate.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 12000 mHz
    /// @param rate Frame rate, in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceFrameRate(uint32_t rate)
    {
        return sfDevXM125Presence::setFrameRate(rate);
    }

    /// @brief This function returns the threshold for detecting faster
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1300
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceIntraDetectionThreshold(uint32_t &thresh)
    {
        return sfDevXM125Presence::getIntraDetectionThreshold(thresh);
    }

    /// @brief This function sets the threshold for detecting faster
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1300
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceIntraDetectionThreshold(uint32_t thresh)
    {
        return sfDevXM125Presence::setIntraDetectionThreshold(thresh);
    }

    /// @brief This function returns the threshold for detecting slower
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1000
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterDetectionThreshold(uint32_t &thresh)
    {
        return sfDevXM125Presence::getInterDetectionThreshold(thresh);
    }

    /// @brief This function sets the threshold for detecting slower
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1000
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterDetectionThreshold(uint32_t thresh)
    {
        return sfDevXM125Presence::setInterDetectionThreshold(thresh);
    }

    /// @brief This function returns the time constant of the low pass
    ///  filter for the inter-frame deviation between fast and slow.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 500 ms
    /// @param time time in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterFrameDeviationTime(uint32_t &time)
    {
        return sfDevXM125Presence::getInterFrameDeviationTime(time);
    }

    /// @brief This function sets the time constant of the low pass
    ///  filter for the inter-frame deviation between fast and slow.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 500 ms
    /// @param time time in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterFrameDeviationTime(uint32_t time)
    {
        return sfDevXM125Presence::setInterFrameDeviationTime(time);
    }

    /// @brief This function returns the cutoff frequency of the low pass
    ///  filter for the fast filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 6000 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterFrameFastCutoff(uint32_t &cut)
    {
        return sfDevXM125Presence::getInterFrameFastCutoff(cut);
    }

    /// @brief This function sets the cutoff frequency of the low pass
    ///  filter for the fast filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 6000 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterFrameFastCutoff(uint32_t cut)
    {
        return sfDevXM125Presence::setInterFrameFastCutoff(cut);
    }

    /// @brief This function returns the cutoff frequency of the low pass
    ///  filter for the slow filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 200 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterFrameSlowCutoff(uint32_t &cut)
    {
        return sfDevXM125Presence::getInterFrameSlowCutoff(cut);
    }

    /// @brief This function sets the cutoff frequency of the low pass
    ///  filter for the slow filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 200 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterFrameSlowCutoff(uint32_t cut)
    {
        return sfDevXM125Presence::setInterFrameSlowCutoff(cut);
    }

    /// @brief This functions returns time constant for the depthwise
    ///  filtering in the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 150ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceIntraFrameTimeConst(uint32_t &time)
    {
        return sfDevXM125Presence::getIntraFrameTimeConst(time);
    }

    /// @brief This functions sets time constant for the depthwise
    ///  filtering in the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 150ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceIntraFrameTimeConst(uint32_t time)
    {
        return sfDevXM125Presence::setIntraFrameTimeConst(time);
    }

    /// @brief This function returns time constant for the output in
    ///  the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 300ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceIntraOutputTimeConst(uint32_t &time)
    {
        return sfDevXM125Presence::getIntraOutputTimeConst(time);
    }

    /// @brief This function sets the time constant for the output in
    ///  the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 300ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceIntraOutputTimeConst(uint32_t time)
    {
        return sfDevXM125Presence::setIntraOutputTimeConst(time);
    }

    /// @brief This function returns time constant for the output in
    ///  the inter-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2000ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceInterOutputTimeConst(uint32_t &time)
    {
        return sfDevXM125Presence::getInterOutputTimeConst(time);
    }

    /// @brief This function sets the time constant for the output in
    ///  the inter-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2000ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceInterOutputTimeConst(uint32_t time)
    {
        return sfDevXM125Presence::setInterOutputTimeConst(time);
    }

    /// @brief This function returns if enable/disable for the automatic
    ///  selection of profile based on start point of measurement.
    ///   Default Value: true
    /// @param en enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceAutoProfileEn(bool &en)
    {
        return sfDevXM125Presence::getAutoProfileEn(en);
    }

    /// @brief This function enables/disables the automatic selection
    ///  of profile based on start point of measurement.
    ///   Default Value: true
    /// @param en enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceAutoProfileEn(bool en)
    {
        return sfDevXM125Presence::setAutoProfileEn(en);
    }

    /// @brief This function returns the Enable/Disable
    ///  automatic selection of step length based on the profile.
    /// @param en Enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceAutoStepLengthEn(bool &en)
    {
        return sfDevXM125Presence::getAutoStepLengthEn(en);
    }

    /// @brief This function Enables/Disables automatic selection
    ///  of step length based on the profile.
    /// @param en Enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceAutoStepLengthEn(bool en)
    {
        return sfDevXM125Presence::setAutoStepLengthEn(en);
    }

    /// @brief This function returns the profile in use. The profile will only
    ///  be used if profile auto selection was disabled.
    /// @param prof Profile to use (enum)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceManualProfile(uint32_t &prof)
    {
        return sfDevXM125Presence::getManualProfile(prof);
    }

    /// @brief This function sets the profile to use. The profile will only
    ///  be used if profile auto selection was disabled.
    /// @param prof Profile to use (enum)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceManualProfile(uint32_t prof)
    {
        return sfDevXM125Presence::setManualProfile(prof);
    }

    /// @brief THis function returns the number of steps between each data point.
    ///  NOTE: The manual step length will only be used if step length auto selection
    ///  was disabled
    ///   Default Value: 72
    /// @param length number of steps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceManualStepLength(uint32_t &length)
    {
        return sfDevXM125Presence::getManualStepLength(length);
    }

    /// @brief THis function returns the number of steps between each data point.
    ///  NOTE: The manual step length will only be used if step length auto selection
    ///  was disabled
    ///   Default Value: 72
    /// @param length number of steps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceManualStepLength(uint32_t length)
    {
        return sfDevXM125Presence::setManualStepLength(length);
    }

    /// @brief This function returns the start of measured interval in mm.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 300mm
    /// @param start measured start interval in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceStart(uint32_t &start)
    {
        return sfDevXM125Presence::getStart(start);
    }

    /// @brief This function sets the start of measured interval in mm.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 300mm
    /// @param start measured start interval in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceStart(uint32_t start)
    {
        return sfDevXM125Presence::setStart(start);
    }

    /// @brief This function returns the end point of measurement
    ///  interval in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2500
    /// @param end End point of measurement in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceEnd(uint32_t &end)
    {
        return sfDevXM125Presence::getEnd(end);
    }

    /// @brief This function returns the end point of measurement
    ///  interval in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2500
    /// @param end End point of measurement in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceEnd(uint32_t end)
    {
        return sfDevXM125Presence::setEnd(end);
    }

    /// @brief This function returns if the device is enabled/disabled
    ///  for the reset of the presence filters during start/restart.
    ///   Default Value: true
    /// @param reset enabled/disabled
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceResetFilters(bool &reset)
    {
        return sfDevXM125Presence::getResetFilters(reset);
    }

    /// @brief This function sets if the device is enabled/disabled
    ///  for the reset of the presence filters during start/restart .
    ///   Default Value: true
    /// @param reset enabled/disabled
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceResetFilters(bool reset)
    {
        return sfDevXM125Presence::setResetFilters(reset);
    }

    /// @brief This function returns the hardware accelerated average
    ///  sampled (HWAAS).
    ///   Default Value: 32
    /// @param avg average samples
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceHWAAS(uint32_t &avg)
    {
        return sfDevXM125Presence::getHWAAS(avg);
    }

    /// @brief This function sets the hardware accelerated average
    ///  sampled (HWAAS).
    ///   Default Value: 32
    /// @param avg average samples
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceHWAAS(uint32_t avg)
    {
        return sfDevXM125Presence::setHWAAS(avg);
    }

    /// @brief This function returns the output presence detection on
    ///  generic GPIO.
    /// @param detected Output presence detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceDetectionOnGPIO(uint32_t &detected)
    {
        return sfDevXM125Presence::getDetectionOnGPIO(detected);
    }

    /// @brief This function sets the output presence detection on
    ///  generic GPIO.
    /// @param detected Output presence detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceDetectionOnGPIO(uint32_t detected)
    {
        return sfDevXM125Presence::setDetectionOnGPIO(detected);
    }

    /// @brief This function sets the execute command
    /// @param cmd command to send to device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setPresenceCommand(uint32_t cmd)
    {
        return sfDevXM125Presence::setCommand(cmd);
    }

    /// @brief This function applies the configuration to the device by
    ///  writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceApplyConfiguration()
    {
        return sfDevXM125Presence::applyConfiguration();
    }

    /// @brief This function starts the device by writing the defined
    ///  start value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceStart()
    {
        return sfDevXM125Presence::start();
    }

    /// @brief This function stops the device by writing the defined
    ///  stop value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceStop()
    {
        return sfDevXM125Presence::stop();
    }

    /// @brief This function enables the uart logs of the device by
    ///  writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceEnableUartLogs()
    {
        return sfDevXM125Presence::enableUartLogs();
    }

    /// @brief This function disables the uart logs of the device by
    ///  writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceDisableUartLogs()
    {
        return sfDevXM125Presence::disableUartLogs();
    }

    /// @brief This function enables the configuration log of the device
    ///  by writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceLogConfiguration()
    {
        return sfDevXM125Presence::logConfiguration();
    }

    /// @brief This function resets the presence detector settings
    ///  of the device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceReset()
    {
        return sfDevXM125Presence::reset();
    }

    /// @brief This function returns the busy bit of the presence status register
    /// @param busy Device busy or not
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getPresenceBusy(uint32_t &busy)
    {
        return sfDevXM125Presence::getBusy(busy);
    }

    /// @brief Completes a busy wait loop while the device is uploading
    ///  information by waiting for the status. Checks the error status register
    ///  to wait until errors are completed/gone
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t presenceBusyWait()
    {
        return sfDevXM125Presence::busyWait();
    }
};