// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>

#include "acc_detector_presence.h"
#include "acc_integration.h"
#include "acc_version.h"
#include "ref_app_breathing.h"

#include "acc_reg_protocol.h"
#include "i2c_ref_app_breathing.h"
#include "ref_app_breathing_reg_protocol.h"


/**
 * @brief Get the protocol error status flags
 *
 * @return error flags as uint32 register value
 */
static uint32_t get_protocol_error_flags(void);


//
// REGISTER READ/WRITE ACCESS FUNCTIONS
//


void ref_app_breathing_reg_read_version(uint32_t *value)
{
	*value = acc_version_get_hex();
}


void ref_app_breathing_reg_read_protocol_status(uint32_t *value)
{
	uint32_t error_flags     = get_protocol_error_flags();
	uint32_t protocol_status = 0;

	if ((error_flags & ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR) != 0)
	{
		protocol_status |=  REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR) != 0)
	{
		protocol_status |= REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_ADDRESS_ERROR) != 0)
	{
		protocol_status |= REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_FAILED) != 0)
	{
		protocol_status |= REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY) != 0)
	{
		protocol_status |= REF_APP_BREATHING_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK;
	}

	*value = protocol_status;
}


void ref_app_breathing_reg_read_measure_counter(uint32_t *value)
{
	*value = i2c_ref_app_breathing_get_counter();
}


void ref_app_breathing_reg_read_app_status(uint32_t *value)
{
	*value = i2c_ref_app_breathing_get_status();
}


void ref_app_breathing_reg_read_breathing_result(uint32_t *value)
{
	*value = i2c_ref_app_breathing_get_result();
}


void ref_app_breathing_reg_read_breathing_rate(uint32_t *value)
{
	float float_value = i2c_ref_app_breathing_get_breathing_rate();

	*value = (uint32_t)float_value;
}


void ref_app_breathing_reg_read_app_state(uint32_t *value)
{
	ref_app_breathing_app_state_t state = i2c_ref_app_breathing_get_app_state();

	switch (state)
	{
		case REF_APP_BREATHING_APP_STATE_INIT:
			*value = REF_APP_BREATHING_REG_APP_STATE_ENUM_INIT;
			break;
		case REF_APP_BREATHING_APP_STATE_NO_PRESENCE:
			*value = REF_APP_BREATHING_REG_APP_STATE_ENUM_NO_PRESENCE;
			break;
		case REF_APP_BREATHING_APP_STATE_INTRA_PRESENCE:
			*value = REF_APP_BREATHING_REG_APP_STATE_ENUM_INTRA_PRESENCE;
			break;
		case REF_APP_BREATHING_APP_STATE_DETERMINE_DISTANCE:
			*value = REF_APP_BREATHING_REG_APP_STATE_ENUM_DETERMINE_DISTANCE;
			break;
		case REF_APP_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE:
			*value = REF_APP_BREATHING_REG_APP_STATE_ENUM_ESTIMATE_BREATHING_RATE;
			break;
		default:
			//Not possible
			break;
	}
}


void ref_app_breathing_reg_read_start(uint32_t *value)
{
	const ref_app_breathing_config_t *config     = i2c_ref_app_breathing_get_config();
	float                            float_value = acc_detector_presence_config_start_get(config->presence_config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool ref_app_breathing_reg_write_start(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_start_set(config->presence_config, float_value);
		status = true;
	}

	return status;
}


void ref_app_breathing_reg_read_end(uint32_t *value)
{
	const ref_app_breathing_config_t *config     = i2c_ref_app_breathing_get_config();
	float                            float_value = acc_detector_presence_config_end_get(config->presence_config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool ref_app_breathing_reg_write_end(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_end_set(config->presence_config, float_value);
		status = true;
	}

	return status;
}


void ref_app_breathing_reg_read_num_distances_to_analyze(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = config->num_dists_to_analyze;
}


bool ref_app_breathing_reg_write_num_distances_to_analyze(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		config->num_dists_to_analyze = value;
		status                       = true;
	}

	return status;
}


void ref_app_breathing_reg_read_distance_determination_duration_s(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = config->distance_determination_duration_s;
}


bool ref_app_breathing_reg_write_distance_determination_duration_s(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		config->distance_determination_duration_s = value;
		status                                    = true;
	}

	return status;
}


void ref_app_breathing_reg_read_use_presence_processor(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = config->use_presence_processor ? 1 : 0;
}


bool ref_app_breathing_reg_write_use_presence_processor(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		config->use_presence_processor = (value != 0);
		status                         = true;
	}

	return status;
}


