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

#include "sfDevXM125Core.h"

// Defines

// defines and data structs
/* ****************************** Presence Values ****************************** */

const uint32_t SFE_XM125_PRESENCE_DETECTOR_STATUS_MASK = 0b10010000111111110000000011111111;

const uint32_t SFE_XM125_PRESENCE_DETECTED_MASK = 0x00000001;
const uint32_t SFE_XM125_PRESENCE_DETECTED_STICKY_MASK = 0x00000002;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK = 0x00008000;
const uint32_t SFE_XM125_PRESENCE_TEMPERATURE_MASK = 0xffff0000;
const uint32_t SFE_XM125_PRESENCE_MAJOR_VERSION_MASK = 0xffff0000;
const uint32_t SFE_XM125_PRESENCE_MINOR_VERSION_MASK = 0x0000ff00;
const uint32_t SFE_XM125_PRESENCE_PATCH_VERSION_MASK = 0x000000ff;

const uint32_t SFE_XM125_PRESENCE_RSS_REGISTER_OK_MASK = 0x00000001;
const uint32_t SFE_XM125_PRESENCE_CONFIG_CREATE_OK_MASK = 0x00000002;
const uint32_t SFE_XM125_PRESENCE_SENSOR_CREATE_OK_MASK = 0x00000004;
const uint32_t SFE_XM125_PRESENCE_SENSOR_CALIBRATE_OK_MASK = 0x00000008;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_CREATE_OK_MASK = 0x00000010;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_BUFFER_OK_MASK = 0x00000020;
const uint32_t SFE_XM125_PRESENCE_SENSOR_BUFFER_OK_MASK = 0x00000040;
const uint32_t SFE_XM125_PRESENCE_CONFIG_APPLY_OK_MASK = 0x00000080;
const uint32_t SFE_XM125_PRESENCE_RSS_REGISTER_ERROR_MASK = 0x00010000;
const uint32_t SFE_XM125_PRESENCE_CONFIG_CREATE_ERROR_MASK = 0x00020000;
const uint32_t SFE_XM125_PRESENCE_SENSOR_CREATE_ERROR_MASK = 0x00040000;
const uint32_t SFE_XM125_PRESENCE_SENSOR_CALIBRATE_ERROR_MASK = 0x00080000;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_CREATE_ERROR_MASK = 0x00100000;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_BUFFER_ERROR_MASK = 0x00200000;
const uint32_t SFE_XM125_PRESENCE_SENSOR_BUFFER_ERROR_MASK = 0x00400000;
const uint32_t SFE_XM125_PRESENCE_CONFIG_APPLY_ERROR_MASK = 0x00800000;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_REG_ERROR_MASK = 0x10000000;
const uint32_t SFE_XM125_PRESENCE_BUSY_MASK = 0x80000000;

const uint32_t SFE_XM125_PRESENCE_ALL_ERROR_MASK =
    (SFE_XM125_PRESENCE_RSS_REGISTER_ERROR_MASK | SFE_XM125_PRESENCE_CONFIG_CREATE_ERROR_MASK |
     SFE_XM125_PRESENCE_SENSOR_CREATE_ERROR_MASK | SFE_XM125_PRESENCE_SENSOR_CALIBRATE_ERROR_MASK |
     SFE_XM125_PRESENCE_DETECTOR_CREATE_ERROR_MASK | SFE_XM125_PRESENCE_DETECTOR_BUFFER_ERROR_MASK |
     SFE_XM125_PRESENCE_SENSOR_BUFFER_ERROR_MASK | SFE_XM125_PRESENCE_CONFIG_APPLY_ERROR_MASK |
     SFE_XM125_PRESENCE_DETECTOR_REG_ERROR_MASK | SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK |
     SFE_XM125_PRESENCE_BUSY_MASK);

