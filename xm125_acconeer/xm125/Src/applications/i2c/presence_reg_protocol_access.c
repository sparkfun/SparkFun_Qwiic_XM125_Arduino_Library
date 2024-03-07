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

#include "acc_reg_protocol.h"
#include "i2c_presence_detector.h"
#include "presence_reg_protocol.h"


/**
 * @brief Get the protocol error status flags
 *
 * @return error flags as uint32 register value
 */
static uint32_t get_protocol_error_flags(void);


//
// REGISTER READ/WRITE ACCESS FUNCTIONS
//


void presence_reg_read_version(uint32_t *value)
{
	*value = acc_version_get_hex();
}


void presence_reg_read_protocol_status(uint32_t *value)
{
	uint32_t error_flags     = get_protocol_error_flags();
	uint32_t protocol_status = 0;

	if ((error_flags & ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR) != 0)
	{
		protocol_status |=  PRESENCE_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR) != 0)
	{
		protocol_status |= PRESENCE_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_ADDRESS_ERROR) != 0)
	{
		protocol_status |= PRESENCE_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_FAILED) != 0)
	{
		protocol_status |= PRESENCE_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY) != 0)
	{
		protocol_status |= PRESENCE_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK;
	}

	*value = protocol_status;
}


void presence_reg_read_measure_counter(uint32_t *value)
{
	*value = i2c_presence_detector_get_counter();
}


void presence_reg_read_detector_status(uint32_t *value)
{
	*value = i2c_presence_detector_get_status();
}


void presence_reg_read_presence_result(uint32_t *value)
{
	*value = i2c_presence_detector_get_result();
}


