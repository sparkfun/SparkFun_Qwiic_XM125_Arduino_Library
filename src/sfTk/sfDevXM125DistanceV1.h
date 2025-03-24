/**
 * @file sfDevXM125DistanceV1.h
 * @brief Header of the SparkFun Qwiic XM125  Library.
 *
 * This file contains the header the Distance Application V1 API backward compatiblity 
 * object
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "sfDevXM125Distance.h"

// Distance class definition

class sfDevXM125DistanceV1 : public sfDevXM125Distance
{
  public:
    /**
     * @brief Initializes the distance detector device.
     *
     * This function sets up the I2C communication and performs initial checks
     * to ensure the device is ready for operation.
     *
     * @param theBus Pointer to the I2C bus object. If nullptr, the default bus is used.
     * @return ksfTkErrOk on success, or error code (value < -1) on failure.
     */
    sfTkError_t begin(sfTkII2C *theBus = nullptr)
    {
        return sfDevXM125Distance::begin(theBus);
    }

    /// @brief This function sets all the beginning values for a basic I2C
    ///  example to be run on the device for presence sensing.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceBegin(uint32_t start = sfe_xm125_distance_start_default,
                              uint32_t end = sfe_xm125_distance_end_default)
    {
        return sfDevXM125Distance::distanceSetup(start, end);
}

    /// @brief This function does all the required checks and busy waits to
    ///  make sure the device is ready for distance readings.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceDetectorReadingSetup()
    {
        return sfDevXM125Distance::detectorReadingSetup();
    }

    /// @brief This function returns the version number of the device
    ///  structure: major.minor.patch
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch)
    {
        return sfDevXM125Distance::getDetectorVersion(major, minor, patch);
    }

    /// @brief This function returns if there was an error from the
    ///  protocol status register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorError(uint32_t &error)
    {
        return sfDevXM125Distance::getDetectorError(error);
    }

    /// @brief This function returns the error status according to the bit
    ///  mask value for the distance devices errors and busy bit
    /// @param status Error status of device (see function for exact error)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorErrorStatus(uint32_t &status)
    {
        return sfDevXM125Distance::getDetectorErrorStatus(status);
    }

    /// @brief This function returns the measure counter, the number of measurements
    ///   performed since restart.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMeasureCounter(uint32_t &counter)
    {
        return sfDevXM125Distance::getMeasureCounter(counter);
    }

    /// @brief This function returns the status of the device if there are any issues
    ///  with any of the status's listed defined.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorStatus(uint32_t &status)
    {
        return sfDevXM125Distance::getDetectorStatus(status);
    }

    /// @brief This function returns the number of detected distances.
    /// @param distance Number of detected distances
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceNumberDistances(uint32_t &distance)
    {
        return sfDevXM125Distance::getNumberDistances(distance);
    }

    /// @brief This function returns the indication that there might be an object
    ///   near the start point of the measured range.
    /// @param edge Flag to determine object in range
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceNearStartEdge(uint32_t &edge)
    {
        return sfDevXM125Distance::getNearStartEdge(edge);
    }

    /// @brief This function returns the indication of a sensor calibration needed.
    /// @param calibrate Flag to indicate calibration required
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceCalibrationNeeded(uint32_t &calibrate)
    {
        return sfDevXM125Distance::getCalibrationNeeded(calibrate);
    }

    /// @brief This function returns if the measure command failed.
    /// @param error Flag to indicate measure command error
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMeasureDistanceError(uint32_t &error)
    {
        return sfDevXM125Distance::getMeasureDistanceError(error);
    }

    /// @brief This function returns the temperature in sensor during measurements
    ///   (in degree Celsius). Note that it has poor absolute accuracy and should
    ///   only be used for relative temperature measurements.
    /// @param temperature Relative temperature of device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceTemperature(int16_t &temperature)
    {
        return sfDevXM125Distance::getTemperature(temperature);
    }

    //--------------------------------------------------------------------------------
    // Generic distance peak distance method
    /// @brief This function returns the distance to peak num
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param num Peak number to get distance (0-9)
    /// @param peak Distance to peak num
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeakDistance(uint8_t num, uint32_t &peak)
    {
        return sfDevXM125Distance::getPeakDistance(num, peak);
    }

    /// @brief This function returns the distance to peak 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak0Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak0Distance(peak);
    }

    /// @brief This function returns the distance to peak 1
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak1Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak1Distance(peak);
    }

    /// @brief This function returns the distance to peak 2
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak2Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak2Distance(peak);
    }

    /// @brief This function returns the distance to peak 3
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak3Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak3Distance(peak);
    }

    /// @brief This function returns the distance to peak 4
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak4Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak4Distance(peak);
    }

    /// @brief This function returns the distance to peak 5
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak5Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak5Distance(peak);
    }

    /// @brief This function returns the distance to peak 6
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak6Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak6Distance(peak);
    }

    /// @brief This function returns the distance to peak 7
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak7Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak7Distance(peak);
    }

    /// @brief This function returns the distance to peak 8
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak8Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak8Distance(peak);
    }

    /// @brief This function returns the distance to peak 9
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak9Distance(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeak9Distance(peak);
    }

    //--------------------------------------------------------------------------------
    // Generic distance peak strength method
    /// @brief This function returns the strength to peak num
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param num Peak number to get strength (0-9)
    /// @param peak strength to peak num
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeakStrength(uint8_t num, int32_t &peak)
    {
        return sfDevXM125Distance::getPeakStrength(num, peak);
    }

    /// @brief This function returns the strength of peak 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak0Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak0Strength(peak);
    }

    /// @brief This function returns the strength of peak 1
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak1Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak1Strength(peak);
    }

    /// @brief This function returns the strength of peak 2
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak2Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak2Strength(peak);
    }

    /// @brief This function returns the strength of peak 3
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak3Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak3Strength(peak);
    }

    /// @brief This function returns the strength of peak 4
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak4Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak4Strength(peak);
    }

    /// @brief This function returns the strength of peak 5
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak5Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak5Strength(peak);
    }

    /// @brief This function returns the strength of peak 6
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak6Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak6Strength(peak);
    }

    /// @brief This function returns the strength of peak 7
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak7Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak7Strength(peak);
    }

    /// @brief This function returns the strength of peak 8
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak8Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak8Strength(peak);
    }

    /// @brief This function returns the strength of peak 9
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak9Strength(int32_t &peak)
    {
        return sfDevXM125Distance::getPeak9Strength(peak);
    }

    /// @brief This function returns the start of measured interval
    ///  in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 250
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceStart(uint32_t &startVal)
    {
        return sfDevXM125Distance::getStart(startVal);
    }

    /// @brief This function sets the start of measured interval in
    ///  millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 250
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceStart(uint32_t start)
    {
        return sfDevXM125Distance::setStart(start);
    }

    /// @brief This function returns the end of measured interval
    ///  in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 3000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceEnd(uint32_t &end)
    {
        return sfDevXM125Distance::getEnd(end);
    }

    /// @brief This function sets the end of measured interval
    ///  in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 3000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceEnd(uint32_t end)
    {
        return sfDevXM125Distance::setEnd(end);
    }

    /// @brief This function returns the used to limit step length.
    ///  If set to 0 (default), the step length is calculated
    ///  based on profile.
    ///  Default Value: 0
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMaxStepLength(uint32_t &length)
    {
        return sfDevXM125Distance::getMaxStepLength(length);
    }

    /// @brief This function sets the used to limit step length.
    ///  If set to 0 (default), the step length is calculated
    ///  based on profile.
    ///  Default Value: 0
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceMaxStepLength(uint32_t length)
    {
        return sfDevXM125Distance::setMaxStepLength(length);
    }

    /// @brief This function reads if the close range leakage
    ///  cancellation logic is enabled.
    ///  Default Value: true
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceCloseRangeLeakageCancellation(bool &range)
    {
        return sfDevXM125Distance::getCloseRangeLeakageCancellation(range);
    }

    /// @brief This function sets the close range leakage
    ///  cancellation logic.
    ///  Default Value: true
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceCloseRangeLeakageCancellation(bool range)
    {
        return sfDevXM125Distance::setCloseRangeLeakageCancellation(range);
    }

    /// @brief This function returns the high signal quality in a
    ///  better SNR (because of higher HWAAS) and higher power consumption.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 15000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceSignalQuality(uint32_t &signal)
    {
        return sfDevXM125Distance::getSignalQuality(signal);
    }

    /// @brief This function sets the high signal quality in a
    ///  better SNR (because of higher HWAAS) and higher power consumption.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 15000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceSignalQuality(uint32_t signal)
    {
        return sfDevXM125Distance::setSignalQuality(signal);
    }

    /// @brief This function returns the max profile of the device.
    ///  Default value = PROFILE5
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMaxProfile(uint32_t &profile)
    {
        return sfDevXM125Distance::getMaxProfile(profile);
    }

    /// @brief This function sets the max profile of the device
    ///  Default value = PROFILE5
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceMaxProfile(uint32_t profile)
    {
        return sfDevXM125Distance::setMaxProfile(profile);
    }

    /// @brief This function returns the threshold method
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceThresholdMethod(uint32_t &method)
    {
        return sfDevXM125Distance::getThresholdMethod(method);
    }

    /// @brief This function sets the threshold method
    /// @param method Threshold method (enum)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceThresholdMethod(uint32_t method)
    {
        return sfDevXM125Distance::setThresholdMethod(method);
    }

    /// @brief This function returns the peak sorting method
    /// @param peak Peak sorting method
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeakSorting(uint32_t &peak)
    {
        return sfDevXM125Distance::getPeakSorting(peak);
    }

    /// @brief This function sets the peak sorting method
    /// @param peak Peak sorting method
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistancePeakSorting(uint32_t peak)
    {
        return sfDevXM125Distance::setPeakSorting(peak);
    }

    /// @brief This function returns the number frames to use for recorded threshold.
    ///  Default Value: 100
    /// @param thresh Number of frames
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceNumFramesRecordedThreshold(uint32_t &thresh)
    {
        return sfDevXM125Distance::getNumFramesRecordedThreshold(thresh);
    }

    /// @brief This function sets the number frames to use for recorded threshold.
    ///  Default Value: 100
    /// @param thresh Number of frames
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceNumFramesRecordedThreshold(uint32_t thresh)
    {
        return sfDevXM125Distance::setNumFramesRecordedThreshold(thresh);
    }

    /// @brief This function returns the fixed amplitude threshold value.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 100000
    /// @param thresh Fixed amplitude threshold value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceFixedAmpThreshold(uint32_t &thresh)
    {
        return sfDevXM125Distance::getFixedAmpThreshold(thresh);
    }

    /// @brief This function sets the fixed amplitude threshold value.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 100000
    /// @param thresh Fixed amplitude threshold value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceFixedAmpThreshold(uint32_t thresh)
    {
        return sfDevXM125Distance::setFixedAmpThreshold(thresh);
    }

    /// @brief This function returns the threshold sensitivity
    ///  (0 <= sensitivity <= 1000)
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 500
    /// @param thresh& Threshold sensitivity
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceThresholdSensitivity(uint32_t &thresh)
    {
        return sfDevXM125Distance::getThresholdSensitivity(thresh);
    }

    /// @brief This function sets the threshold sensitivity
    ///  (0 <= sensitivity <= 1000)
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 500
    /// @param thresh Threshold sensitivity
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceThresholdSensitivity(uint32_t thresh)
    {
        return sfDevXM125Distance::setThresholdSensitivity(thresh);
    }

    /// @brief This function returns the reflector shape
    ///   Default Value: GENERIC
    /// @param shape Generic or planar reflection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceReflectorShape(uint32_t &shape)
    {
        return sfDevXM125Distance::getReflectorShape(shape);
    }

    /// @brief This function sets the reflector shape
    ///   Default Value: GENERIC
    /// @param shape Generic or planar reflection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceReflectorShape(uint32_t shape)
    {
        return sfDevXM125Distance::setReflectorShape(shape);
    }

    /// @brief This function returns the fixed strength threshold value.
    ///  Default Value: 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param thresh fixed threshold strength value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceFixedStrengthThresholdValue(int32_t &thresh)
    {
        return sfDevXM125Distance::getFixedStrengthThresholdValue(thresh);
    }

    /// @brief This function sets the fixed strength threshold value.
    ///  Default Value: 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param thresh fixed threshold strength value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceFixedStrengthThresholdValue(int32_t thresh)
    {
        return sfDevXM125Distance::setFixedStrengthThresholdValue(thresh);
    }

    /// @brief This function returns the measure on wakeup status.
    ///   Default Value: false
    /// @param measure Measure on wakeup occurrence
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMeasureOneWakeup(bool &measure)
    {
        return sfDevXM125Distance::getMeasureOneWakeup(measure);
    }

    /// @brief This function sets the measure on wakeup status.
    ///   Default Value: false
    /// @param measure Measure on wakeup occurrence
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceMeasureOneWakeup(bool measure)
    {
        return sfDevXM125Distance::setMeasureOneWakeup(measure);
    }

    /// @brief This function sets the specific execute command as defined in the
    ///   datasheet on Page 25, Section 6.2.40 Command.
    /// @param apply Enable configuration and calibrate
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceCommand(uint32_t command)
    {
        return sfDevXM125Distance::setCommand(command);
    }

    /// @brief This function applies the configuration to the device by
    ///  writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceApplyConfiguration()
    {
        return sfDevXM125Distance::applyConfiguration();
    }

    /// @brief This function starts the device by writing the defined
    ///  start value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceStart()
    {
        return sfDevXM125Distance::start();
    }

    /// @brief This function stops the device by writing the defined
    ///  stop value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceStop()
    {
        return sfDevXM125Distance::stop();
    }

    /// @brief This function calibrates the device by writing the defined
    ///  calibration value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceCalibrate()
    {
        return sfDevXM125Distance::calibrate();
    }

    /// @brief This function recalibrates the device by writing the defined
    ///  recalibrate value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceRecalibrate()
    {
        return sfDevXM125Distance::recalibrate();
    }

    /// @brief This function enables the uart logs of the device by
    ///  writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceEnableUartLogs()
    {
        return sfDevXM125Distance::enableUartLogs();
    }

    /// @brief This function disables the uart logs of the device by
    ///  writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceDisableUartLogs()
    {
        return sfDevXM125Distance::disableUartLogs();
    }

    /// @brief This function enables the configuration log of the device
    ///  by writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceLogConfiguration()
    {
        return sfDevXM125Distance::logConfiguration();
    }

    /// @brief This function resets the distance detector settings
    ///  of the device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceReset()
    {
        return sfDevXM125Distance::reset();
    }

    /// @brief Completes a busy wait loop while the device is uploading
    ///  information by waiting for the status
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceBusyWait()
    {
        return sfDevXM125Distance::busyWait();
    }
};