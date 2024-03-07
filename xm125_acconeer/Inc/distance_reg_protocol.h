// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef DISTANCE_REG_PROTOCOL_H_
#define DISTANCE_REG_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

// The number of registers in the application
#define DISTANCE_REG_REGISTER_COUNT 40U

// The register address defines
#define DISTANCE_REG_VERSION_ADDRESS 0U
#define DISTANCE_REG_PROTOCOL_STATUS_ADDRESS 1U
#define DISTANCE_REG_MEASURE_COUNTER_ADDRESS 2U
#define DISTANCE_REG_DETECTOR_STATUS_ADDRESS 3U
#define DISTANCE_REG_DISTANCE_RESULT_ADDRESS 16U
#define DISTANCE_REG_PEAK0_DISTANCE_ADDRESS 17U
#define DISTANCE_REG_PEAK1_DISTANCE_ADDRESS 18U
#define DISTANCE_REG_PEAK2_DISTANCE_ADDRESS 19U
#define DISTANCE_REG_PEAK3_DISTANCE_ADDRESS 20U
#define DISTANCE_REG_PEAK4_DISTANCE_ADDRESS 21U
#define DISTANCE_REG_PEAK5_DISTANCE_ADDRESS 22U
#define DISTANCE_REG_PEAK6_DISTANCE_ADDRESS 23U
#define DISTANCE_REG_PEAK7_DISTANCE_ADDRESS 24U
#define DISTANCE_REG_PEAK8_DISTANCE_ADDRESS 25U
#define DISTANCE_REG_PEAK9_DISTANCE_ADDRESS 26U
#define DISTANCE_REG_PEAK0_STRENGTH_ADDRESS 27U
#define DISTANCE_REG_PEAK1_STRENGTH_ADDRESS 28U
#define DISTANCE_REG_PEAK2_STRENGTH_ADDRESS 29U
#define DISTANCE_REG_PEAK3_STRENGTH_ADDRESS 30U
#define DISTANCE_REG_PEAK4_STRENGTH_ADDRESS 31U
#define DISTANCE_REG_PEAK5_STRENGTH_ADDRESS 32U
#define DISTANCE_REG_PEAK6_STRENGTH_ADDRESS 33U
#define DISTANCE_REG_PEAK7_STRENGTH_ADDRESS 34U
#define DISTANCE_REG_PEAK8_STRENGTH_ADDRESS 35U
#define DISTANCE_REG_PEAK9_STRENGTH_ADDRESS 36U
#define DISTANCE_REG_START_ADDRESS 64U
#define DISTANCE_REG_END_ADDRESS 65U
#define DISTANCE_REG_MAX_STEP_LENGTH_ADDRESS 66U
#define DISTANCE_REG_CLOSE_RANGE_LEAKAGE_CANCELLATION_ADDRESS 67U
#define DISTANCE_REG_SIGNAL_QUALITY_ADDRESS 68U
#define DISTANCE_REG_MAX_PROFILE_ADDRESS 69U
#define DISTANCE_REG_THRESHOLD_METHOD_ADDRESS 70U
#define DISTANCE_REG_PEAK_SORTING_ADDRESS 71U
#define DISTANCE_REG_NUM_FRAMES_RECORDED_THRESHOLD_ADDRESS 72U
#define DISTANCE_REG_FIXED_AMPLITUDE_THRESHOLD_VALUE_ADDRESS 73U
#define DISTANCE_REG_THRESHOLD_SENSITIVITY_ADDRESS 74U
#define DISTANCE_REG_REFLECTOR_SHAPE_ADDRESS 75U
#define DISTANCE_REG_FIXED_STRENGTH_THRESHOLD_VALUE_ADDRESS 76U
#define DISTANCE_REG_MEASURE_ON_WAKEUP_ADDRESS 128U
#define DISTANCE_REG_COMMAND_ADDRESS 256U

// The defines for version fields (masks ans bit-positions)
#define DISTANCE_REG_VERSION_FIELD_MAJOR_POS  16U
#define DISTANCE_REG_VERSION_FIELD_MAJOR_MASK 0xffff0000
#define DISTANCE_REG_VERSION_FIELD_MINOR_POS  8U
#define DISTANCE_REG_VERSION_FIELD_MINOR_MASK 0x0000ff00
#define DISTANCE_REG_VERSION_FIELD_PATCH_POS  0U
#define DISTANCE_REG_VERSION_FIELD_PATCH_MASK 0x000000ff