void presence_reg_read_presence_distance(uint32_t *value)
{
	float float_value = i2c_presence_detector_get_distance();

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


void presence_reg_read_intra_presence_score(uint32_t *value)
{
	float float_value = i2c_presence_detector_get_intra_presence_score();

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


void presence_reg_read_inter_presence_score(uint32_t *value)
{
	float float_value = i2c_presence_detector_get_inter_presence_score();

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


void presence_reg_read_sweeps_per_frame(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	*value = acc_detector_presence_config_sweeps_per_frame_get(config);
}


bool presence_reg_write_sweeps_per_frame(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		acc_detector_presence_config_sweeps_per_frame_set(config, value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_frame_presence_timeout(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	*value = acc_detector_presence_config_inter_frame_presence_timeout_get(config);
}


bool presence_reg_write_inter_frame_presence_timeout(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		acc_detector_presence_config_inter_frame_presence_timeout_set(config, value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_phase_boost_enabled(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	bool bool_value = acc_detector_presence_config_inter_phase_boost_get(config);

	*value = bool_value ? 1 : 0;
}


bool presence_reg_write_inter_phase_boost_enabled(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		bool enabled = (value != 0);
		acc_detector_presence_config_inter_phase_boost_set(config, enabled);
		status = true;
	}

	return status;
}


void presence_reg_read_intra_detection_enabled(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	bool bool_value = acc_detector_presence_config_intra_detection_get(config);

	*value = bool_value ? 1 : 0;
}


bool presence_reg_write_intra_detection_enabled(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		bool enabled = (value != 0);
		acc_detector_presence_config_intra_detection_set(config, enabled);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_detection_enabled(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	bool bool_value = acc_detector_presence_config_inter_detection_get(config);

	*value = bool_value ? 1 : 0;
}


bool presence_reg_write_inter_detection_enabled(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		bool enabled = (value != 0);
		acc_detector_presence_config_inter_detection_set(config, enabled);
		status = true;
	}

	return status;
}


void presence_reg_read_frame_rate(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_frame_rate_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_frame_rate(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_frame_rate_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_intra_detection_threshold(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_intra_detection_threshold_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_intra_detection_threshold(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_intra_detection_threshold_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_detection_threshold(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_inter_detection_threshold_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_inter_detection_threshold(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_inter_detection_threshold_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_frame_deviation_time_const(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_inter_frame_deviation_time_const_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_inter_frame_deviation_time_const(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_inter_frame_deviation_time_const_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_frame_fast_cutoff(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_inter_frame_fast_cutoff_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_inter_frame_fast_cutoff(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_inter_frame_fast_cutoff_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_frame_slow_cutoff(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_inter_frame_slow_cutoff_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_inter_frame_slow_cutoff(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_inter_frame_slow_cutoff_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_intra_frame_time_const(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_intra_frame_time_const_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_intra_frame_time_const(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_intra_frame_time_const_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_intra_output_time_const(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_intra_output_time_const_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_intra_output_time_const(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_intra_output_time_const_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_inter_output_time_const(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_inter_output_time_const_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_inter_output_time_const(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_inter_output_time_const_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_auto_profile_enabled(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	bool bool_value = acc_detector_presence_config_auto_profile_get(config);

	*value = bool_value ? 1 : 0;
}


bool presence_reg_write_auto_profile_enabled(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		bool enabled = (value != 0);
		acc_detector_presence_config_auto_profile_set(config, enabled);
		status = true;
	}

	return status;
}


void presence_reg_read_auto_step_length_enabled(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	bool bool_value = acc_detector_presence_config_auto_step_length_get(config);

	*value = bool_value ? 1 : 0;
}


bool presence_reg_write_auto_step_length_enabled(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		bool enabled = (value != 0);
		acc_detector_presence_config_auto_step_length_set(config, enabled);
		status = true;
	}

	return status;
}


void presence_reg_read_manual_profile(uint32_t *value)
{
	const acc_detector_presence_config_t *config       = i2c_presence_detector_get_config();
	acc_config_profile_t                 profile       = acc_detector_presence_config_profile_get(config);
	uint32_t                             profile_value = 0;

	switch (profile)
	{
		case ACC_CONFIG_PROFILE_1:
			profile_value = PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE1;
			break;
		case ACC_CONFIG_PROFILE_2:
			profile_value = PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE2;
			break;
		case ACC_CONFIG_PROFILE_3:
			profile_value = PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE3;
			break;
		case ACC_CONFIG_PROFILE_4:
			profile_value = PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE4;
			break;
		case ACC_CONFIG_PROFILE_5:
			profile_value = PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE5;
			break;
		default:
			break;
	}

	*value = profile_value;
}


bool presence_reg_write_manual_profile(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		status = true;
		acc_config_profile_t profile = ACC_CONFIG_PROFILE_1;

		switch (value)
		{
			case PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE1:
				profile = ACC_CONFIG_PROFILE_1;
				break;
			case PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE2:
				profile = ACC_CONFIG_PROFILE_2;
				break;
			case PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE3:
				profile = ACC_CONFIG_PROFILE_3;
				break;
			case PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE4:
				profile = ACC_CONFIG_PROFILE_4;
				break;
			case PRESENCE_REG_MANUAL_PROFILE_ENUM_PROFILE5:
				profile = ACC_CONFIG_PROFILE_5;
				break;
			default:
				status = false;
				break;
		}

		if (status)
		{
			acc_detector_presence_config_profile_set(config, profile);
		}
	}

	return status;
}


void presence_reg_read_manual_step_length(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	*value = acc_detector_presence_config_step_length_get(config);
}


bool presence_reg_write_manual_step_length(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		acc_detector_presence_config_step_length_set(config, value);
		status = true;
	}

	return status;
}


void presence_reg_read_start(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_start_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_start(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_start_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_end(uint32_t *value)
{
	const acc_detector_presence_config_t *config     = i2c_presence_detector_get_config();
	float                                float_value = acc_detector_presence_config_end_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(float_value);
}


bool presence_reg_write_end(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		float float_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_presence_config_end_set(config, float_value);
		status = true;
	}

	return status;
}


void presence_reg_read_reset_filters_on_prepare(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	bool bool_value = acc_detector_presence_config_reset_filters_on_prepare_get(config);

	*value = bool_value ? 1 : 0;
}


bool presence_reg_write_reset_filters_on_prepare(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		bool enabled = (value != 0);
		acc_detector_presence_config_reset_filters_on_prepare_set(config, enabled);
		status = true;
	}

	return status;
}


void presence_reg_read_hwaas(uint32_t *value)
{
	const acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	*value = acc_detector_presence_config_hwaas_get(config);
}


bool presence_reg_write_hwaas(uint32_t value)
{
	bool                           status  = false;
	acc_detector_presence_config_t *config = i2c_presence_detector_get_config();

	if (config != NULL)
	{
		acc_detector_presence_config_hwaas_set(config, value);
		status = true;
	}

	return status;
}


void presence_reg_read_detection_on_gpio(uint32_t *value)
{
	if (i2c_presence_detector_get_presence_detected_gpio())
	{
		*value = 1;
	}
	else
	{
		*value = 0;
	}
}


bool presence_reg_write_detection_on_gpio(const uint32_t value)
{
	bool enable = (value != 0);

	i2c_presence_detector_presence_detected_gpio(enable);

	return true;
}


void presence_reg_read_presence_actual_frame_rate(uint32_t *value)
{
	*value = i2c_presence_detector_get_presence_frame_rate_mhz();
}


bool presence_reg_write_command(const uint32_t value)
{
	return i2c_presence_detector_command(value);
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
