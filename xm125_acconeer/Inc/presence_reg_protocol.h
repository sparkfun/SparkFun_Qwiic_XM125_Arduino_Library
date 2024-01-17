// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef PRESENCE_REG_PROTOCOL_H_
#define PRESENCE_REG_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

// The number of registers in the application
#define PRESENCE_REG_REGISTER_COUNT 32U

// The register address defines
#define PRESENCE_REG_VERSION_ADDRESS 0U
#define PRESENCE_REG_PROTOCOL_STATUS_ADDRESS 1U
#define PRESENCE_REG_MEASURE_COUNTER_ADDRESS 2U
#define PRESENCE_REG_DETECTOR_STATUS_ADDRESS 3U
#define PRESENCE_REG_PRESENCE_RESULT_ADDRESS 16U
#define PRESENCE_REG_PRESENCE_DISTANCE_ADDRESS 17U
#define PRESENCE_REG_INTRA_PRESENCE_SCORE_ADDRESS 18U
#define PRESENCE_REG_INTER_PRESENCE_SCORE_ADDRESS 19U
#define PRESENCE_REG_SWEEPS_PER_FRAME_ADDRESS 64U
#define PRESENCE_REG_INTER_FRAME_PRESENCE_TIMEOUT_ADDRESS 65U
#define PRESENCE_REG_INTER_PHASE_BOOST_ENABLED_ADDRESS 66U
#define PRESENCE_REG_INTRA_DETECTION_ENABLED_ADDRESS 67U
#define PRESENCE_REG_INTER_DETECTION_ENABLED_ADDRESS 68U
#define PRESENCE_REG_FRAME_RATE_ADDRESS 69U
#define PRESENCE_REG_INTRA_DETECTION_THRESHOLD_ADDRESS 70U
#define PRESENCE_REG_INTER_DETECTION_THRESHOLD_ADDRESS 71U
#define PRESENCE_REG_INTER_FRAME_DEVIATION_TIME_CONST_ADDRESS 72U
#define PRESENCE_REG_INTER_FRAME_FAST_CUTOFF_ADDRESS 73U
#define PRESENCE_REG_INTER_FRAME_SLOW_CUTOFF_ADDRESS 74U
#define PRESENCE_REG_INTRA_FRAME_TIME_CONST_ADDRESS 75U
#define PRESENCE_REG_INTRA_OUTPUT_TIME_CONST_ADDRESS 76U
#define PRESENCE_REG_INTER_OUTPUT_TIME_CONST_ADDRESS 77U
#define PRESENCE_REG_AUTO_PROFILE_ENABLED_ADDRESS 78U
#define PRESENCE_REG_AUTO_STEP_LENGTH_ENABLED_ADDRESS 79U
#define PRESENCE_REG_MANUAL_PROFILE_ADDRESS 80U
#define PRESENCE_REG_MANUAL_STEP_LENGTH_ADDRESS 81U
#define PRESENCE_REG_START_ADDRESS 82U
#define PRESENCE_REG_END_ADDRESS 83U
#define PRESENCE_REG_RESET_FILTERS_ON_PREPARE_ADDRESS 84U
#define PRESENCE_REG_HWAAS_ADDRESS 85U
#define PRESENCE_REG_DETECTION_ON_GPIO_ADDRESS 128U
#define PRESENCE_REG_COMMAND_ADDRESS 256U

// The defines for version fields (masks ans bit-positions)
#define PRESENCE_REG_VERSION_FIELD_MAJOR_POS  16U
#define PRESENCE_REG_VERSION_FIELD_MAJOR_MASK 0xffff0000
#define PRESENCE_REG_VERSION_FIELD_MINOR_POS  8U
#define PRESENCE_REG_VERSION_FIELD_MINOR_MASK 0x0000ff00
#define PRESENCE_REG_VERSION_FIELD_PATCH_POS  0U
#define PRESENCE_REG_VERSION_FIELD_PATCH_MASK 0x000000ff

// The defines for protocol_status fields (masks ans bit-positions)
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_POS  0U
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK 0x00000001
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_POS  1U
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK 0x00000002
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_POS  2U
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK 0x00000004
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_POS  3U
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK 0x00000008
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_POS  4U
#define PRESENCE_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK 0x00000010

