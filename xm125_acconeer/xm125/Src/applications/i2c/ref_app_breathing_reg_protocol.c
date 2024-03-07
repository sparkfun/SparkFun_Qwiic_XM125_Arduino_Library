// Copyright (c) Acconeer AB, 2024
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_reg_protocol.h"
#include "ref_app_breathing_reg_protocol.h"


/**
 * This struct defines the links between a register address and the register read and/or write function.
 */
const acc_reg_protocol_t ref_app_breathing_reg_protocol[REF_APP_BREATHING_REG_REGISTER_COUNT] = {
    {
        REF_APP_BREATHING_REG_VERSION_ADDRESS,  // version address
        ref_app_breathing_reg_read_version,     // version read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_PROTOCOL_STATUS_ADDRESS,  // protocol_status address
        ref_app_breathing_reg_read_protocol_status,     // protocol_status read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_MEASURE_COUNTER_ADDRESS,  // measure_counter address
        ref_app_breathing_reg_read_measure_counter,     // measure_counter read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_APP_STATUS_ADDRESS,  // app_status address
        ref_app_breathing_reg_read_app_status,     // app_status read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_BREATHING_RESULT_ADDRESS,  // breathing_result address
        ref_app_breathing_reg_read_breathing_result,     // breathing_result read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_BREATHING_RATE_ADDRESS,  // breathing_rate address
        ref_app_breathing_reg_read_breathing_rate,     // breathing_rate read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_APP_STATE_ADDRESS,  // app_state address
        ref_app_breathing_reg_read_app_state,     // app_state read access function
        NULL,
    },
    {
        REF_APP_BREATHING_REG_START_ADDRESS,  // start address
        ref_app_breathing_reg_read_start,     // start read access function
        ref_app_breathing_reg_write_start,    // start write access function
    },
    {
        REF_APP_BREATHING_REG_END_ADDRESS,  // end address
        ref_app_breathing_reg_read_end,     // end read access function
        ref_app_breathing_reg_write_end,    // end write access function
    },
    {
        REF_APP_BREATHING_REG_NUM_DISTANCES_TO_ANALYZE_ADDRESS,  // num_distances_to_analyze address
        ref_app_breathing_reg_read_num_distances_to_analyze,     // num_distances_to_analyze read access function
        ref_app_breathing_reg_write_num_distances_to_analyze,    // num_distances_to_analyze write access function
    },
    {
        REF_APP_BREATHING_REG_DISTANCE_DETERMINATION_DURATION_S_ADDRESS,  // distance_determination_duration_s address
        ref_app_breathing_reg_read_distance_determination_duration_s,     // distance_determination_duration_s read access function
        ref_app_breathing_reg_write_distance_determination_duration_s,    // distance_determination_duration_s write access function
    },
    {
        REF_APP_BREATHING_REG_USE_PRESENCE_PROCESSOR_ADDRESS,  // use_presence_processor address
        ref_app_breathing_reg_read_use_presence_processor,     // use_presence_processor read access function
        ref_app_breathing_reg_write_use_presence_processor,    // use_presence_processor write access function
    },
    {
        REF_APP_BREATHING_REG_LOWEST_BREATHING_RATE_ADDRESS,  // lowest_breathing_rate address
        ref_app_breathing_reg_read_lowest_breathing_rate,     // lowest_breathing_rate read access function
        ref_app_breathing_reg_write_lowest_breathing_rate,    // lowest_breathing_rate write access function
    },
    {
        REF_APP_BREATHING_REG_HIGHEST_BREATHING_RATE_ADDRESS,  // highest_breathing_rate address
        ref_app_breathing_reg_read_highest_breathing_rate,     // highest_breathing_rate read access function
        ref_app_breathing_reg_write_highest_breathing_rate,    // highest_breathing_rate write access function
    },
    {
        REF_APP_BREATHING_REG_TIME_SERIES_LENGTH_S_ADDRESS,  // time_series_length_s address
        ref_app_breathing_reg_read_time_series_length_s,     // time_series_length_s read access function
        ref_app_breathing_reg_write_time_series_length_s,    // time_series_length_s write access function
    },
    {
        REF_APP_BREATHING_REG_FRAME_RATE_ADDRESS,  // frame_rate address
        ref_app_breathing_reg_read_frame_rate,     // frame_rate read access function
        ref_app_breathing_reg_write_frame_rate,    // frame_rate write access function
    },
    {
        REF_APP_BREATHING_REG_SWEEPS_PER_FRAME_ADDRESS,  // sweeps_per_frame address
        ref_app_breathing_reg_read_sweeps_per_frame,     // sweeps_per_frame read access function
        ref_app_breathing_reg_write_sweeps_per_frame,    // sweeps_per_frame write access function
    },
    {
        REF_APP_BREATHING_REG_HWAAS_ADDRESS,  // hwaas address
        ref_app_breathing_reg_read_hwaas,     // hwaas read access function
        ref_app_breathing_reg_write_hwaas,    // hwaas write access function
    },
    {
        REF_APP_BREATHING_REG_PROFILE_ADDRESS,  // profile address
        ref_app_breathing_reg_read_profile,     // profile read access function
        ref_app_breathing_reg_write_profile,    // profile write access function
    },
    {
        REF_APP_BREATHING_REG_INTRA_DETECTION_THRESHOLD_ADDRESS,  // intra_detection_threshold address
        ref_app_breathing_reg_read_intra_detection_threshold,     // intra_detection_threshold read access function
        ref_app_breathing_reg_write_intra_detection_threshold,    // intra_detection_threshold write access function
    },
    {
        REF_APP_BREATHING_REG_COMMAND_ADDRESS,  // command address
        NULL,
        ref_app_breathing_reg_write_command,    // command write access function
    },
};


