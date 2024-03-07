// Copyright (c) Acconeer AB, 2024
// All rights reserved

#ifndef REF_APP_BREATHING_REG_PROTOCOL_H_
#define REF_APP_BREATHING_REG_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

// The number of registers in the application
#define REF_APP_BREATHING_REG_REGISTER_COUNT 21U

// The register address defines
#define REF_APP_BREATHING_REG_VERSION_ADDRESS 0U
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_ADDRESS 1U
#define REF_APP_BREATHING_REG_MEASURE_COUNTER_ADDRESS 2U
#define REF_APP_BREATHING_REG_APP_STATUS_ADDRESS 3U
#define REF_APP_BREATHING_REG_BREATHING_RESULT_ADDRESS 16U
#define REF_APP_BREATHING_REG_BREATHING_RATE_ADDRESS 17U
#define REF_APP_BREATHING_REG_APP_STATE_ADDRESS 18U
#define REF_APP_BREATHING_REG_START_ADDRESS 64U
#define REF_APP_BREATHING_REG_END_ADDRESS 65U
#define REF_APP_BREATHING_REG_NUM_DISTANCES_TO_ANALYZE_ADDRESS 66U
#define REF_APP_BREATHING_REG_DISTANCE_DETERMINATION_DURATION_S_ADDRESS 67U
#define REF_APP_BREATHING_REG_USE_PRESENCE_PROCESSOR_ADDRESS 68U
#define REF_APP_BREATHING_REG_LOWEST_BREATHING_RATE_ADDRESS 69U
#define REF_APP_BREATHING_REG_HIGHEST_BREATHING_RATE_ADDRESS 70U
#define REF_APP_BREATHING_REG_TIME_SERIES_LENGTH_S_ADDRESS 71U
#define REF_APP_BREATHING_REG_FRAME_RATE_ADDRESS 72U
#define REF_APP_BREATHING_REG_SWEEPS_PER_FRAME_ADDRESS 73U
#define REF_APP_BREATHING_REG_HWAAS_ADDRESS 74U
#define REF_APP_BREATHING_REG_PROFILE_ADDRESS 75U
#define REF_APP_BREATHING_REG_INTRA_DETECTION_THRESHOLD_ADDRESS 76U
#define REF_APP_BREATHING_REG_COMMAND_ADDRESS 256U

// The defines for version fields (masks ans bit-positions)
#define REF_APP_BREATHING_REG_VERSION_FIELD_MAJOR_POS  16U
#define REF_APP_BREATHING_REG_VERSION_FIELD_MAJOR_MASK 0xffff0000
#define REF_APP_BREATHING_REG_VERSION_FIELD_MINOR_POS  8U
#define REF_APP_BREATHING_REG_VERSION_FIELD_MINOR_MASK 0x0000ff00
#define REF_APP_BREATHING_REG_VERSION_FIELD_PATCH_POS  0U
#define REF_APP_BREATHING_REG_VERSION_FIELD_PATCH_MASK 0x000000ff

// The defines for protocol_status fields (masks ans bit-positions)
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_POS  0U
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK 0x00000001
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_POS  1U
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK 0x00000002
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_POS  2U
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK 0x00000004
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_POS  3U
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK 0x00000008
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_POS  4U
#define REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK 0x00000010

// The defines for app_status fields (masks ans bit-positions)
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_RSS_REGISTER_OK_POS  0U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_RSS_REGISTER_OK_MASK 0x00000001
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_CREATE_OK_POS  1U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_CREATE_OK_MASK 0x00000002
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CREATE_OK_POS  2U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CREATE_OK_MASK 0x00000004
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_OK_POS  3U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK 0x00000008
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_CREATE_OK_POS  4U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_CREATE_OK_MASK 0x00000010
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_BUFFER_OK_POS  5U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_BUFFER_OK_MASK 0x00000020
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_BUFFER_OK_POS  6U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_BUFFER_OK_MASK 0x00000040
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_OK_POS  7U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_OK_MASK 0x00000080
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_RSS_REGISTER_ERROR_POS  16U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_RSS_REGISTER_ERROR_MASK 0x00010000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_CREATE_ERROR_POS  17U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_CREATE_ERROR_MASK 0x00020000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CREATE_ERROR_POS  18U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK 0x00040000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_POS  19U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK 0x00080000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_CREATE_ERROR_POS  20U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_CREATE_ERROR_MASK 0x00100000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_BUFFER_ERROR_POS  21U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_BUFFER_ERROR_MASK 0x00200000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_BUFFER_ERROR_POS  22U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK 0x00400000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_ERROR_POS  23U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK 0x00800000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_POS  28U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_APP_ERROR_MASK 0x10000000
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_BUSY_POS  31U
#define REF_APP_BREATHING_REG_APP_STATUS_FIELD_BUSY_MASK 0x80000000