// The defines for detector_status fields (masks ans bit-positions)
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_OK_POS  0U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_OK_MASK 0x00000001
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_OK_POS  1U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_OK_MASK 0x00000002
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_OK_POS  2U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_OK_MASK 0x00000004
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_OK_POS  3U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK 0x00000008
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_OK_POS  4U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_OK_MASK 0x00000010
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_OK_POS  5U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_OK_MASK 0x00000020
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_OK_POS  6U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_OK_MASK 0x00000040
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_OK_POS  7U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_OK_MASK 0x00000080
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_ERROR_POS  16U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_ERROR_MASK 0x00010000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_ERROR_POS  17U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_ERROR_MASK 0x00020000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_ERROR_POS  18U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK 0x00040000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_POS  19U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK 0x00080000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_ERROR_POS  20U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_ERROR_MASK 0x00100000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_ERROR_POS  21U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_ERROR_MASK 0x00200000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_ERROR_POS  22U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK 0x00400000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_ERROR_POS  23U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK 0x00800000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_POS  28U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK 0x10000000
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_BUSY_POS  31U
#define PRESENCE_REG_DETECTOR_STATUS_FIELD_BUSY_MASK 0x80000000

// The defines for presence_result fields (masks ans bit-positions)
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_PRESENCE_DETECTED_POS  0U
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_PRESENCE_DETECTED_MASK 0x00000001
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_PRESENCE_DETECTED_STICKY_POS  1U
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_PRESENCE_DETECTED_STICKY_MASK 0x00000002
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_DETECTOR_ERROR_POS  15U
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_DETECTOR_ERROR_MASK 0x00008000
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_TEMPERATURE_POS  16U
#define PRESENCE_REG_PRESENCE_RESULT_FIELD_TEMPERATURE_MASK 0xffff0000

// The defines for manual_profile enum values
#define PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE1 1U
#define PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE2 2U
#define PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE3 3U
#define PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE4 4U
#define PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE5 5U

// The defines for command enum values
#define PRESENCE_REG_COMMAND_ENUM_APPLY_CONFIGURATION 1U
#define PRESENCE_REG_COMMAND_ENUM_START_DETECTOR 2U
#define PRESENCE_REG_COMMAND_ENUM_STOP_DETECTOR 3U
#define PRESENCE_REG_COMMAND_ENUM_ENABLE_UART_LOGS 32U
#define PRESENCE_REG_COMMAND_ENUM_DISABLE_UART_LOGS 33U
#define PRESENCE_REG_COMMAND_ENUM_LOG_CONFIGURATION 34U
#define PRESENCE_REG_COMMAND_ENUM_RESET_MODULE 0x52535421

// The config default defines
#define PRESENCE_REG_SWEEPS_PER_FRAME_DEFAULT_VALUE 16U
#define PRESENCE_REG_INTER_FRAME_PRESENCE_TIMEOUT_DEFAULT_VALUE 3U
#define PRESENCE_REG_INTER_PHASE_BOOST_ENABLED_DEFAULT_VALUE 0U
#define PRESENCE_REG_INTRA_DETECTION_ENABLED_DEFAULT_VALUE 1U
#define PRESENCE_REG_INTER_DETECTION_ENABLED_DEFAULT_VALUE 1U
#define PRESENCE_REG_FRAME_RATE_DEFAULT_VALUE 12000U
#define PRESENCE_REG_INTRA_DETECTION_THRESHOLD_DEFAULT_VALUE 1300U
#define PRESENCE_REG_INTER_DETECTION_THRESHOLD_DEFAULT_VALUE 1000U
#define PRESENCE_REG_INTER_FRAME_DEVIATION_TIME_CONST_DEFAULT_VALUE 500U
#define PRESENCE_REG_INTER_FRAME_FAST_CUTOFF_DEFAULT_VALUE 6000U
#define PRESENCE_REG_INTER_FRAME_SLOW_CUTOFF_DEFAULT_VALUE 200U
#define PRESENCE_REG_INTRA_FRAME_TIME_CONST_DEFAULT_VALUE 150U
#define PRESENCE_REG_INTRA_OUTPUT_TIME_CONST_DEFAULT_VALUE 300U
#define PRESENCE_REG_INTER_OUTPUT_TIME_CONST_DEFAULT_VALUE 2000U
#define PRESENCE_REG_AUTO_PROFILE_ENABLED_DEFAULT_VALUE 1U
#define PRESENCE_REG_AUTO_STEP_LENGTH_ENABLED_DEFAULT_VALUE 1U
#define PRESENCE_REG_MANUAL_PROFILE_DEFAULT_VALUE PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE4
#define PRESENCE_REG_MANUAL_STEP_LENGTH_DEFAULT_VALUE 72U
#define PRESENCE_REG_START_DEFAULT_VALUE 300U
#define PRESENCE_REG_END_DEFAULT_VALUE 2500U
#define PRESENCE_REG_RESET_FILTERS_ON_PREPARE_DEFAULT_VALUE 1U
#define PRESENCE_REG_HWAAS_DEFAULT_VALUE 32U
#define PRESENCE_REG_DETECTION_ON_GPIO_DEFAULT_VALUE 0U


/**
 * The presence protocol setup function.
 */
void presence_reg_protocol_setup(void);