void ref_app_breathing_reg_protocol_setup(void)
{
    acc_reg_protocol_setup(ref_app_breathing_reg_protocol, REF_APP_BREATHING_REG_REGISTER_COUNT);
}


void ref_app_breathing_reg_protocol_write_default(void)
{
    ref_app_breathing_reg_write_start(REF_APP_BREATHING_REG_START_DEFAULT_VALUE);
    ref_app_breathing_reg_write_end(REF_APP_BREATHING_REG_END_DEFAULT_VALUE);
    ref_app_breathing_reg_write_num_distances_to_analyze(REF_APP_BREATHING_REG_NUM_DISTANCES_TO_ANALYZE_DEFAULT_VALUE);
    ref_app_breathing_reg_write_distance_determination_duration_s(REF_APP_BREATHING_REG_DISTANCE_DETERMINATION_DURATION_S_DEFAULT_VALUE);
    ref_app_breathing_reg_write_use_presence_processor(REF_APP_BREATHING_REG_USE_PRESENCE_PROCESSOR_DEFAULT_VALUE);
    ref_app_breathing_reg_write_lowest_breathing_rate(REF_APP_BREATHING_REG_LOWEST_BREATHING_RATE_DEFAULT_VALUE);
    ref_app_breathing_reg_write_highest_breathing_rate(REF_APP_BREATHING_REG_HIGHEST_BREATHING_RATE_DEFAULT_VALUE);
    ref_app_breathing_reg_write_time_series_length_s(REF_APP_BREATHING_REG_TIME_SERIES_LENGTH_S_DEFAULT_VALUE);
    ref_app_breathing_reg_write_frame_rate(REF_APP_BREATHING_REG_FRAME_RATE_DEFAULT_VALUE);
    ref_app_breathing_reg_write_sweeps_per_frame(REF_APP_BREATHING_REG_SWEEPS_PER_FRAME_DEFAULT_VALUE);
    ref_app_breathing_reg_write_hwaas(REF_APP_BREATHING_REG_HWAAS_DEFAULT_VALUE);
    ref_app_breathing_reg_write_profile(REF_APP_BREATHING_REG_PROFILE_DEFAULT_VALUE);
    ref_app_breathing_reg_write_intra_detection_threshold(REF_APP_BREATHING_REG_INTRA_DETECTION_THRESHOLD_DEFAULT_VALUE);
}