// The defines for breathing_result fields (masks ans bit-positions)
#define REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_RESULT_READY_POS  0U
#define REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_RESULT_READY_MASK 0x00000001
#define REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_RESULT_READY_STICKY_POS  1U
#define REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_RESULT_READY_STICKY_MASK 0x00000002
#define REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_TEMPERATURE_POS  16U
#define REF_APP_BREATHING_REG_BREATHING_RESULT_FIELD_TEMPERATURE_MASK 0xffff0000

// The defines for app_state enum values
#define REF_APP_BREATHING_REG_APP_STATE_ENUM_INIT 0U
#define REF_APP_BREATHING_REG_APP_STATE_ENUM_NO_PRESENCE 1U
#define REF_APP_BREATHING_REG_APP_STATE_ENUM_INTRA_PRESENCE 2U
#define REF_APP_BREATHING_REG_APP_STATE_ENUM_DETERMINE_DISTANCE 3U
#define REF_APP_BREATHING_REG_APP_STATE_ENUM_ESTIMATE_BREATHING_RATE 4U

// The defines for profile enum values
#define REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE1 1U
#define REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE2 2U
#define REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE3 3U
#define REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE4 4U
#define REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE5 5U

// The defines for command enum values
#define REF_APP_BREATHING_REG_COMMAND_ENUM_APPLY_CONFIGURATION 1U
#define REF_APP_BREATHING_REG_COMMAND_ENUM_START_APP 2U
#define REF_APP_BREATHING_REG_COMMAND_ENUM_STOP_APP 3U
#define REF_APP_BREATHING_REG_COMMAND_ENUM_ENABLE_UART_LOGS 32U
#define REF_APP_BREATHING_REG_COMMAND_ENUM_DISABLE_UART_LOGS 33U
#define REF_APP_BREATHING_REG_COMMAND_ENUM_LOG_CONFIGURATION 34U
#define REF_APP_BREATHING_REG_COMMAND_ENUM_RESET_MODULE 0x52535421

// The config default defines
#define REF_APP_BREATHING_REG_START_DEFAULT_VALUE 300U
#define REF_APP_BREATHING_REG_END_DEFAULT_VALUE 1500U
#define REF_APP_BREATHING_REG_NUM_DISTANCES_TO_ANALYZE_DEFAULT_VALUE 3U
#define REF_APP_BREATHING_REG_DISTANCE_DETERMINATION_DURATION_S_DEFAULT_VALUE 5U
#define REF_APP_BREATHING_REG_USE_PRESENCE_PROCESSOR_DEFAULT_VALUE 1U
#define REF_APP_BREATHING_REG_LOWEST_BREATHING_RATE_DEFAULT_VALUE 6U
#define REF_APP_BREATHING_REG_HIGHEST_BREATHING_RATE_DEFAULT_VALUE 60U
#define REF_APP_BREATHING_REG_TIME_SERIES_LENGTH_S_DEFAULT_VALUE 20U
#define REF_APP_BREATHING_REG_FRAME_RATE_DEFAULT_VALUE 10000U
#define REF_APP_BREATHING_REG_SWEEPS_PER_FRAME_DEFAULT_VALUE 16U
#define REF_APP_BREATHING_REG_HWAAS_DEFAULT_VALUE 32U
#define REF_APP_BREATHING_REG_PROFILE_DEFAULT_VALUE REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE3
#define REF_APP_BREATHING_REG_INTRA_DETECTION_THRESHOLD_DEFAULT_VALUE 6000U


/**
 * The ref_app_breathing protocol setup function.
 */
void ref_app_breathing_reg_protocol_setup(void);


/**
 * The ref_app_breathing protocol write default register value function.
 */
void ref_app_breathing_reg_protocol_write_default(void);


/**
 * Read:
 * Get the RSS version.
 */
void ref_app_breathing_reg_read_version(uint32_t *value);


/**
 * Read:
 * Get protocol error flags.
 */
void ref_app_breathing_reg_read_protocol_status(uint32_t *value);


/**
 * Read:
 * Get the measure counter, the number of measurements performed since
 * restart.
 */
void ref_app_breathing_reg_read_measure_counter(uint32_t *value);


/**
 * Read:
 * Get application status flags.
 */
void ref_app_breathing_reg_read_app_status(uint32_t *value);


/**
 * Read:
 * The result from the breathing reference application.
 */
void ref_app_breathing_reg_read_breathing_result(uint32_t *value);


