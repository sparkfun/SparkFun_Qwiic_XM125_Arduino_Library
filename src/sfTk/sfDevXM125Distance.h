/**
 * @file sfDevXM125Distance.h
 * @brief Header of the SparkFun Qwiic XM125  Library.
 *
 * This file contains the header the Distance Application object
 *
 * @author SparkFun Electronics
 * @date 2024-2025
 * @copyright Copyright (c) 2024-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "sfDevXM125Core.h"

// Defines

/* ****************************** Distance Values ****************************** */

const uint32_t SFE_XM125_DISTANCE_MAJOR_VERSION_MASK = 0xffff0000;
const uint32_t SFE_XM125_DISTANCE_MINOR_VERSION_MASK = 0x0000ff00;
const uint32_t SFE_XM125_DISTANCE_PATCH_VERSION_MASK = 0x000000ff;
const uint32_t SFE_XM125_DISTANCE_NUMBER_DISTANCES_MASK = 0x0000000f;
const uint32_t SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK = 0x00000100;
const uint32_t SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK = 0x00000400;
const uint32_t SFE_XM125_DISTANCE_TEMPERATURE_MASK = 0xffff0000;
const uint32_t SFE_XM125_DISTANCE_RSS_REGISTER_OK_MASK = 0x00000001;
const uint32_t SFE_XM125_DISTANCE_CONFIG_CREATE_OK_MASK = 0x00000002;
const uint32_t SFE_XM125_DISTANCE_SENSOR_CREATE_OK_MASK = 0x00000004;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_CREATE_OK_MASK = 0x00000008;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_BUFFER_OK_MASK = 0x00000010;
const uint32_t SFE_XM125_DISTANCE_SENSOR_BUFFER_OK_MASK = 0x00000020;
const uint32_t SFE_XM125_DISTANCE_CALIBRATION_BUFFER_OK_MASK = 0x00000040;
const uint32_t SFE_XM125_DISTANCE_CONFIG_APPLY_OK_MASK = 0x00000080;
const uint32_t SFE_XM125_DISTANCE_SENSOR_CALIBRATE_OK_MASK = 0x00000100;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_OK_MASK = 0x00000200;
const uint32_t SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK = 0x00010000;
const uint32_t SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK = 0x00020000;
const uint32_t SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK = 0x00040000;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK = 0x00080000;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK = 0x00100000;
const uint32_t SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK = 0x00200000;
const uint32_t SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK = 0x00400000;
const uint32_t SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK = 0x00800000;
const uint32_t SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK = 0x01000000;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK = 0x02000000;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK = 0x10000000;
const uint32_t SFE_XM125_DISTANCE_BUSY_MASK = 0x80000000;
const uint32_t SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK = 0x00000200;

const uint32_t SFE_XM125_DISTANCE_ALL_ERROR_MASK =
    (SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK | SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK |
     SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK | SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK |
     SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK | SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK |
     SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK | SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK |
     SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK | SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK |
     SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK | SFE_XM125_DISTANCE_BUSY_MASK);

const uint32_t SFE_XM125_DISTANCE_MAJOR_VERSION_MASK_SHIFT = 16;
const uint32_t SFE_XM125_DISTANCE_MINOR_VERSION_MASK_SHIFT = 8;
const uint32_t SFE_XM125_DISTANCE_RSS_REGISTER_ERROR_MASK_SHIFT = 15;
const uint32_t SFE_XM125_DISTANCE_CONFIG_CREATE_ERROR_MASK_SHIFT = 16;
const uint32_t SFE_XM125_DISTANCE_SENSOR_CREATE_ERROR_MASK_SHIFT = 17;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_CREATE_ERROR_MASK_SHIFT = 18;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_BUFFER_ERROR_MASK_SHIFT = 19;
const uint32_t SFE_XM125_DISTANCE_SENSOR_BUFFER_ERROR_MASK_SHIFT = 20;
const uint32_t SFE_XM125_DISTANCE_CALIBRATION_BUFFER_ERROR_MASK_SHIFT = 21;
const uint32_t SFE_XM125_DISTANCE_CONFIG_APPLY_ERROR_MASK_SHIFT = 22;
const uint32_t SFE_XM125_DISTANCE_SENSOR_CALIBRATE_ERROR_MASK_SHIFT = 23;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_CALIBRATE_ERROR_MASK_SHIFT = 24;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_ERROR_MASK_SHIFT = 27;
const uint32_t SFE_XM125_DISTANCE_BUSY_MASK_SHIFT = 30;
const uint32_t SFE_XM125_DISTANCE_NEAR_START_EDGE_MASK_SHIFT = 8;
const uint32_t SFE_XM125_DISTANCE_CALIBRATION_NEEDED_MASK_SHIFT = 9;
const uint32_t SFE_XM125_DISTANCE_MEASURE_DISTANCE_ERROR_MASK_SHIFT = 10;
const uint32_t SFE_XM125_DISTANCE_TEMPERATURE_MASK_SHIFT = 16;
const uint32_t SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK_SHIFT = 30;