const uint32_t SFE_XM125_PRESENCE_MAJOR_VERSION_MASK_SHIFT = 16;
const uint32_t SFE_XM125_PRESENCE_MINOR_VERSION_MASK_SHIFT = 8;
const uint32_t SFE_XM125_PRESENCE_RSS_REGISTER_ERROR_MASK_SHIFT = 15;
const uint32_t SFE_XM125_PRESENCE_CONFIG_CREATE_ERROR_MASK_SHIFT = 16;
const uint32_t SFE_XM125_PRESENCE_SENSOR_CREATE_ERROR_MASK_SHIFT = 17;
const uint32_t SFE_XM125_PRESENCE_SENSOR_CALIBRATE_ERROR_MASK_SHIFT = 18;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_CREATE_ERROR_MASK_SHIFT = 19;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_BUFFER_ERROR_MASK_SHIFT = 20;
const uint32_t SFE_XM125_PRESENCE_SENSOR_BUFFER_ERROR_MASK_SHIFT = 21;
const uint32_t SFE_XM125_PRESENCE_CONFIG_APPLY_ERROR_MASK_SHIFT = 22;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_REG_ERROR_MASK_SHIFT = 27;
const uint32_t SFE_XM125_PRESENCE_DETECTOR_ERROR_MASK_SHIFT = 14;
const uint32_t SFE_XM125_PRESENCE_TEMPERATURE_MASK_SHIFT = 16;
const uint32_t SFE_XM125_PRESENCE_BUSY_MASK_SHIFT = 30;

const uint16_t SFE_XM125_PRESENCE_VERSION = 0x00;
typedef struct
{
    uint32_t presence_major : 16;
    uint32_t presence_minor : 8;
    uint32_t presence_patch : 8;
} sfe_xm125_presence_version_t;

const uint16_t SFE_XM125_PRESENCE_PROTOCOL_STATUS = 0x01;
typedef struct
{
    uint32_t presence_protocol_state_error : 1;
    uint32_t presence_packet_length_error : 1;
    uint32_t presence_address_error : 1;
    uint32_t presence_write_failed : 1;
    uint32_t presence_write_to_read_only : 1;
    uint32_t reserved1 : 27;
} sfe_xm125_presence_protocol_status_t;

const uint16_t SFE_XM125_PRESENCE_MEASURE_COUNTER = 0x02;

const uint16_t SFE_XM125_PRESENCE_DETECTOR_STATUS = 0x03;
typedef struct
{
    uint32_t presence_rss_register_ok : 1;
    uint32_t presence_config_create_ok : 1;
    uint32_t presence_sensor_create_ok : 1;
    uint32_t presence_sensor_calibrate_ok : 1;
    uint32_t presence_detector_create_ok : 1;
    uint32_t presence_detector_buffer_ok : 1;
    uint32_t presence_sensor_buffer_ok : 1;
    uint32_t presence_config_apply_ok : 1;
    uint32_t reserved1 : 8;
    uint32_t presence_rss_register_error : 1;
    uint32_t presence_config_create_error : 1;
    uint32_t presence_sensor_create_error : 1;
    uint32_t presence_sensor_calibrate_error : 1;
    uint32_t presence_detector_create_error : 1;
    uint32_t presence_detector_buffer_error : 1;
    uint32_t presence_sensor_buffer_error : 1;
    uint32_t presence_config_apply_error : 1;
    uint32_t reserved2 : 4;
    uint32_t presence_detector_error : 1;
    uint32_t reserved3 : 2;
    uint32_t presence_busy : 1;

} sfe_xm125_presence_detector_status_t;

const uint16_t SFE_XM125_PRESENCE_RESULT = 0x10;
typedef struct
{
    uint32_t presence_detected : 1;
    uint32_t presence_detected_sticky : 1;
    uint32_t reserved1 : 13;
    uint32_t presence_detector_error : 1;
    uint32_t presence_temperature : 16;
} sfe_xm125_presence_result_t;

const uint16_t SFE_XM125_PRESENCE_DISTANCE = 0x11;
const uint16_t SFE_XM125_INTRA_PRESENCE_SCORE = 0x12;
const uint16_t SFE_XM125_INTER_PRESENCE = 0x13;

const uint16_t SFE_XM125_PRESENCE_SWEEPS_PER_FRAME = 0x40;
const uint16_t sfe_xm125_presence_sweeps_per_frame_default = 16;

const uint16_t SFE_XM125_PRESENCE_INTER_FRAME_TIMEOUT = 0x41;
const uint16_t sfe_xm125_presence_inter_frame_timeout_default = 3;

const uint16_t SFE_XM125_PRESENCE_INTER_PHASE_BOOST_ENABLED = 0x42;
const bool sfe_xm125_presence_inter_phase_boost_enabled_default = false;