/**
 * Read:
 * The breathing rate. 0 if no breathing rate available. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
void ref_app_breathing_reg_read_breathing_rate(uint32_t *value);


/**
 * Read:
 * The current state of the application.
 */
void ref_app_breathing_reg_read_app_state(uint32_t *value);


/**
 * Read:
 * The start point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
void ref_app_breathing_reg_read_start(uint32_t *value);


/**
 * Write:
 * The start point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
bool ref_app_breathing_reg_write_start(const uint32_t value);


/**
 * Read:
 * The end point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
void ref_app_breathing_reg_read_end(uint32_t *value);


/**
 * Write:
 * The end point of measurement interval in millimeters. Note: This
 * value is a factor 1000 larger than the RSS value.
 */
bool ref_app_breathing_reg_write_end(const uint32_t value);


/**
 * Read:
 * Number of distance points to analyze in breathing.
 */
void ref_app_breathing_reg_read_num_distances_to_analyze(uint32_t *value);


/**
 * Write:
 * Number of distance points to analyze in breathing.
 */
bool ref_app_breathing_reg_write_num_distances_to_analyze(const uint32_t value);


/**
 * Read:
 * Time to determine distance to presence in seconds.
 */
void ref_app_breathing_reg_read_distance_determination_duration_s(uint32_t *value);


/**
 * Write:
 * Time to determine distance to presence in seconds.
 */
bool ref_app_breathing_reg_write_distance_determination_duration_s(const uint32_t value);


/**
 * Read:
 * Use presence detector to determine distance to motion.
 */
void ref_app_breathing_reg_read_use_presence_processor(uint32_t *value);


/**
 * Write:
 * Use presence detector to determine distance to motion.
 */
bool ref_app_breathing_reg_write_use_presence_processor(const uint32_t value);


/**
 * Read:
 * Lowest anticipated breathing rate in breaths per minute.
 */
void ref_app_breathing_reg_read_lowest_breathing_rate(uint32_t *value);


/**
 * Write:
 * Lowest anticipated breathing rate in breaths per minute.
 */
bool ref_app_breathing_reg_write_lowest_breathing_rate(const uint32_t value);


/**
 * Read:
 * Highest anticipated breathing rate in breaths per minute.
 */
void ref_app_breathing_reg_read_highest_breathing_rate(uint32_t *value);


/**
 * Write:
 * Highest anticipated breathing rate in breaths per minute.
 */
bool ref_app_breathing_reg_write_highest_breathing_rate(const uint32_t value);


/**
 * Read:
 * Length of time series in seconds.
 */
void ref_app_breathing_reg_read_time_series_length_s(uint32_t *value);


/**
 * Write:
 * Length of time series in seconds.
 */
bool ref_app_breathing_reg_write_time_series_length_s(const uint32_t value);


/**
 * Read:
 * The presence detector frame rate. Note: This value is a factor 1000
 * larger than the RSS value.
 */
void ref_app_breathing_reg_read_frame_rate(uint32_t *value);


/**
 * Write:
 * The presence detector frame rate. Note: This value is a factor 1000
 * larger than the RSS value.
 */
bool ref_app_breathing_reg_write_frame_rate(const uint32_t value);


/**
 * Read:
 * The number of sweeps that will be captured in each frame
 * (measurement).
 */
void ref_app_breathing_reg_read_sweeps_per_frame(uint32_t *value);


/**
 * Write:
 * The number of sweeps that will be captured in each frame
 * (measurement).
 */
bool ref_app_breathing_reg_write_sweeps_per_frame(const uint32_t value);


/**
 * Read:
 * The hardware accelerated average samples (HWAAS).
 */
void ref_app_breathing_reg_read_hwaas(uint32_t *value);


/**
 * Write:
 * The hardware accelerated average samples (HWAAS).
 */
bool ref_app_breathing_reg_write_hwaas(const uint32_t value);


/**
 * Read:
 * The profile to use.
 */
void ref_app_breathing_reg_read_profile(uint32_t *value);


/**
 * Write:
 * The profile to use.
 */
bool ref_app_breathing_reg_write_profile(const uint32_t value);


/**
 * Read:
 * The threshold for detecting faster movements inside frames. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
void ref_app_breathing_reg_read_intra_detection_threshold(uint32_t *value);


/**
 * Write:
 * The threshold for detecting faster movements inside frames. Note:
 * This value is a factor 1000 larger than the RSS value.
 */
bool ref_app_breathing_reg_write_intra_detection_threshold(const uint32_t value);


/**
 * Write:
 * Execute command.
 */
bool ref_app_breathing_reg_write_command(const uint32_t value);


#endif