// The defines for protocol_status fields (masks ans bit-positions)
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_POS  0U
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK 0x00000001
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_POS  1U
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK 0x00000002
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_POS  2U
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK 0x00000004
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_POS  3U
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK 0x00000008
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_POS  4U
#define DISTANCE_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK 0x00000010

// The defines for detector_status fields (masks ans bit-positions)
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_OK_POS  0U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_OK_MASK 0x00000001
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_OK_POS  1U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_OK_MASK 0x00000002
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_OK_POS  2U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_OK_MASK 0x00000004
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_OK_POS  3U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_OK_MASK 0x00000008
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_OK_POS  4U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_OK_MASK 0x00000010
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_OK_POS  5U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_OK_MASK 0x00000020
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CALIBRATION_BUFFER_OK_POS  6U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CALIBRATION_BUFFER_OK_MASK 0x00000040
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_OK_POS  7U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_OK_MASK 0x00000080
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_OK_POS  8U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK 0x00000100
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CALIBRATE_OK_POS  9U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CALIBRATE_OK_MASK 0x00000200
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_ERROR_POS  16U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_RSS_REGISTER_ERROR_MASK 0x00010000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_ERROR_POS  17U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_CREATE_ERROR_MASK 0x00020000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_ERROR_POS  18U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK 0x00040000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_ERROR_POS  19U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CREATE_ERROR_MASK 0x00080000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_ERROR_POS  20U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_BUFFER_ERROR_MASK 0x00100000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_ERROR_POS  21U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK 0x00200000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CALIBRATION_BUFFER_ERROR_POS  22U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CALIBRATION_BUFFER_ERROR_MASK 0x00400000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_ERROR_POS  23U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK 0x00800000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_POS  24U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK 0x01000000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CALIBRATE_ERROR_POS  25U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_CALIBRATE_ERROR_MASK 0x02000000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_POS  28U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_DETECTOR_ERROR_MASK 0x10000000
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_BUSY_POS  31U
#define DISTANCE_REG_DETECTOR_STATUS_FIELD_BUSY_MASK 0x80000000

// The defines for distance_result fields (masks ans bit-positions)
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_NUM_DISTANCES_POS  0U
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_NUM_DISTANCES_MASK 0x0000000f
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_NEAR_START_EDGE_POS  8U
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_NEAR_START_EDGE_MASK 0x00000100
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_CALIBRATION_NEEDED_POS  9U
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_CALIBRATION_NEEDED_MASK 0x00000200
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_MEASURE_DISTANCE_ERROR_POS  10U
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_MEASURE_DISTANCE_ERROR_MASK 0x00000400
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_TEMPERATURE_POS  16U
#define DISTANCE_REG_DISTANCE_RESULT_FIELD_TEMPERATURE_MASK 0xffff0000

// The defines for max_profile enum values
#define DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE1 1U
#define DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE2 2U
#define DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE3 3U
#define DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE4 4U
#define DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE5 5U

// The defines for threshold_method enum values
#define DISTANCE_REG_THRESHOLD_METHOD_ENUM_FIXED_AMPLITUDE 1U
#define DISTANCE_REG_THRESHOLD_METHOD_ENUM_RECORDED 2U
#define DISTANCE_REG_THRESHOLD_METHOD_ENUM_CFAR 3U
#define DISTANCE_REG_THRESHOLD_METHOD_ENUM_FIXED_STRENGTH 4U

// The defines for peak_sorting enum values
#define DISTANCE_REG_PEAK_SORTING_ENUM_CLOSEST 1U
#define DISTANCE_REG_PEAK_SORTING_ENUM_STRONGEST 2U

// The defines for reflector_shape enum values
#define DISTANCE_REG_REFLECTOR_SHAPE_ENUM_GENERIC 1U
#define DISTANCE_REG_REFLECTOR_SHAPE_ENUM_PLANAR 2U