const uint16_t SFE_XM125_PRESENCE_INTRA_DETECTION_ENABLED = 0x43;
const bool sfe_xm125_presence_intra_detection_enabled_default = true;

const uint16_t SFE_XM125_PRESENCE_INTER_DETECTION_ENABLED = 0x44;
const bool sfe_xm125_presence_inter_detection_enabled_default = true;

const uint16_t SFE_XM125_PRESENCE_FRAME_RATE = 0x45;
const uint16_t sfe_xm125_presence_frame_rate_default = 12000;

const uint16_t SFE_XM125_PRESENCE_INTRA_DETECTION_THRESHOLD = 0x46;
const uint16_t sfe_xm125_presence_intra_detection_threshold_default = 1300;

const uint16_t SFE_XM125_PRESENCE_INTER_DETECTION_THRESHOLD = 0x47;
const uint16_t sfe_xm125_presence_inter_detection_threshold_default = 1000;

const uint16_t SFE_XM125_PRESENCE_INTER_FRAME_DEVIATION = 0x48;
const uint16_t sfe_xm125_presence_inter_frame_deviation_default = 500;

const uint16_t SFE_XM125_PRESENCE_INTER_FRAME_FAST_CUTOFF = 0x49;
const uint16_t sfe_xm125_presence_inter_frame_fast_cutoff_default = 6000;

const uint16_t SFE_XM125_PRESENCE_INTER_FRAME_SLOW_CUTOFF = 0x4a;
const uint16_t sfe_xm125_presence_inter_frame_slow_cutoff_default = 200;

const uint16_t SFE_XM125_PRESENCE_INTRA_FRAME_TIME_CONST = 0x4b;
const uint16_t sfe_xm125_presence_intra_frame_time_const_default = 150;

const uint16_t SFE_XM125_PRESENCE_INTRA_OUTPUT_TIME_CONST = 0x4c;
const uint16_t sfe_xm125_presence_intra_output_time_const_default = 300;

const uint16_t SFE_XM125_PRESENCE_INTER_OUTPUT_TIME_CONST = 0x4d;
const uint16_t sfe_xm125_presence_inter_output_time_const_default = 2000;

const uint16_t SFE_XM125_PRESENCE_AUTO_PROFILE_ENABLED = 0x4e;
const bool sfe_xm125_presence_auto_profile_enabled_default = true;

const uint16_t SFE_XM125_PRESENCE_AUTO_STEP_LENGTH_ENABLED = 0x4f;
const bool sfe_xm125_presence_auto_step_length_enabled_default = true;

const uint16_t SFE_XM125_PRESENCE_MANUAL_PROFILE = 0x50;
typedef enum
{
    XM125_PRESENCE_PROFILE1 = 1,
    XM125_PRESENCE_PROFILE2 = 2,
    XM125_PRESENCE_PROFILE3 = 3,
    XM125_PRESENCE_PROFILE4 = 4,
    XM125_PRESENCE_PROFILE5 = 5,
} sfe_xm125_presence_manual_profile_t;

const uint16_t SFE_XM125_PRESENCE_MANUAL_STEP_LENGTH = 0x51;
const uint16_t sfe_xm125_presence_manual_step_length_default = 72;

const uint16_t SFE_XM125_PRESENCE_START = 0x52;
const uint16_t sfe_xm125_presence_start_default = 250;

const uint16_t SFE_XM125_PRESENCE_END = 0x53;
const uint16_t sfe_xm125_presence_end_default = 2500;

const uint16_t SFE_XM125_PRESENCE_RESET_FILTERS_ON_PREPARE = 0x54;
const bool sfe_xm125_presence_reset_filters_on_prepare_default = true;

const uint16_t SFE_XM125_PRESENCE_HWAAS = 0x55;
const uint16_t sfe_xm125_presence_hwaas_default = 32;

const uint16_t SFE_XM125_PRESENCE_DETECTION_ON_GPIO = 0x80;
const bool sfe_xm125_presence_detection_on_gpio_default = false;

