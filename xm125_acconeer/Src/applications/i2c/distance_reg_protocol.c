// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_reg_protocol.h"
#include "distance_reg_protocol.h"


/**
 * This struct defines the links between a register address and the register read and/or write function.
 */
const acc_reg_protocol_t distance_reg_protocol[DISTANCE_REG_REGISTER_COUNT] = {
    {
        DISTANCE_REG_VERSION_ADDRESS,  // version address
        distance_reg_read_version,     // version read access function
        NULL,
    },
    {
        DISTANCE_REG_PROTOCOL_STATUS_ADDRESS,  // protocol_status address
        distance_reg_read_protocol_status,     // protocol_status read access function
        NULL,
    },
    {
        DISTANCE_REG_MEASURE_COUNTER_ADDRESS,  // measure_counter address
        distance_reg_read_measure_counter,     // measure_counter read access function
        NULL,
    },
    {
        DISTANCE_REG_DETECTOR_STATUS_ADDRESS,  // detector_status address
        distance_reg_read_detector_status,     // detector_status read access function
        NULL,
    },
    {
        DISTANCE_REG_DISTANCE_RESULT_ADDRESS,  // distance_result address
        distance_reg_read_distance_result,     // distance_result read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK0_DISTANCE_ADDRESS,  // peak0_distance address
        distance_reg_read_peak0_distance,     // peak0_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK1_DISTANCE_ADDRESS,  // peak1_distance address
        distance_reg_read_peak1_distance,     // peak1_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK2_DISTANCE_ADDRESS,  // peak2_distance address
        distance_reg_read_peak2_distance,     // peak2_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK3_DISTANCE_ADDRESS,  // peak3_distance address
        distance_reg_read_peak3_distance,     // peak3_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK4_DISTANCE_ADDRESS,  // peak4_distance address
        distance_reg_read_peak4_distance,     // peak4_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK5_DISTANCE_ADDRESS,  // peak5_distance address
        distance_reg_read_peak5_distance,     // peak5_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK6_DISTANCE_ADDRESS,  // peak6_distance address
        distance_reg_read_peak6_distance,     // peak6_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK7_DISTANCE_ADDRESS,  // peak7_distance address
        distance_reg_read_peak7_distance,     // peak7_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK8_DISTANCE_ADDRESS,  // peak8_distance address
        distance_reg_read_peak8_distance,     // peak8_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK9_DISTANCE_ADDRESS,  // peak9_distance address
        distance_reg_read_peak9_distance,     // peak9_distance read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK0_STRENGTH_ADDRESS,  // peak0_strength address
        distance_reg_read_peak0_strength,     // peak0_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK1_STRENGTH_ADDRESS,  // peak1_strength address
        distance_reg_read_peak1_strength,     // peak1_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK2_STRENGTH_ADDRESS,  // peak2_strength address
        distance_reg_read_peak2_strength,     // peak2_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK3_STRENGTH_ADDRESS,  // peak3_strength address
        distance_reg_read_peak3_strength,     // peak3_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK4_STRENGTH_ADDRESS,  // peak4_strength address
        distance_reg_read_peak4_strength,     // peak4_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK5_STRENGTH_ADDRESS,  // peak5_strength address
        distance_reg_read_peak5_strength,     // peak5_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK6_STRENGTH_ADDRESS,  // peak6_strength address
        distance_reg_read_peak6_strength,     // peak6_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK7_STRENGTH_ADDRESS,  // peak7_strength address
        distance_reg_read_peak7_strength,     // peak7_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK8_STRENGTH_ADDRESS,  // peak8_strength address
        distance_reg_read_peak8_strength,     // peak8_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_PEAK9_STRENGTH_ADDRESS,  // peak9_strength address
        distance_reg_read_peak9_strength,     // peak9_strength read access function
        NULL,
    },
    {
        DISTANCE_REG_START_ADDRESS,  // start address
        distance_reg_read_start,     // start read access function
        distance_reg_write_start,    // start write access function
    },
    {
        DISTANCE_REG_END_ADDRESS,  // end address
        distance_reg_read_end,     // end read access function
        distance_reg_write_end,    // end write access function
    },
    {
        DISTANCE_REG_MAX_STEP_LENGTH_ADDRESS,  // max_step_length address
        distance_reg_read_max_step_length,     // max_step_length read access function
        distance_reg_write_max_step_length,    // max_step_length write access function
    },
    {
        DISTANCE_REG_CLOSE_RANGE_LEAKAGE_CANCELLATION_ADDRESS,  // close_range_leakage_cancellation address
        distance_reg_read_close_range_leakage_cancellation,     // close_range_leakage_cancellation read access function
        distance_reg_write_close_range_leakage_cancellation,    // close_range_leakage_cancellation write access function
    },
    {
        DISTANCE_REG_SIGNAL_QUALITY_ADDRESS,  // signal_quality address
        distance_reg_read_signal_quality,     // signal_quality read access function
        distance_reg_write_signal_quality,    // signal_quality write access function
    },
    {
        DISTANCE_REG_MAX_PROFILE_ADDRESS,  // max_profile address
        distance_reg_read_max_profile,     // max_profile read access function
        distance_reg_write_max_profile,    // max_profile write access function
    },
    {
        DISTANCE_REG_THRESHOLD_METHOD_ADDRESS,  // threshold_method address
        distance_reg_read_threshold_method,     // threshold_method read access function
        distance_reg_write_threshold_method,    // threshold_method write access function
    },
    {
        DISTANCE_REG_PEAK_SORTING_ADDRESS,  // peak_sorting address
        distance_reg_read_peak_sorting,     // peak_sorting read access function
        distance_reg_write_peak_sorting,    // peak_sorting write access function
    },
    {
        DISTANCE_REG_NUM_FRAMES_RECORDED_THRESHOLD_ADDRESS,  // num_frames_recorded_threshold address
        distance_reg_read_num_frames_recorded_threshold,     // num_frames_recorded_threshold read access function
        distance_reg_write_num_frames_recorded_threshold,    // num_frames_recorded_threshold write access function
    },
    {
        DISTANCE_REG_FIXED_AMPLITUDE_THRESHOLD_VALUE_ADDRESS,  // fixed_amplitude_threshold_value address
        distance_reg_read_fixed_amplitude_threshold_value,     // fixed_amplitude_threshold_value read access function
        distance_reg_write_fixed_amplitude_threshold_value,    // fixed_amplitude_threshold_value write access function
    },
    {
        DISTANCE_REG_THRESHOLD_SENSITIVITY_ADDRESS,  // threshold_sensitivity address
        distance_reg_read_threshold_sensitivity,     // threshold_sensitivity read access function
        distance_reg_write_threshold_sensitivity,    // threshold_sensitivity write access function
    },
    {
        DISTANCE_REG_REFLECTOR_SHAPE_ADDRESS,  // reflector_shape address
        distance_reg_read_reflector_shape,     // reflector_shape read access function
        distance_reg_write_reflector_shape,    // reflector_shape write access function
    },
    {
        DISTANCE_REG_FIXED_STRENGTH_THRESHOLD_VALUE_ADDRESS,  // fixed_strength_threshold_value address
        distance_reg_read_fixed_strength_threshold_value,     // fixed_strength_threshold_value read access function
        distance_reg_write_fixed_strength_threshold_value,    // fixed_strength_threshold_value write access function
    },
    {
        DISTANCE_REG_MEASURE_ON_WAKEUP_ADDRESS,  // measure_on_wakeup address
        distance_reg_read_measure_on_wakeup,     // measure_on_wakeup read access function
        distance_reg_write_measure_on_wakeup,    // measure_on_wakeup write access function
    },
    {
        DISTANCE_REG_COMMAND_ADDRESS,  // command address
        NULL,
        distance_reg_write_command,    // command write access function
    },
};