// The defines for command enum values
#define DISTANCE_REG_COMMAND_ENUM_APPLY_CONFIG_AND_CALIBRATE 1U
#define DISTANCE_REG_COMMAND_ENUM_MEASURE_DISTANCE 2U
#define DISTANCE_REG_COMMAND_ENUM_APPLY_CONFIGURATION 3U
#define DISTANCE_REG_COMMAND_ENUM_CALIBRATE 4U
#define DISTANCE_REG_COMMAND_ENUM_RECALIBRATE 5U
#define DISTANCE_REG_COMMAND_ENUM_ENABLE_UART_LOGS 32U
#define DISTANCE_REG_COMMAND_ENUM_DISABLE_UART_LOGS 33U
#define DISTANCE_REG_COMMAND_ENUM_LOG_CONFIGURATION 34U
#define DISTANCE_REG_COMMAND_ENUM_RESET_MODULE 0x52535421

// The config default defines
#define DISTANCE_REG_START_DEFAULT_VALUE 250U
#define DISTANCE_REG_END_DEFAULT_VALUE 3000U
#define DISTANCE_REG_MAX_STEP_LENGTH_DEFAULT_VALUE 0U
#define DISTANCE_REG_CLOSE_RANGE_LEAKAGE_CANCELLATION_DEFAULT_VALUE 1U
#define DISTANCE_REG_SIGNAL_QUALITY_DEFAULT_VALUE 15000U
#define DISTANCE_REG_MAX_PROFILE_DEFAULT_VALUE DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE5
#define DISTANCE_REG_THRESHOLD_METHOD_DEFAULT_VALUE DISTANCE_REG_THRESHOLD_METHOD_ENUM_CFAR
#define DISTANCE_REG_PEAK_SORTING_DEFAULT_VALUE DISTANCE_REG_PEAK_SORTING_ENUM_STRONGEST
#define DISTANCE_REG_NUM_FRAMES_RECORDED_THRESHOLD_DEFAULT_VALUE 100U
#define DISTANCE_REG_FIXED_AMPLITUDE_THRESHOLD_VALUE_DEFAULT_VALUE 100000U
#define DISTANCE_REG_THRESHOLD_SENSITIVITY_DEFAULT_VALUE 500U
#define DISTANCE_REG_REFLECTOR_SHAPE_DEFAULT_VALUE DISTANCE_REG_REFLECTOR_SHAPE_ENUM_GENERIC
#define DISTANCE_REG_FIXED_STRENGTH_THRESHOLD_VALUE_DEFAULT_VALUE 0U
#define DISTANCE_REG_MEASURE_ON_WAKEUP_DEFAULT_VALUE 0U


/**
 * The distance protocol setup function.
 */
void distance_reg_protocol_setup(void);


/**
 * The distance protocol write default register value function.
 */
void distance_reg_protocol_write_default(void);


/**
 * Read:
 * Get the RSS version.
 */
void distance_reg_read_version(uint32_t *value);


/**
 * Read:
 * Get protocol error flags.
 */
void distance_reg_read_protocol_status(uint32_t *value);


/**
 * Read:
 * Get the measure counter, the number of measurements performed since
 * restart.
 */
void distance_reg_read_measure_counter(uint32_t *value);


/**
 * Read:
 * Get detector status flags.
 */
void distance_reg_read_detector_status(uint32_t *value);


/**
 * Read:
 * The result from the distance detector.
 */
void distance_reg_read_distance_result(uint32_t *value);


/**
 * Read:
 * The distance to peak 0. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak0_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 1. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak1_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 2. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak2_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 3. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak3_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 4. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak4_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 5. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak5_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 6. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak6_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 7. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak7_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 8. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak8_distance(uint32_t *value);


/**
 * Read:
 * The distance to peak 9. Note: This value is a factor 1000 larger
 * than the RSS value.
 */