const uint32_t SFE_XM125_DISTANCE_DETECTOR_STATUS_MASK = 0b10010000111111110000000011111111;

const uint16_t SFE_XM125_DISTANCE_VERSION = 0x00;
typedef struct
{
    uint32_t distance_major : 16;
    uint32_t distance_minor : 8;
    uint32_t distance_patch : 8;
} sfe_xm125_distance_version_t;

const uint16_t SFE_XM125_DISTANCE_PROTOCOL_STATUS = 0x01;
typedef struct
{
    uint32_t distance_protocol_state_error : 1;
    uint32_t distance_packet_length_error : 1;
    uint32_t distance_address_error : 1;
    uint32_t distance_write_failed : 1;
    uint32_t distance_write_to_read_only : 1;
    uint32_t reserved1 : 27;
} sfe_xm125_distance_protocol_status_t;

const uint16_t SFE_XM125_DISTANCE_MEASURE_COUNTER = 0x02;

const uint16_t SFE_XM125_DISTANCE_DETECTOR_STATUS = 0x03;
typedef struct
{
    uint32_t distance_rss_register_ok : 1;
    uint32_t distance_config_create_ok : 1;
    uint32_t distance_sensor_create_ok : 1;
    uint32_t distance_detector_create_ok : 1;
    uint32_t distance_detector_buffer_ok : 1;
    uint32_t distance_sensor_buffer_ok : 1;
    uint32_t distance_calibration_buffer_ok : 1;
    uint32_t distance_config_apply_ok : 1;
    uint32_t distance_sensor_calibrate_ok : 1;
    uint32_t distance_detector_calibrate_ok : 1;
    uint32_t reserved1 : 6;
    uint32_t distance_rss_register_error : 1;
    uint32_t distance_config_create_error : 1;
    uint32_t distance_sensor_create_error : 1;
    uint32_t distance_detector_create_error : 1;
    uint32_t distance_detector_buffer_error : 1;
    uint32_t distance_sensor_buffer_error : 1;
    uint32_t distance_calibration_buffer_error : 1;
    uint32_t distance_config_apply_error : 1;
    uint32_t distance_sensor_calibrate_error : 1;
    uint32_t distance_detector_calibrate_error : 1;
    uint32_t reserved2 : 2;
    uint32_t distance_detector_error : 1;
    uint32_t reserved3 : 2;
    uint32_t distance_busy : 1;
} sfe_xm125_distance_detector_status_t;

const uint16_t SFE_XM125_DISTANCE_RESULT = 0x10;
typedef struct
{
    uint32_t distance_num_distances : 4;
    uint32_t rsvd1 : 4; // skips "reserved" bits
    uint32_t distance_near_start_edge : 1;
    uint32_t distance_calibration_needed : 1;
    uint32_t distance_measure_distance_error : 1;
    uint32_t reserved1 : 5;
    uint32_t distance_temperature : 16;
} sfe_xm125_distance_result_t;

