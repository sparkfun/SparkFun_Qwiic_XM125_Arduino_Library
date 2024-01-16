// sfe_xm125_regs.h
//
// This is a library written for SparkFun Pulsed Coherent Radar Sensor - XM125 
//
// SparkFun sells these boards at its website: www.sparkfun.com
//
// Do you like this library? Help support SparkFun. Buy a board! 
//
// SparkFun Indoor Air Quality Sensor - XM125 (Qwiic)    https://www.sparkfun.com/products/
//
// Written by Madison Chodikov @ SparkFun Electronics, January 2023
//
// Repository:
//		https://github.com/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library/
//
// SparkFun code, firmware, and software is released under the MIT
// License(http://opensource.org/licenses/MIT).
//
// SPDX-License-Identifier: MIT
//
//    The MIT License (MIT)
//
//    Copyright (c) 2022 SparkFun Electronics
//    Permission is hereby granted, free of charge, to any person obtaining a
//    copy of this software and associated documentation files (the "Software"),
//    to deal in the Software without restriction, including without limitation
//    the rights to use, copy, modify, merge, publish, distribute, sublicense,
//    and/or sell copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following conditions: The
//    above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
//    "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//    NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
//    ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// The following defines the bits and registers of the ENS160.

#include <stdint.h>


/* ****************************** Distance Values ****************************** */

const uint16_t SFE_XM125_I2C_ADDRESS = 0x52;

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

// Default Value: 250
const uint16_t SFE_XM125_DISTANCE_START = 0x40;
const uint16_t sfe_xm125_distance_start_default = 250;

// Default Value: 3000
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
} sfe_xm125_threshold_method_t;

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
const uint16_t sfe_xm125_distance_fixed_amp_thresh_val_default = 100000;

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

/* ****************************** Presence Values ****************************** */

const uint16_t SFE_XM125_PRESENCE_VERSION = 0x00;
typedef struct 
{
    uint32_t presence_major : 16;
    uint32_t presence_minor : 8;
    uint32_t presence_patch : 8;
} sfe_xm125_presence_version_t;

const uint16_t SFE_XM125_DISTANCE_PROTOCOL_STATUS = 0x01;
typedef struct 
{
    uint32_t presence_protocol_state_error : 1;
    uint32_t presence_packet_length_error : 1;
    uint32_t presence_address_error : 1;
    uint32_t presence_write_failed : 1;
    uint32_t presence_write_to_read_only : 1;
    uint32_t reserved1 : 27;
} sfe_xm125_presence_protocol_status_t;

const uint16_t SFE_XM125_PRESENCE_RESULT = 0x02;

const uint16_t SFE_XM125_DISTANCE_DETECTOR_STATUS = 0x03;
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
const bool sfe_xm125_presence_inter_phase_boost_enabled_defaultS = false;

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
    XM125_PRESENCE_PROFILE1 = 4,
    XM125_PRESENCE_PROFILE1 = 5,
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