void distance_reg_read_peak9_distance(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 0. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak0_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 1. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak1_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 2. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak2_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 3. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak3_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 4. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak4_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 5. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak5_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 6. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak6_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 7. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak7_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 8. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak8_strength(uint32_t *value);


/**
 * Read:
 * The reflective strength of peak 9. Note: This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_peak9_strength(uint32_t *value);


/**
 * Read:
 * The start of measured interval in millimeters. Note: This value is
 * a factor 1000 larger than the RSS value.
 */
void distance_reg_read_start(uint32_t *value);


/**
 * Write:
 * The start of measured interval in millimeters. Note: This value is
 * a factor 1000 larger than the RSS value.
 */
bool distance_reg_write_start(const uint32_t value);


/**
 * Read:
 * The end of measured interval in millimeters. Note: This value is a
 * factor 1000 larger than the RSS value.
 */
void distance_reg_read_end(uint32_t *value);


/**
 * Write:
 * The end of measured interval in millimeters. Note: This value is a
 * factor 1000 larger than the RSS value.
 */
bool distance_reg_write_end(const uint32_t value);


/**
 * Read:
 * Used to limit step length. If set to 0 (default), the step length
 * is calculated based on profile.
 */
void distance_reg_read_max_step_length(uint32_t *value);


/**
 * Write:
 * Used to limit step length. If set to 0 (default), the step length
 * is calculated based on profile.
 */
bool distance_reg_write_max_step_length(const uint32_t value);


/**
 * Read:
 * Enable the close range leakage cancellation logic.
 */
void distance_reg_read_close_range_leakage_cancellation(uint32_t *value);


/**
 * Write:
 * Enable the close range leakage cancellation logic.
 */
bool distance_reg_write_close_range_leakage_cancellation(const uint32_t value);


/**
 * Read:
 * High signal quality results in a better SNR (because of higher
 * HWAAS) and higher power consumption. Note:This value is a factor
 * 1000 larger than the RSS value.
 */
void distance_reg_read_signal_quality(uint32_t *value);


/**
 * Write:
 * High signal quality results in a better SNR (because of higher
 * HWAAS) and higher power consumption. Note:This value is a factor
 * 1000 larger than the RSS value.
 */
bool distance_reg_write_signal_quality(const uint32_t value);


/**
 * Read:
 * Max profile.
 */
void distance_reg_read_max_profile(uint32_t *value);


/**
 * Write:
 * Max profile.
 */
bool distance_reg_write_max_profile(const uint32_t value);


/**
 * Read:
 * Threshold method.
 */
void distance_reg_read_threshold_method(uint32_t *value);


/**
 * Write:
 * Threshold method.
 */
bool distance_reg_write_threshold_method(const uint32_t value);


/**
 * Read:
 * Peak sorting method.
 */
void distance_reg_read_peak_sorting(uint32_t *value);


/**
 * Write:
 * Peak sorting method.
 */
bool distance_reg_write_peak_sorting(const uint32_t value);


/**
 * Read:
 * The number frames to use for recorded threshold.
 */
void distance_reg_read_num_frames_recorded_threshold(uint32_t *value);


/**
 * Write:
 * The number frames to use for recorded threshold.
 */
bool distance_reg_write_num_frames_recorded_threshold(const uint32_t value);


/**
 * Read:
 * Fixed amplitude threshold value Note: This value is a factor 1000
 * larger than the RSS value.
 */
void distance_reg_read_fixed_amplitude_threshold_value(uint32_t *value);


/**
 * Write:
 * Fixed amplitude threshold value Note: This value is a factor 1000
 * larger than the RSS value.
 */
bool distance_reg_write_fixed_amplitude_threshold_value(const uint32_t value);


/**
 * Read:
 * Threshold sensitivity (0 <= sensitivity <= 1000) Note: This value
 * is a factor 1000 larger than the RSS value.
 */
void distance_reg_read_threshold_sensitivity(uint32_t *value);


/**
 * Write:
 * Threshold sensitivity (0 <= sensitivity <= 1000) Note: This value
 * is a factor 1000 larger than the RSS value.
 */
bool distance_reg_write_threshold_sensitivity(const uint32_t value);


/**
 * Read:
 * Reflector shape.
 */
void distance_reg_read_reflector_shape(uint32_t *value);


/**
 * Write:
 * Reflector shape.
 */
bool distance_reg_write_reflector_shape(const uint32_t value);


/**
 * Read:
 * Fixed strength threshold value Note: This value is a factor 1000
 * larger than the RSS value.
 */
void distance_reg_read_fixed_strength_threshold_value(uint32_t *value);


/**
 * Write:
 * Fixed strength threshold value Note: This value is a factor 1000
 * larger than the RSS value.
 */
bool distance_reg_write_fixed_strength_threshold_value(const uint32_t value);


/**
 * Read:
 * Perform measure on wake up.
 */
void distance_reg_read_measure_on_wakeup(uint32_t *value);


/**
 * Write:
 * Perform measure on wake up.
 */
bool distance_reg_write_measure_on_wakeup(const uint32_t value);


/**
 * Write:
 * Execute command.
 */
bool distance_reg_write_command(const uint32_t value);


#endif