const uint16_t SFE_XM125_DISTANCE_PEAK0_DISTANCE = 0x11;
const uint16_t SFE_XM125_DISTANCE_PEAK1_DISTANCE = 0x12;
const uint16_t SFE_XM125_DISTANCE_PEAK2_DISTANCE = 0x13;
const uint16_t SFE_XM125_DISTANCE_PEAK3_DISTANCE = 0x14;
const uint16_t SFE_XM125_DISTANCE_PEAK4_DISTANCE = 0x15;
const uint16_t SFE_XM125_DISTANCE_PEAK5_DISTANCE = 0x16;
const uint16_t SFE_XM125_DISTANCE_PEAK6_DISTANCE = 0x17;
const uint16_t SFE_XM125_DISTANCE_PEAK7_DISTANCE = 0x18;
const uint16_t SFE_XM125_DISTANCE_PEAK8_DISTANCE = 0x19;
const uint16_t SFE_XM125_DISTANCE_PEAK9_DISTANCE = 0x1a;
const uint16_t SFE_XM125_DISTANCE_PEAK0_STRENGTH = 0x1b;
const uint16_t SFE_XM125_DISTANCE_PEAK1_STRENGTH = 0x1c;
const uint16_t SFE_XM125_DISTANCE_PEAK2_STRENGTH = 0x1d;
const uint16_t SFE_XM125_DISTANCE_PEAK3_STRENGTH = 0x1e;
const uint16_t SFE_XM125_DISTANCE_PEAK4_STRENGTH = 0x1f;
const uint16_t SFE_XM125_DISTANCE_PEAK5_STRENGTH = 0x20;
const uint16_t SFE_XM125_DISTANCE_PEAK6_STRENGTH = 0x21;
const uint16_t SFE_XM125_DISTANCE_PEAK7_STRENGTH = 0x22;
const uint16_t SFE_XM125_DISTANCE_PEAK8_STRENGTH = 0x23;
const uint16_t SFE_XM125_DISTANCE_PEAK9_STRENGTH = 0x24;

// Default Value: 250mm
const uint16_t SFE_XM125_DISTANCE_START = 0x40;
const uint16_t sfe_xm125_distance_start_default = 250;

// Default Value: 3000mm
const uint16_t SFE_XM125_DISTANCE_END = 0x41;
const uint16_t sfe_xm125_distance_end_default = 3000;

// Default Value: 0
const uint16_t SFE_XM125_DISTANCE_MAX_STEP_LENGTH = 0x42;
const uint16_t sfe_xm125_distance_max_step_length_default = 0;

// Default Value: True
const uint16_t SFE_XM125_DISTANCE_CLOSE_RANGE_LEAKAGE = 0x43;
const bool sfe_xm125_distance_close_range_leakage_default = true;

// Default Value: 15000
const uint16_t SFE_XM125_DISTANCE_SIGNAL_QUALITY = 0x44;
const uint16_t sfe_xm125_distance_signal_quality_default = 15000;

// Default: PROFILE5 (enum)
const uint16_t SFE_XM125_DISTANCE_MAX_PROFILE = 0x45;
typedef enum
{
    XM125_DISTANCE_PROFILE1 = 1,
    XM125_DISTANCE_PROFILE2 = 2,
    XM125_DISTANCE_PROFILE3 = 3,
    XM125_DISTANCE_PROFILE4 = 4,
    XM125_DISTANCE_PROFILE5 = 5,
} sfe_xm125_distance_profile_t;

// Default Value: CFAR (enum)
const uint16_t SFE_XM125_DISTANCE_THRESHOLD_METHOD = 0x46;
typedef enum
{
    XM125_DISTANCE_FIXED_AMPLITUDE = 1,
    XM125_DISTANCE_RECORDED = 2,
    XM125_DISTANCE_CFAR = 3,
    XM125_DISTANCE_FIXED_STRENGTH = 4,
} sfe_xm125_distance_threshold_method_t;

// Default Value: STRONGEST (enum)
const uint16_t SFE_XM125_DISTANCE_PEAK_SORTING = 0x47;
typedef enum
{
    XM125_DISTANCE_CLOSEST = 1,
    XM125_DISTANCE_STRONGEST = 2,
} sfe_xm125_distance_peak_sorting_t;

