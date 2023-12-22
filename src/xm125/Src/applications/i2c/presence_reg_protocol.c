// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_reg_protocol.h"
#include "presence_reg_protocol.h"


/**
 * This struct defines the links between a register address and the register read and/or write function.
 */
const acc_reg_protocol_t presence_reg_protocol[PRESENCE_REG_REGISTER_COUNT] = {
    {
        PRESENCE_REG_VERSION_ADDRESS,  // version address
        presence_reg_read_version,     // version read access function
        NULL,
    },
    {
        PRESENCE_REG_PROTOCOL_STATUS_ADDRESS,  // protocol_status address
        presence_reg_read_protocol_status,     // protocol_status read access function
        NULL,
    },
    {
        PRESENCE_REG_MEASURE_COUNTER_ADDRESS,  // measure_counter address
        presence_reg_read_measure_counter,     // measure_counter read access function
        NULL,
    },
    {
        PRESENCE_REG_DETECTOR_STATUS_ADDRESS,  // detector_status address
        presence_reg_read_detector_status,     // detector_status read access function
        NULL,
    },
    {
        PRESENCE_REG_PRESENCE_RESULT_ADDRESS,  // presence_result address
        presence_reg_read_presence_result,     // presence_result read access function
        NULL,
    },
    {
        PRESENCE_REG_PRESENCE_DISTANCE_ADDRESS,  // presence_distance address
        presence_reg_read_presence_distance,     // presence_distance read access function
        NULL,
    },
    {
        PRESENCE_REG_INTRA_PRESENCE_SCORE_ADDRESS,  // intra_presence_score address
        presence_reg_read_intra_presence_score,     // intra_presence_score read access function
        NULL,
    },
    {
        PRESENCE_REG_INTER_PRESENCE_SCORE_ADDRESS,  // inter_presence_score address
        presence_reg_read_inter_presence_score,     // inter_presence_score read access function
        NULL,
    },
    {
        PRESENCE_REG_SWEEPS_PER_FRAME_ADDRESS,  // sweeps_per_frame address
        presence_reg_read_sweeps_per_frame,     // sweeps_per_frame read access function
        presence_reg_write_sweeps_per_frame,    // sweeps_per_frame write access function
    },
    {
        PRESENCE_REG_INTER_FRAME_PRESENCE_TIMEOUT_ADDRESS,  // inter_frame_presence_timeout address
        presence_reg_read_inter_frame_presence_timeout,     // inter_frame_presence_timeout read access function
        presence_reg_write_inter_frame_presence_timeout,    // inter_frame_presence_timeout write access function
    },
    {
        PRESENCE_REG_INTER_PHASE_BOOST_ENABLED_ADDRESS,  // inter_phase_boost_enabled address
        presence_reg_read_inter_phase_boost_enabled,     // inter_phase_boost_enabled read access function
        presence_reg_write_inter_phase_boost_enabled,    // inter_phase_boost_enabled write access function
    },
    {
        PRESENCE_REG_INTRA_DETECTION_ENABLED_ADDRESS,  // intra_detection_enabled address
        presence_reg_read_intra_detection_enabled,     // intra_detection_enabled read access function
        presence_reg_write_intra_detection_enabled,    // intra_detection_enabled write access function
    },
    {
        PRESENCE_REG_INTER_DETECTION_ENABLED_ADDRESS,  // inter_detection_enabled address
        presence_reg_read_inter_detection_enabled,     // inter_detection_enabled read access function
        presence_reg_write_inter_detection_enabled,    // inter_detection_enabled write access function
    },
    {
        PRESENCE_REG_FRAME_RATE_ADDRESS,  // frame_rate address
        presence_reg_read_frame_rate,     // frame_rate read access function
        presence_reg_write_frame_rate,    // frame_rate write access function
    },
    {
        PRESENCE_REG_INTRA_DETECTION_THRESHOLD_ADDRESS,  // intra_detection_threshold address
        presence_reg_read_intra_detection_threshold,     // intra_detection_threshold read access function
        presence_reg_write_intra_detection_threshold,    // intra_detection_threshold write access function
    },
    {
        PRESENCE_REG_INTER_DETECTION_THRESHOLD_ADDRESS,  // inter_detection_threshold address
        presence_reg_read_inter_detection_threshold,     // inter_detection_threshold read access function
        presence_reg_write_inter_detection_threshold,    // inter_detection_threshold write access function
    },
    {
        PRESENCE_REG_INTER_FRAME_DEVIATION_TIME_CONST_ADDRESS,  // inter_frame_deviation_time_const address
        presence_reg_read_inter_frame_deviation_time_const,     // inter_frame_deviation_time_const read access function
        presence_reg_write_inter_frame_deviation_time_const,    // inter_frame_deviation_time_const write access function
    },
    {
        PRESENCE_REG_INTER_FRAME_FAST_CUTOFF_ADDRESS,  // inter_frame_fast_cutoff address
        presence_reg_read_inter_frame_fast_cutoff,     // inter_frame_fast_cutoff read access function
        presence_reg_write_inter_frame_fast_cutoff,    // inter_frame_fast_cutoff write access function
    },
    {
        PRESENCE_REG_INTER_FRAME_SLOW_CUTOFF_ADDRESS,  // inter_frame_slow_cutoff address
        presence_reg_read_inter_frame_slow_cutoff,     // inter_frame_slow_cutoff read access function
        presence_reg_write_inter_frame_slow_cutoff,    // inter_frame_slow_cutoff write access function
    },
    {
        PRESENCE_REG_INTRA_FRAME_TIME_CONST_ADDRESS,  // intra_frame_time_const address
        presence_reg_read_intra_frame_time_const,     // intra_frame_time_const read access function
        presence_reg_write_intra_frame_time_const,    // intra_frame_time_const write access function
    },
    {
        PRESENCE_REG_INTRA_OUTPUT_TIME_CONST_ADDRESS,  // intra_output_time_const address
        presence_reg_read_intra_output_time_const,     // intra_output_time_const read access function
        presence_reg_write_intra_output_time_const,    // intra_output_time_const write access function
    },
    {
        PRESENCE_REG_INTER_OUTPUT_TIME_CONST_ADDRESS,  // inter_output_time_const address
        presence_reg_read_inter_output_time_const,     // inter_output_time_const read access function
        presence_reg_write_inter_output_time_const,    // inter_output_time_const write access function
    },
    {
        PRESENCE_REG_AUTO_PROFILE_ENABLED_ADDRESS,  // auto_profile_enabled address
        presence_reg_read_auto_profile_enabled,     // auto_profile_enabled read access function
        presence_reg_write_auto_profile_enabled,    // auto_profile_enabled write access function
    },
    {
        PRESENCE_REG_AUTO_STEP_LENGTH_ENABLED_ADDRESS,  // auto_step_length_enabled address
        presence_reg_read_auto_step_length_enabled,     // auto_step_length_enabled read access function
        presence_reg_write_auto_step_length_enabled,    // auto_step_length_enabled write access function
    },
    {
        PRESENCE_REG_MANUAL_PROFILE_ADDRESS,  // manual_profile address
        presence_reg_read_manual_profile,     // manual_profile read access function
        presence_reg_write_manual_profile,    // manual_profile write access function
    },
    {
        PRESENCE_REG_MANUAL_STEP_LENGTH_ADDRESS,  // manual_step_length address
        presence_reg_read_manual_step_length,     // manual_step_length read access function
        presence_reg_write_manual_step_length,    // manual_step_length write access function
    },
    {
        PRESENCE_REG_START_ADDRESS,  // start address
        presence_reg_read_start,     // start read access function
        presence_reg_write_start,    // start write access function
    },
    {
        PRESENCE_REG_END_ADDRESS,  // end address
        presence_reg_read_end,     // end read access function
        presence_reg_write_end,    // end write access function
    },
    {
        PRESENCE_REG_RESET_FILTERS_ON_PREPARE_ADDRESS,  // reset_filters_on_prepare address
        presence_reg_read_reset_filters_on_prepare,     // reset_filters_on_prepare read access function
        presence_reg_write_reset_filters_on_prepare,    // reset_filters_on_prepare write access function
    },
    {
        PRESENCE_REG_HWAAS_ADDRESS,  // hwaas address
        presence_reg_read_hwaas,     // hwaas read access function
        presence_reg_write_hwaas,    // hwaas write access function
    },
    {
        PRESENCE_REG_DETECTION_ON_GPIO_ADDRESS,  // detection_on_gpio address
        presence_reg_read_detection_on_gpio,     // detection_on_gpio read access function
        presence_reg_write_detection_on_gpio,    // detection_on_gpio write access function
    },
    {
        PRESENCE_REG_COMMAND_ADDRESS,  // command address
        NULL,
        presence_reg_write_command,    // command write access function
    },
};