void ref_app_breathing_reg_read_lowest_breathing_rate(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = config->lowest_breathing_rate;
}


bool ref_app_breathing_reg_write_lowest_breathing_rate(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		config->lowest_breathing_rate = value;
		status                        = true;
	}

	return status;
}


void ref_app_breathing_reg_read_highest_breathing_rate(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = config->highest_breathing_rate;
}


bool ref_app_breathing_reg_write_highest_breathing_rate(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		config->highest_breathing_rate = value;
		status                         = true;
	}

	return status;
}


void ref_app_breathing_reg_read_time_series_length_s(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = config->time_series_length_s;
}


bool ref_app_breathing_reg_write_time_series_length_s(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		config->time_series_length_s = value;
		status                       = true;
	}

	return status;
}


void ref_app_breathing_reg_read_frame_rate(uint32_t *value)
{
	const ref_app_breathing_config_t *config     = i2c_ref_app_breathing_get_config();
	float                            float_value = acc_detector_presence_config_frame_rate_get(config->presence_config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool ref_app_breathing_reg_write_frame_rate(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_frame_rate_set(config->presence_config, float_value);
		status = true;
	}

	return status;
}


void ref_app_breathing_reg_read_sweeps_per_frame(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = acc_detector_presence_config_sweeps_per_frame_get(config->presence_config);
}


bool ref_app_breathing_reg_write_sweeps_per_frame(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		acc_detector_presence_config_sweeps_per_frame_set(config->presence_config, value);
		status = true;
	}

	return status;
}


void ref_app_breathing_reg_read_hwaas(uint32_t *value)
{
	const ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	*value = acc_detector_presence_config_hwaas_get(config->presence_config);
}


bool ref_app_breathing_reg_write_hwaas(uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		acc_detector_presence_config_hwaas_set(config->presence_config, value);
		status = true;
	}

	return status;
}


void ref_app_breathing_reg_read_profile(uint32_t *value)
{
	const ref_app_breathing_config_t *config       = i2c_ref_app_breathing_get_config();
	acc_config_profile_t             profile       = acc_detector_presence_config_profile_get(config->presence_config);
	uint32_t                         profile_value = 0;

	switch (profile)
	{
		case ACC_CONFIG_PROFILE_1:
			profile_value = REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE1;
			break;
		case ACC_CONFIG_PROFILE_2:
			profile_value = REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE2;
			break;
		case ACC_CONFIG_PROFILE_3:
			profile_value = REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE3;
			break;
		case ACC_CONFIG_PROFILE_4:
			profile_value = REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE4;
			break;
		case ACC_CONFIG_PROFILE_5:
			profile_value = REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE5;
			break;
		default:
			break;
	}

	*value = profile_value;
}


bool ref_app_breathing_reg_write_profile(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		status = true;
		acc_config_profile_t profile = ACC_CONFIG_PROFILE_1;

		switch (value)
		{
			case REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE1:
				profile = ACC_CONFIG_PROFILE_1;
				break;
			case REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE2:
				profile = ACC_CONFIG_PROFILE_2;
				break;
			case REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE3:
				profile = ACC_CONFIG_PROFILE_3;
				break;
			case REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE4:
				profile = ACC_CONFIG_PROFILE_4;
				break;
			case REF_APP_BREATHING_REG_PROFILE_ENUM_PROFILE5:
				profile = ACC_CONFIG_PROFILE_5;
				break;
			default:
				status = false;
				break;
		}

		if (status)
		{
			acc_detector_presence_config_profile_set(config->presence_config, profile);
		}
	}

	return status;
}


void ref_app_breathing_reg_read_intra_detection_threshold(uint32_t *value)
{
	const ref_app_breathing_config_t *config     = i2c_ref_app_breathing_get_config();
	float                            float_value = acc_detector_presence_config_intra_detection_threshold_get(config->presence_config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool ref_app_breathing_reg_write_intra_detection_threshold(const uint32_t value)
{
	bool                       status  = false;
	ref_app_breathing_config_t *config = i2c_ref_app_breathing_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_intra_detection_threshold_set(config->presence_config, float_value);
		status = true;
	}

	return status;
}


bool ref_app_breathing_reg_write_command(const uint32_t value)
{
	return i2c_ref_app_breathing_command(value);
}


//
// PRIVATE HELPER FUNCTIONS
//


static uint32_t get_protocol_error_flags(void)
{
	/* Make sure we do not have a race for  error flags */
	acc_integration_critical_section_enter();

	uint32_t error_flags = acc_reg_protocol_get_error_flags();

	acc_integration_critical_section_exit();

	return error_flags;
}