/**
 * The presence protocol write default register value function.
 */
void presence_reg_protocol_write_default(void);


/**
 * Read:
 * Get the RSS version.
 */
void presence_reg_read_version(uint32_t *value);


/**
 * Read:
 * Get protocol error flags.
 */
void presence_reg_read_protocol_status(uint32_t *value);


/**
 * Read:
 * Get the measure counter, the number of measurements performed since
 * restart.
 */
void presence_reg_read_measure_counter(uint32_t *value);


/**
 * Read:
 * Get detector status flags.
 */
void presence_reg_read_detector_status(uint32_t *value);


/**
 * Read:
 * The result from the presence detector.
 */
void presence_reg_read_presence_result(uint32_t *value);


/**
 * Read:
 * The distance, in millimeters, for the detected presence
 */
void presence_reg_read_presence_distance(uint32_t *value);


/**
 * Read:
 * A measure of the amount of fast motion detected.
 */
void presence_reg_read_intra_presence_score(uint32_t *value);


/**
 * Read:
 * A measure of the amount of slow motion detected.
 */
void presence_reg_read_inter_presence_score(uint32_t *value);


/**
 * Read:
 * The number of sweeps that will be captured in each frame
 * (measurement).
 */
void presence_reg_read_sweeps_per_frame(uint32_t *value);


/**
 * Write:
 * The number of sweeps that will be captured in each frame
 * (measurement).
 */
bool presence_reg_write_sweeps_per_frame(const uint32_t value);


/**
 * Read:
 * Number of seconds the inter-frame presence score needs to decrease
 * before exponential scaling starts for faster decline. Should be
 * between 0 and 30 where 0 means no timeout. Note:
 */
void presence_reg_read_inter_frame_presence_timeout(uint32_t *value);


/**
 * Write:
 * Number of seconds the inter-frame presence score needs to decrease
 * before exponential scaling starts for faster decline. Should be
 * between 0 and 30 where 0 means no timeout. Note:
 */
bool presence_reg_write_inter_frame_presence_timeout(const uint32_t value);


/**
 * Read:
 * Enable to increase detection of slow motions by utilizing the phase
 * information in the Sparse IQ data.
 */
void presence_reg_read_inter_phase_boost_enabled(uint32_t *value);


/**
 * Write:
 * Enable to increase detection of slow motions by utilizing the phase
 * information in the Sparse IQ data.
 */
bool presence_reg_write_inter_phase_boost_enabled(const uint32_t value);


/**
 * Read:
 * Enable to detect faster movements inside frames.
 */
void presence_reg_read_intra_detection_enabled(uint32_t *value);


/**
 * Write:
 * Enable to detect faster movements inside frames.
 */
bool presence_reg_write_intra_detection_enabled(const uint32_t value);


/**
 * Read:
 * Enable to detect slower movements between frames.
 */
void presence_reg_read_inter_detection_enabled(uint32_t *value);


/**
 * Write:
 * Enable to detect slower movements between frames.
 */
bool presence_reg_write_inter_detection_enabled(const uint32_t value);


/**
 * Read:
 * The presence detector frame rate. Note: This value is a factor 1000
 * larger than the RSS value.
 */
void presence_reg_read_frame_rate(uint32_t *value);


/**
 * Write:
 * The presence detector frame rate. Note: This value is a factor 1000
 * larger than the RSS value.
 */
bool presence_reg_write_frame_rate(const uint32_t value);


/**
 * Read:
 * The threshold for detecting faster movements inside frames. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
void presence_reg_read_intra_detection_threshold(uint32_t *value);


/**
 * Write:
 * The threshold for detecting faster movements inside frames. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
bool presence_reg_write_intra_detection_threshold(const uint32_t value);


/**
 * Read:
 * This is the threshold for detecting slower movements between
 * frames. Note: This value is a factor 1000 larger than the RSS
 * value.
 */
void presence_reg_read_inter_detection_threshold(uint32_t *value);


/**
 * Write:
 * This is the threshold for detecting slower movements between
 * frames. Note: This value is a factor 1000 larger than the RSS
 * value.
 */
bool presence_reg_write_inter_detection_threshold(const uint32_t value);


/**
 * Read:
 * The time constant of the low pass filter for the inter-frame
 * deviation between fast and slow. Note: This value is a factor 1000
 * larger than the RSS value.
 */
void presence_reg_read_inter_frame_deviation_time_const(uint32_t *value);


/**
 * Write:
 * The time constant of the low pass filter for the inter-frame
 * deviation between fast and slow. Note: This value is a factor 1000
 * larger than the RSS value.
 */
bool presence_reg_write_inter_frame_deviation_time_const(const uint32_t value);


/**
 * Read:
 * The cutoff frequency of the low pass filter for the fast filtered
 * absolute sweep mean. Note: This value is a factor 1000 larger than
 * the RSS value.
 */