void presence_reg_protocol_setup(void)
{
    acc_reg_protocol_setup(presence_reg_protocol, PRESENCE_REG_REGISTER_COUNT);
}


void presence_reg_protocol_write_default(void)
{
    presence_reg_write_sweeps_per_frame(PRESENCE_REG_SWEEPS_PER_FRAME_DEFAULT_VALUE);
    presence_reg_write_inter_frame_presence_timeout(PRESENCE_REG_INTER_FRAME_PRESENCE_TIMEOUT_DEFAULT_VALUE);
    presence_reg_write_inter_phase_boost_enabled(PRESENCE_REG_INTER_PHASE_BOOST_ENABLED_DEFAULT_VALUE);
    presence_reg_write_intra_detection_enabled(PRESENCE_REG_INTRA_DETECTION_ENABLED_DEFAULT_VALUE);
    presence_reg_write_inter_detection_enabled(PRESENCE_REG_INTER_DETECTION_ENABLED_DEFAULT_VALUE);
    presence_reg_write_frame_rate(PRESENCE_REG_FRAME_RATE_DEFAULT_VALUE);
    presence_reg_write_intra_detection_threshold(PRESENCE_REG_INTRA_DETECTION_THRESHOLD_DEFAULT_VALUE);
    presence_reg_write_inter_detection_threshold(PRESENCE_REG_INTER_DETECTION_THRESHOLD_DEFAULT_VALUE);
    presence_reg_write_inter_frame_deviation_time_const(PRESENCE_REG_INTER_FRAME_DEVIATION_TIME_CONST_DEFAULT_VALUE);
    presence_reg_write_inter_frame_fast_cutoff(PRESENCE_REG_INTER_FRAME_FAST_CUTOFF_DEFAULT_VALUE);
    presence_reg_write_inter_frame_slow_cutoff(PRESENCE_REG_INTER_FRAME_SLOW_CUTOFF_DEFAULT_VALUE);
    presence_reg_write_intra_frame_time_const(PRESENCE_REG_INTRA_FRAME_TIME_CONST_DEFAULT_VALUE);
    presence_reg_write_intra_output_time_const(PRESENCE_REG_INTRA_OUTPUT_TIME_CONST_DEFAULT_VALUE);
    presence_reg_write_inter_output_time_const(PRESENCE_REG_INTER_OUTPUT_TIME_CONST_DEFAULT_VALUE);
    presence_reg_write_auto_profile_enabled(PRESENCE_REG_AUTO_PROFILE_ENABLED_DEFAULT_VALUE);
    presence_reg_write_auto_step_length_enabled(PRESENCE_REG_AUTO_STEP_LENGTH_ENABLED_DEFAULT_VALUE);
    presence_reg_write_manual_profile(PRESENCE_REG_MANUAL_PROFILE_DEFAULT_VALUE);
    presence_reg_write_manual_step_length(PRESENCE_REG_MANUAL_STEP_LENGTH_DEFAULT_VALUE);
    presence_reg_write_start(PRESENCE_REG_START_DEFAULT_VALUE);
    presence_reg_write_end(PRESENCE_REG_END_DEFAULT_VALUE);
    presence_reg_write_reset_filters_on_prepare(PRESENCE_REG_RESET_FILTERS_ON_PREPARE_DEFAULT_VALUE);
    presence_reg_write_hwaas(PRESENCE_REG_HWAAS_DEFAULT_VALUE);
    presence_reg_write_detection_on_gpio(PRESENCE_REG_DETECTION_ON_GPIO_DEFAULT_VALUE);
}