const uint16_t SFE_XM125_PRESENCE_COMMAND = 0x100;
typedef enum
{
    XM125_PRESENCE_APPLY_CONFIGURATION = 1,
    XM125_PRESENCE_START_DETECTOR = 2,
    XM125_PRESENCE_STOP_DETECTOR = 3,
    XM125_PRESENCE_ENABLE_UART_LOGS = 32,
    XM125_PRESENCE_DISABLE_UART_LOGS = 33,
    XM125_PRESENCE_LOG_CONFIGURATION = 34,
    XM125_PRESENCE_RESET_MODULE = 138119737,
} sfe_xm125_presence_command_t;

const uint32_t SFE_XM125_PRESENCE_APPLY_CONFIGURATION = 1;
const uint32_t SFE_XM125_PRESENCE_START_DETECTOR = 2;
const uint32_t SFE_XM125_PRESENCE_STOP_DETECTOR = 3;
const uint32_t SFE_XM125_PRESENCE_ENABLE_UART_LOGS = 32;
const uint32_t SFE_XM125_PRESENCE_DISABLE_UART_LOGS = 33;
const uint32_t SFE_XM125_PRESENCE_LOG_CONFIGURATION = 34;
const uint32_t SFE_XM125_PRESENCE_RESET_MODULE = 1381192737;

// Presence class definition