// Default Value: 100
const uint16_t SFE_XM125_DISTANCE_NUM_FRAMES_RECORDED_THRESH = 0x48;
const uint16_t sfe_xm125_distance_num_frames_recorded_thresh_default = 100;

// Default Value: 100000
const uint16_t SFE_XM125_DISTANCE_FIXED_AMPLITUDE_THRESHOLD_VAL = 0x49;
const uint32_t sfe_xm125_distance_fixed_amp_thresh_val_default = 100000;

// Default Value: 500
const uint16_t SFE_XM125_DISTANCE_THREHSOLD_SENSITIVITY = 0x4a;
const uint16_t sfe_xm125_distance_threshold_sensitivity_default = 500;

// Default Value: GENERIC
const uint16_t SFE_XM125_DISTANCE_REFLECTOR_SHAPE = 0x4b;
typedef enum
{
    XM125_DISTANCE_GENERIC = 1,
    XM125_DISTANCE_PLANAR = 2,
} sfe_xm125_distance_reflector_shape_t;

// Default Value: 0
const uint16_t SFE_XM125_DISTANCE_FIXED_STRENGTH_THRESHOLD_VAL = 0x4c;
const uint16_t sfe_xm125_distance_fixed_strength_threshold_val_default = 0;

// Default Value: False
const uint16_t SFE_XM125_DISTANCE_MEASURE_ON_WAKEUP = 0x80;
const bool sfe_xm125_distance_measure_on_wakup = false;

const uint16_t SFE_XM125_DISTANCE_COMMAND = 0x100;
typedef enum
{
    XM125_DISTANCE_APPLY_CONFIG_AND_CALIBRATE = 1,
    XM125_DISTANCE_MEASURE_DISTANCE = 2,
    XM125_DISTANCE_APPLY_CONFIGURATION = 3,
    XM125_DISTANCE_CALIBRATE = 4,
    XM125_DISTANCE_RECALIBRATE = 5,
    XM125_DISTANCE_ENABLE_UART_LOGS = 32,
    XM125_DISTANCE_DISABLE_UART_LOGS = 33,
    XM125_DISTANCE_LOG_CONFIGURATION = 34,
    XM125_DISTANCE_RESET_MODULE = 1381192737,
} sfe_xm125_distance_command_t;

const uint32_t SFE_XM125_DISTANCE_APPLY_CONFIGURATION = 1;
const uint32_t SFE_XM125_DISTANCE_START_DETECTOR = 2;
const uint32_t SFE_XM125_DISTANCE_STOP_DETECTOR = 3;
const uint32_t SFE_XM125_DISTANCE_CALIBRATE = 4;
const uint32_t SFE_XM125_DISTANCE_RECALIBRATE = 5;
const uint32_t SFE_XM125_DISTANCE_ENABLE_UART_LOGS = 32;
const uint32_t SFE_XM125_DISTANCE_DISABLE_UART_LOGS = 33;
const uint32_t SFE_XM125_DISTANCE_LOG_CONFIGURATION = 34;
const uint32_t SFE_XM125_DISTANCE_RESET_MODULE = 1381192737;

// Distance class definition