void presence_reg_read_inter_frame_fast_cutoff(uint32_t *value);


/**
 * Write:
 * The cutoff frequency of the low pass filter for the fast filtered
 * absolute sweep mean. Note: This value is a factor 1000 larger than
 * the RSS value.
 */
bool presence_reg_write_inter_frame_fast_cutoff(const uint32_t value);


/**
 * Read:
 * The cutoff frequency of the low pass filter for the slow filtered
 * absolute sweep mean. Note: This value is a factor 1000 larger than
 * the RSS value.
 */
void presence_reg_read_inter_frame_slow_cutoff(uint32_t *value);


/**
 * Write:
 * The cutoff frequency of the low pass filter for the slow filtered
 * absolute sweep mean. Note: This value is a factor 1000 larger than
 * the RSS value.
 */
bool presence_reg_write_inter_frame_slow_cutoff(const uint32_t value);


/**
 * Read:
 * The time constant for the depthwise filtering in the intra-frame
 * part. Note: This value is a factor 1000 larger than the RSS value.
 */
void presence_reg_read_intra_frame_time_const(uint32_t *value);


/**
 * Write:
 * The time constant for the depthwise filtering in the intra-frame
 * part. Note: This value is a factor 1000 larger than the RSS value.
 */
bool presence_reg_write_intra_frame_time_const(const uint32_t value);


/**
 * Read:
 * The time constant for the output in the intra-frame part. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
void presence_reg_read_intra_output_time_const(uint32_t *value);


/**
 * Write:
 * The time constant for the output in the intra-frame part. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
bool presence_reg_write_intra_output_time_const(const uint32_t value);


/**
 * Read:
 * The time constant for the output in the inter-frame part. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
void presence_reg_read_inter_output_time_const(uint32_t *value);


/**
 * Write:
 * The time constant for the output in the inter-frame part. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
bool presence_reg_write_inter_output_time_const(const uint32_t value);


/**
 * Read:
 * Enable/Disable automatic selection of profile based on start point
 * of measurement.
 */
void presence_reg_read_auto_profile_enabled(uint32_t *value);


/**
 * Write:
 * Enable/Disable automatic selection of profile based on start point
 * of measurement.
 */
bool presence_reg_write_auto_profile_enabled(const uint32_t value);


/**
 * Read:
 * Enable/Disable automatic selection of step length based on the
 * profile.
 */
void presence_reg_read_auto_step_length_enabled(uint32_t *value);


/**
 * Write:
 * Enable/Disable automatic selection of step length based on the
 * profile.
 */
bool presence_reg_write_auto_step_length_enabled(const uint32_t value);


/**
 * Read:
 * The profile to use. The profile will only be used if profile auto
 * selection was disabled.
 */
void presence_reg_read_manual_profile(uint32_t *value);


/**
 * Write:
 * The profile to use. The profile will only be used if profile auto
 * selection was disabled.
 */
bool presence_reg_write_manual_profile(const uint32_t value);


/**
 * Read:
 * The number of steps between each data point. The manual step length
 * will only be used if step length auto selection was disabled.
 */
void presence_reg_read_manual_step_length(uint32_t *value);


/**
 * Write:
 * The number of steps between each data point. The manual step length
 * will only be used if step length auto selection was disabled.
 */
bool presence_reg_write_manual_step_length(const uint32_t value);


/**
 * Read:
 * The start point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
void presence_reg_read_start(uint32_t *value);


/**
 * Write:
 * The start point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
bool presence_reg_write_start(const uint32_t value);


/**
 * Read:
 * The end point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
void presence_reg_read_end(uint32_t *value);


/**
 * Write:
 * The end point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
bool presence_reg_write_end(const uint32_t value);


/**
 * Read:
 * Enable/Disable reset of the presence filters during start/restart.
 */
void presence_reg_read_reset_filters_on_prepare(uint32_t *value);


/**
 * Write:
 * Enable/Disable reset of the presence filters during start/restart.
 */
bool presence_reg_write_reset_filters_on_prepare(const uint32_t value);


/**
 * Read:
 * The hardware accelerated average samples (HWAAS).
 */
void presence_reg_read_hwaas(uint32_t *value);


/**
 * Write:
 * The hardware accelerated average samples (HWAAS).
 */
bool presence_reg_write_hwaas(const uint32_t value);


/**
 * Read:
 * Output presence detection on generic gpio
 */
void presence_reg_read_detection_on_gpio(uint32_t *value);


/**
 * Write:
 * Output presence detection on generic gpio
 */
bool presence_reg_write_detection_on_gpio(const uint32_t value);


/**
 * Write:
 * Execute command.
 */
bool presence_reg_write_command(const uint32_t value);


#endif