void distance_reg_protocol_setup(void)
{
    acc_reg_protocol_setup(distance_reg_protocol, DISTANCE_REG_REGISTER_COUNT);
}


void distance_reg_protocol_write_default(void)
{
    distance_reg_write_start(DISTANCE_REG_START_DEFAULT_VALUE);
    distance_reg_write_end(DISTANCE_REG_END_DEFAULT_VALUE);
    distance_reg_write_max_step_length(DISTANCE_REG_MAX_STEP_LENGTH_DEFAULT_VALUE);
    distance_reg_write_close_range_leakage_cancellation(DISTANCE_REG_CLOSE_RANGE_LEAKAGE_CANCELLATION_DEFAULT_VALUE);
    distance_reg_write_signal_quality(DISTANCE_REG_SIGNAL_QUALITY_DEFAULT_VALUE);
    distance_reg_write_max_profile(DISTANCE_REG_MAX_PROFILE_DEFAULT_VALUE);
    distance_reg_write_threshold_method(DISTANCE_REG_THRESHOLD_METHOD_DEFAULT_VALUE);
    distance_reg_write_peak_sorting(DISTANCE_REG_PEAK_SORTING_DEFAULT_VALUE);
    distance_reg_write_num_frames_recorded_threshold(DISTANCE_REG_NUM_FRAMES_RECORDED_THRESHOLD_DEFAULT_VALUE);
    distance_reg_write_fixed_amplitude_threshold_value(DISTANCE_REG_FIXED_AMPLITUDE_THRESHOLD_VALUE_DEFAULT_VALUE);
    distance_reg_write_threshold_sensitivity(DISTANCE_REG_THRESHOLD_SENSITIVITY_DEFAULT_VALUE);
    distance_reg_write_reflector_shape(DISTANCE_REG_REFLECTOR_SHAPE_DEFAULT_VALUE);
    distance_reg_write_fixed_strength_threshold_value(DISTANCE_REG_FIXED_STRENGTH_THRESHOLD_VALUE_DEFAULT_VALUE);
    distance_reg_write_measure_on_wakeup(DISTANCE_REG_MEASURE_ON_WAKEUP_DEFAULT_VALUE);
}