class sfDevXM125Presence : public sfDevXM125Core
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
    sfTkError_t begin(sfTkII2C *theBus = nullptr) override;
    /// @brief This function sets all the beginning values for a basic I2C
    ///  example to be run on the device for presence sensing.
    /// @param start Start value for presence sensing in mm  - default value is 1000
    /// @param end End value for presence sensing in mm - default value is 5000
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t detectorStart(uint32_t start = 1000, uint32_t end = 5000);

    /// @brief This function returns the presence value of the register
    ///  with all the checks in place as per the I2C Datasheet.
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistanceValuemm(uint32_t &presenceVal);

    /// @brief This function returns the RSS version number
    /// @param version Version number
    /// @param patch Patch version number
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch);

    /// @brief This function returns the protocol state erorr
    /// @param state Error code for device
    ///   0 = Protocol state error
    ///   1 = Packet length error
    ///   2 = Address error
    ///   3 = Write failed
    ///   4 = Write to read only
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorError(uint32_t &error);

    /// @brief This function returns the measure counter; the number of
    ///  measurements performed since restart.
    /// @param counter number of measurements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getMeasureCounter(uint32_t &counter);

    /// @brief This function returns the detector status flags. See page 17, section
    ///   6.2.4 Detector Status
    /// @param status Status detector flag
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorStatus(uint32_t &status);

    /// @brief This function returns the error status according to the bit
    ///  mask value for the presence devices errors and busy bit
    /// @param status Error status of device (see function for exact error)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorErrorStatus(uint32_t &status);

    /// @brief This function returns if there was presence detected
    /// @param detected Presence Detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorPresenceDetected(uint32_t &detected);

    /// @brief This function returns if there was presence detected, sticky bit with
    ///  clear on read.
    /// @param sticky Sticky Presence Detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorPresenceStickyDetected(uint32_t &sticky);

    /// @brief This function returns the indication that the presence detector
    ///  failed.
    /// @param calibrate Indication of sensor calibration
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectorRegError(uint32_t &error);

    /// @brief This function returns the temperature in sensor during measurement (in
    ///   degree Celsius). Note that it has poor absolute accuracy and should only
    ///   be used for relative temperature measurements.
    /// @param temp Relative temperature
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getTemperature(uint32_t &temp);

    /// @brief This function returns the distance, in millimeters, for the detected
    ///  presence.
    /// @param distance distance in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDistance(uint32_t &distance);

    /// @brief This function returns the measure of amount of fast motion detected.
    /// @param intra Measure of fast motion
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getIntraPresenceScore(uint32_t &intra);

    /// @brief This function returns the measure of amount of slow motion detected.
    /// @param inter Measure of slow motion
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterPresenceScore(uint32_t &inter);

    /// @brief This function returns the number of sweeps that will be
    ///   captured in each frame (measurement).
    ///   Default Value: 16 seconds
    /// @param sweeps Number of sweeps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getSweepsPerFrame(uint32_t &sweeps);

    /// @brief This function sets the number of sweeps that will be
    ///   captured in each frame (measurement).
    ///   Default Value: 16 seconds
    /// @param sweeps Number of sweeps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setSweepsPerFrame(uint32_t sweeps);

    /// @brief Returns the number of seconds the inter-frame presence score needs to decrease before
    ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
    ///  means no timeout.
    ///   Default Value: 3 seconds
    /// @param time Number of seconds
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterFramePresenceTimeout(uint32_t &time);

    /// @brief Sets the number of seconds the inter-frame presence score needs to decrease before
    ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
    ///  means no timeout.
    ///   Default Value: 3 seconds
    /// @param time Number of seconds
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterFramePresenceTimeout(uint32_t time);

    /// @brief This function returns the enable to increase detection of slow
    ///  motions by utilizing the phase information in the Sparse IQ data
    /// @param en Enables increased detection
    ///   Default Value: false
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterPhaseBoostEnabled(bool &en);

    /// @brief This function sets the enable to increase detection of slow
    ///  motions by utilizing the phase information in the Sparse IQ data
    /// @param en Enables increased detection
    ///   Default Value: false
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterPhaseBoostEnabled(bool en);

    /// @brief This function returns the bit to enable to detect faster
    ///  movements inside frames.
    ///   Default Value: true
    /// @param en Enables faster detection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getIntraDetectionEnabled(bool &en);

    /// @brief This function returns the bit to enable to detect slower
    ///  movements between frames.
    ///   Default Value: true
    /// @param en Enables faster detection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterDetectionEnabled(bool en);

    /// @brief This function returns the presence detector frame rate.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 12000 mHz
    /// @param rate Frame rate, in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getFrameRate(uint32_t &rate);

    /// @brief This function sets the presence detector frame rate.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 12000 mHz
    /// @param rate Frame rate, in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setFrameRate(uint32_t rate);

    /// @brief This function returns the threshold for detecting faster
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1300
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getIntraDetectionThreshold(uint32_t &thresh);

    /// @brief This function sets the threshold for detecting faster
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1300
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setIntraDetectionThreshold(uint32_t thresh);

    /// @brief This function returns the threshold for detecting slower
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1000
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterDetectionThreshold(uint32_t &thresh);

    /// @brief This function sets the threshold for detecting slower
    ///  movements inside frames.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 1000
    /// @param thresh threshold for faster movements
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterDetectionThreshold(uint32_t thresh);

    /// @brief This function returns the time constant of the low pass
    ///  filter for the inter-frame deviation between fast and slow.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 500 ms
    /// @param time time in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterFrameDeviationTime(uint32_t &time);

    /// @brief This function sets the time constant of the low pass
    ///  filter for the inter-frame deviation between fast and slow.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 500 ms
    /// @param time time in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterFrameDeviationTime(uint32_t time);

    /// @brief This function returns the cutoff frequency of the low pass
    ///  filter for the fast filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 6000 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterFrameFastCutoff(uint32_t &cut);

    /// @brief This function sets the cutoff frequency of the low pass
    ///  filter for the fast filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 6000 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterFrameFastCutoff(uint32_t cut);

    /// @brief This function returns the cutoff frequency of the low pass
    ///  filter for the slow filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 200 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterFrameSlowCutoff(uint32_t &cut);

    /// @brief This function sets the cutoff frequency of the low pass
    ///  filter for the slow filtered absolute sweep mean.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 200 mHz
    /// @param cut cutoff frequency in mHz
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterFrameSlowCutoff(uint32_t cut);

    /// @brief This functions returns time constant for the depthwise
    ///  filtering in the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 150ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getIntraFrameTimeConst(uint32_t &time);

    /// @brief This functions sets time constant for the depthwise
    ///  filtering in the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 150ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setIntraFrameTimeConst(uint32_t time);

    /// @brief This function returns time constant for the output in
    ///  the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 300ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getIntraOutputTimeConst(uint32_t &time);

    /// @brief This function sets the time constant for the output in
    ///  the intra-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 300ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setIntraOutputTimeConst(uint32_t time);

    /// @brief This function returns time constant for the output in
    ///  the inter-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2000ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getInterOutputTimeConst(uint32_t &time);

    /// @brief This function sets the time constant for the output in
    ///  the inter-frame part
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2000ms
    /// @param time time const in ms
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setInterOutputTimeConst(uint32_t time);

    /// @brief This function returns if enable/disable for the automatic
    ///  selection of profile based on start point of measurement.
    ///   Default Value: true
    /// @param en enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getAutoProfileEn(bool &en);

    /// @brief This function enables/disables the automatic selection
    ///  of profile based on start point of measurement.
    ///   Default Value: true
    /// @param en enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setAutoProfileEn(bool en);

    /// @brief This function returns the Enable/Disable
    ///  automatic selection of step length based on the profile.
    /// @param en Enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getAutoStepLengthEn(bool &en);

    /// @brief This function Enables/Disables automatic selection
    ///  of step length based on the profile.
    /// @param en Enable/disable selection
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setAutoStepLengthEn(bool en);

    /// @brief This function returns the profile in use. The profile will only
    ///  be used if profile auto selection was disabled.
    /// @param prof Profile to use (enum)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getManualProfile(uint32_t &prof);

    /// @brief This function sets the profile to use. The profile will only
    ///  be used if profile auto selection was disabled.
    /// @param prof Profile to use (enum)
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setManualProfile(uint32_t prof);

    /// @brief THis function returns the number of steps between each data point.
    ///  NOTE: The manual step length will only be used if step length auto selection
    ///  was disabled
    ///   Default Value: 72
    /// @param length number of steps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getManualStepLength(uint32_t &length);

    /// @brief THis function returns the number of steps between each data point.
    ///  NOTE: The manual step length will only be used if step length auto selection
    ///  was disabled
    ///   Default Value: 72
    /// @param length number of steps
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setManualStepLength(uint32_t length);

    /// @brief This function returns the start of measured interval in mm.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 300mm
    /// @param start measured start interval in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getStart(uint32_t &start);

    /// @brief This function sets the start of measured interval in mm.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///  Default Value: 300mm
    /// @param start measured start interval in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setStart(uint32_t start);

    /// @brief This function returns the end point of measurement
    ///  interval in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2500
    /// @param end End point of measurement in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getEnd(uint32_t &end);

    /// @brief This function returns the end point of measurement
    ///  interval in millimeters.
    ///  Note: This value is a factor 1000 larger than the RSS value
    ///   Default Value: 2500
    /// @param end End point of measurement in mm
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setEnd(uint32_t end);

    /// @brief This function returns if the device is enabled/disabled
    ///  for the reset of the presence filters during start/restart.
    ///   Default Value: true
    /// @param reset enabled/disabled
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getResetFilters(bool &reset);

    /// @brief This function sets if the device is enabled/disabled
    ///  for the reset of the presence filters during start/restart .
    ///   Default Value: true
    /// @param reset enabled/disabled
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setResetFilters(bool reset);

    /// @brief This function returns the hardware accelerated average
    ///  sampled (HWAAS).
    ///   Default Value: 32
    /// @param avg average samples
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getHWAAS(uint32_t &avg);

    /// @brief This function sets the hardware accelerated average
    ///  sampled (HWAAS).
    ///   Default Value: 32
    /// @param avg average samples
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setHWAAS(uint32_t avg);

    /// @brief This function returns the output presence detection on
    ///  generic GPIO.
    /// @param detected Output presence detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getDetectionOnGPIO(uint32_t &detected);

    /// @brief This function returns the output presence detection on
    ///  generic GPIO.
    /// @param detected Output presence detected
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setDetectionOnGPIO(uint32_t detected);

    /// @brief This function sets the execute command
    /// @param cmd command to send to device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t setCommand(uint32_t cmd);

    /// @brief This function applies the configuration to the device by
    ///  writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t applyConfiguration();

    /// @brief This function starts the device by writing the defined
    ///  start value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t start();

    /// @brief This function stops the device by writing the defined
    ///  stop value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t stop();

    /// @brief This function enables the uart logs of the device by
    ///  writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t enableUartLogs();

    /// @brief This function disables the uart logs of the device by
    ///  writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t disableUartLogs();

    /// @brief This function enables the configuration log of the device
    ///  by writing the defined value to the presence command register
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t logConfiguration();

    /// @brief This function resets the presence detector settings
    ///  of the device
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t reset();

    /// @brief This function returns the busy bit of the presence status register
    /// @param busy Device busy or not
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t getBusy(uint32_t &busy);

    /// @brief Completes a busy wait loop while the device is uploading
    ///  information by waiting for the status. Checks the error status register
    ///  to wait until errors are completed/gone
    /// @return ksfTkErrOk on success, or error code (value < -1)
    sfTkError_t busyWait();
};