class sfDevXM125Distance : public sfDevXM125Core
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
    sfTkError_t begin(sfTkII2C *theBus = nullptr);

    /// @brief This function sets all the beginning values for a basic I2C
    ///  example to be run on the device for presence sensing.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceBegin(uint32_t start = sfe_xm125_distance_start_default,
                              uint32_t end = sfe_xm125_distance_end_default);

    /// @brief This function does all the required checks and busy waits to
    ///  make sure the device is ready for distance readings.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceDetectorReadingSetup();

    /// @brief This function returns the version number of the device
    ///  structure: major.minor.patch
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch);

    /// @brief This function returns if there was an error from the
    ///  protocol status register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorError(uint32_t &error);

    /// @brief This function returns the error status according to the bit
    ///  mask value for the distance devices errors and busy bit
    /// @param status Error status of device (see function for exact error)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorErrorStatus(uint32_t &status);

    /// @brief This function returns the measure counter, the number of measurements
    ///   performed since restart.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMeasureCounter(uint32_t &counter);

    /// @brief This function returns the status of the device if there are any issues
    ///  with any of the status's listed defined.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceDetectorStatus(uint32_t &status);

    /// @brief This function returns the number of detected distances.
    /// @param distance Number of detected distances
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceNumberDistances(uint32_t &distance);

    /// @brief This function returns the indication that there might be an object
    ///   near the start point of the measured range.
    /// @param edge Flag to determine object in range
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceNearStartEdge(uint32_t &edge);

    /// @brief This function returns the indication of a sensor calibration needed.
    /// @param calibrate Flag to indicate calibration required
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceCalibrationNeeded(uint32_t &calibrate);

    /// @brief This function returns if the measure command failed.
    /// @param error Flag to indicate measure command error
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMeasureDistanceError(uint32_t &error);

    /// @brief This function returns the temperature in sensor during measurements
    ///   (in degree Celsius). Note that it has poor absolute accuracy and should
    ///   only be used for relative temperature measurements.
    /// @param temperature Relative temperature of device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceTemperature(int16_t &temperature);

    //--------------------------------------------------------------------------------
    // Generic distance peak distance method
    /// @brief This function returns the distance to peak num
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param num Peak number to get distance (0-9)
    /// @param peak Distance to peak num
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeakDistance(uint8_t num, uint32_t &peak);

    /// @brief This function returns the distance to peak 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak0Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 1
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak1Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 2
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak2Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 3
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak3Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 4
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak4Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 5
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak5Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 6
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak6Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 7
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak7Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 8
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak8Distance(uint32_t &peak);

    /// @brief This function returns the distance to peak 9
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak9Distance(uint32_t &peak);

    //--------------------------------------------------------------------------------
    // Generic distance peak strength method
    /// @brief This function returns the strength to peak num
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param num Peak number to get strength (0-9)
    /// @param peak strength to peak num
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeakStrength(uint8_t num, int32_t &peak);

    /// @brief This function returns the strength of peak 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak0Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 1
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak1Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 2
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak2Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 3
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak3Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 4
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak4Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 5
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak5Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 6
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak6Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 7
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak7Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 8
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak8Strength(int32_t &peak);

    /// @brief This function returns the strength of peak 9
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeak9Strength(int32_t &peak);

    /// @brief This function returns the start of measured interval
    ///  in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 250
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceStart(uint32_t &startVal);

    /// @brief This function sets the start of measured interval in
    ///  millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 250
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceStart(uint32_t start);

    /// @brief This function returns the end of measured interval
    ///  in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 3000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceEnd(uint32_t &end);

    /// @brief This function sets the end of measured interval
    ///  in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 3000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceEnd(uint32_t end);

    /// @brief This function returns the used to limit step length.
    ///  If set to 0 (default), the step length is calculated
    ///  based on profile.
    ///  Default Value: 0
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMaxStepLength(uint32_t &length);

    /// @brief This function sets the used to limit step length.
    ///  If set to 0 (default), the step length is calculated
    ///  based on profile.
    ///  Default Value: 0
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceMaxStepLength(uint32_t length);

    /// @brief This function reads if the close range leakage
    ///  cancellation logic is enabled.
    ///  Default Value: true
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceCloseRangeLeakageCancellation(bool &range);

    /// @brief This function sets the close range leakage
    ///  cancellation logic.
    ///  Default Value: true
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceCloseRangeLeakageCancellation(bool range);

    /// @brief This function returns the high signal quality in a
    ///  better SNR (because of higher HWAAS) and higher power consumption.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 15000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceSignalQuality(uint32_t &signal);

    /// @brief This function sets the high signal quality in a
    ///  better SNR (because of higher HWAAS) and higher power consumption.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 15000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceSignalQuality(uint32_t signal);

    /// @brief This function returns the max profile of the device.
    ///  Default value = PROFILE5
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMaxProfile(uint32_t &profile);

    /// @brief This function sets the max profile of the device
    ///  Default value = PROFILE5
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceMaxProfile(uint32_t profile);

    /// @brief This function returns the threshold method
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceThresholdMethod(uint32_t &method);

    /// @brief This function sets the threshold method
    /// @param method Threshold method (enum)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceThresholdMethod(uint32_t method);

    /// @brief This function returns the peak sorting method
    /// @param peak Peak sorting method
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistancePeakSorting(uint32_t &peak);

    /// @brief This function sets the peak sorting method
    /// @param peak Peak sorting method
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistancePeakSorting(uint32_t peak);

    /// @brief This function returns the number frames to use for recorded threshold.
    ///  Default Value: 100
    /// @param thresh Number of frames
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceNumFramesRecordedThreshold(uint32_t &thresh);

    /// @brief This function sets the number frames to use for recorded threshold.
    ///  Default Value: 100
    /// @param thresh Number of frames
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceNumFramesRecordedThreshold(uint32_t thresh);

    /// @brief This function returns the fixed amplitude threshold value.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 100000
    /// @param thresh Fixed amplitude threshold value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceFixedAmpThreshold(uint32_t &thresh);

    /// @brief This function sets the fixed amplitude threshold value.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 100000
    /// @param thresh Fixed amplitude threshold value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceFixedAmpThreshold(uint32_t thresh);

    /// @brief This function returns the threshold sensitivity
    ///  (0 <= sensitivity <= 1000)
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 500
    /// @param thresh& Threshold sensitivity
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceThresholdSensitivity(uint32_t &thresh);

    /// @brief This function sets the threshold sensitivity
    ///  (0 <= sensitivity <= 1000)
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 500
    /// @param thresh Threshold sensitivity
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceThresholdSensitivity(uint32_t thresh);

    /// @brief This function returns the reflector shape
    ///   Default Value: GENERIC
    /// @param shape Generic or planar reflection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceReflectorShape(uint32_t &shape);

    /// @brief This function sets the reflector shape
    ///   Default Value: GENERIC
    /// @param shape Generic or planar reflection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceReflectorShape(uint32_t shape);

    /// @brief This function returns the fixed strength threshold value.
    ///  Default Value: 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param thresh fixed threshold strength value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceFixedStrengthThresholdValue(int32_t &thresh);

    /// @brief This function sets the fixed strength threshold value.
    ///  Default Value: 0
    ///  Note: This value is a factor 1000 larger than the RSS value
    /// @param thresh fixed threshold strength value
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceFixedStrengthThresholdValue(int32_t thresh);

    /// @brief This function returns the measure on wakeup status.
    ///   Default Value: false
    /// @param measure Measure on wakeup occurrence
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceMeasureOneWakeup(bool &measure);

    /// @brief This function sets the measure on wakeup status.
    ///   Default Value: false
    /// @param measure Measure on wakeup occurrence
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceMeasureOneWakeup(bool measure);

    /// @brief This function sets the specific execute command as defined in the
    ///   datasheet on Page 25, Section 6.2.40 Command.
    /// @param apply Enable configuration and calibrate
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDistanceCommand(uint32_t command);

    /// @brief This function applies the configuration to the device by
    ///  writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceApplyConfiguration();

    /// @brief This function starts the device by writing the defined
    ///  start value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceStart();

    /// @brief This function stops the device by writing the defined
    ///  stop value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceStop();

    /// @brief This function calibrates the device by writing the defined
    ///  calibration value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceCalibrate();

    /// @brief This function recalibrates the device by writing the defined
    ///  recalibrate value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceRecalibrate();

    /// @brief This function enables the uart logs of the device by
    ///  writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceEnableUartLogs();

    /// @brief This function disables the uart logs of the device by
    ///  writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceDisableUartLogs();

    /// @brief This function enables the configuration log of the device
    ///  by writing the defined value to the distance command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceLogConfiguration();

    /// @brief This function resets the distance detector settings
    ///  of the device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceReset();

    /// @brief Completes a busy wait loop while the device is uploading
    ///  information by waiting for the status
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t distanceBusyWait();
};