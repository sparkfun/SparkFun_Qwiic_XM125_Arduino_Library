// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>

#include "acc_detector_distance.h"
#include "acc_detector_distance_definitions.h"
#include "acc_integration.h"
#include "acc_version.h"

#include "acc_reg_protocol.h"
#include "distance_reg_protocol.h"
#include "i2c_distance_detector.h"


/**
 * @brief Get the protocol error status flags
 *
 * @return error flags as uint32 register value
 */
static uint32_t get_protocol_error_flags(void);


/**
 * @brief Get peak distance
 *
 * This function will return the peak distance in mm
 *
 * @param[in] peak_id The index of the peak
 * @return The uint32 value representation of the peak distance
 */
static uint32_t get_peak_distance(uint16_t peak_id);


/**
 * @brief Get peak strength
 *
 * This function will return the peak distance in dB * 1000
 *
 * @param[in] peak_id The index of the peak
 * @return The int32 value representation of the peak strength
 */
static uint32_t get_peak_strength(uint16_t peak_id);


//
// REGISTER READ/WRITE ACCESS FUNCTIONS
//


void distance_reg_read_version(uint32_t *value)
{
	*value = acc_version_get_hex();
}


void distance_reg_read_protocol_status(uint32_t *value)
{
	uint32_t error_flags     = get_protocol_error_flags();
	uint32_t protocol_status = 0;

	if ((error_flags & ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR) != 0)
	{
		protocol_status |=  DISTANCE_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR) != 0)
	{
		protocol_status |= DISTANCE_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_ADDRESS_ERROR) != 0)
	{
		protocol_status |= DISTANCE_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_FAILED) != 0)
	{
		protocol_status |= DISTANCE_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY) != 0)
	{
		protocol_status |= DISTANCE_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK;
	}

	*value = protocol_status;
}


void distance_reg_read_measure_counter(uint32_t *value)
{
	*value = i2c_distance_detector_get_counter();
}


void distance_reg_read_detector_status(uint32_t *value)
{
	*value = i2c_distance_detector_get_status();
}


void distance_reg_read_distance_result(uint32_t *value)
{
	*value = i2c_distance_detector_get_result();
}


void distance_reg_read_peak0_distance(uint32_t *value)
{
	*value = get_peak_distance(0);
}


void distance_reg_read_peak1_distance(uint32_t *value)
{
	*value = get_peak_distance(1);
}


void distance_reg_read_peak2_distance(uint32_t *value)
{
	*value = get_peak_distance(2);
}


void distance_reg_read_peak3_distance(uint32_t *value)
{
	*value = get_peak_distance(3);
}


void distance_reg_read_peak4_distance(uint32_t *value)
{
	*value = get_peak_distance(4);
}


void distance_reg_read_peak5_distance(uint32_t *value)
{
	*value = get_peak_distance(5);
}


void distance_reg_read_peak6_distance(uint32_t *value)
{
	*value = get_peak_distance(6);
}


void distance_reg_read_peak7_distance(uint32_t *value)
{
	*value = get_peak_distance(7);
}


void distance_reg_read_peak8_distance(uint32_t *value)
{
	*value = get_peak_distance(8);
}


void distance_reg_read_peak9_distance(uint32_t *value)
{
	*value = get_peak_distance(9);
}


void distance_reg_read_peak0_strength(uint32_t *value)
{
	*value = get_peak_strength(0);
}


void distance_reg_read_peak1_strength(uint32_t *value)
{
	*value = get_peak_strength(1);
}


void distance_reg_read_peak2_strength(uint32_t *value)
{
	*value = get_peak_strength(2);
}


void distance_reg_read_peak3_strength(uint32_t *value)
{
	*value = get_peak_strength(3);
}


void distance_reg_read_peak4_strength(uint32_t *value)
{
	*value = get_peak_strength(4);
}


void distance_reg_read_peak5_strength(uint32_t *value)
{
	*value = get_peak_strength(5);
}


void distance_reg_read_peak6_strength(uint32_t *value)
{
	*value = get_peak_strength(6);
}


void distance_reg_read_peak7_strength(uint32_t *value)
{
	*value = get_peak_strength(7);
}


void distance_reg_read_peak8_strength(uint32_t *value)
{
	*value = get_peak_strength(8);
}


void distance_reg_read_peak9_strength(uint32_t *value)
{
	*value = get_peak_strength(9);
}


void distance_reg_read_start(uint32_t *value)
{
	const acc_detector_distance_config_t *config = i2c_distance_detector_get_config();
	float                                start   = acc_detector_distance_config_start_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(start);
}


bool distance_reg_write_start(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		float start = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_distance_config_start_set(config, start);
		status = true;
	}

	return status;
}


void distance_reg_read_end(uint32_t *value)
{
	const acc_detector_distance_config_t *config = i2c_distance_detector_get_config();
	float                                end     = acc_detector_distance_config_end_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(end);
}


bool distance_reg_write_end(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		float end = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_distance_config_end_set(config, end);
		status = true;
	}

	return status;
}


void distance_reg_read_max_step_length(uint32_t *value)
{
	const acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	*value = acc_detector_distance_config_max_step_length_get(config);
}


bool distance_reg_write_max_step_length(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		acc_detector_distance_config_max_step_length_set(config, value);
		status = true;
	}

	return status;
}


void distance_reg_read_close_range_leakage_cancellation(uint32_t *value)
{
	const acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	*value = acc_detector_distance_config_close_range_leakage_cancellation_get(config);
}


bool distance_reg_write_close_range_leakage_cancellation(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		acc_detector_distance_config_close_range_leakage_cancellation_set(config, (value != 0));
		status = true;
	}

	return status;
}


void distance_reg_read_signal_quality(uint32_t *value)
{
	const acc_detector_distance_config_t *config        = i2c_distance_detector_get_config();
	float                                signal_quality = acc_detector_distance_config_signal_quality_get(config);

	*value = acc_reg_protocol_float_to_int32_milli(signal_quality);
}


bool distance_reg_write_signal_quality(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		float signal_quality = acc_reg_protocol_int32_milli_to_float(value);
		acc_detector_distance_config_signal_quality_set(config, signal_quality);
		status = true;
	}

	return status;
}


void distance_reg_read_max_profile(uint32_t *value)
{
	const acc_detector_distance_config_t *config       = i2c_distance_detector_get_config();
	acc_config_profile_t                 profile       = acc_detector_distance_config_max_profile_get(config);
	uint32_t                             profile_value = 0;

	switch (profile)
	{
		case ACC_CONFIG_PROFILE_1:
			profile_value = DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE1;
			break;
		case ACC_CONFIG_PROFILE_2:
			profile_value = DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE2;
			break;
		case ACC_CONFIG_PROFILE_3:
			profile_value = DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE3;
			break;
		case ACC_CONFIG_PROFILE_4:
			profile_value = DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE4;
			break;
		case ACC_CONFIG_PROFILE_5:
			profile_value = DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE5;
			break;
		default:
			break;
	}

	*value = profile_value;
}


bool distance_reg_write_max_profile(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		status = true;
		acc_config_profile_t profile = ACC_CONFIG_PROFILE_1;

		switch (value)
		{
			case DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE1:
				profile = ACC_CONFIG_PROFILE_1;
				break;
			case DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE2:
				profile = ACC_CONFIG_PROFILE_2;
				break;
			case DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE3:
				profile = ACC_CONFIG_PROFILE_3;
				break;
			case DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE4:
				profile = ACC_CONFIG_PROFILE_4;
				break;
			case DISTANCE_REG_MAX_PROFILE_ENUM_PROFILE5:
				profile = ACC_CONFIG_PROFILE_5;
				break;
			default:
				status = false;
				break;
		}

		if (status)
		{
			acc_detector_distance_config_max_profile_set(config, profile);
		}
	}

	return status;
}


void distance_reg_read_threshold_method(uint32_t *value)
{
	const acc_detector_distance_config_t     *config                = i2c_distance_detector_get_config();
	acc_detector_distance_threshold_method_t threshold_method       = acc_detector_distance_config_threshold_method_get(config);
	uint32_t                                 threshold_method_value = 0;

	switch (threshold_method)
	{
		case ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_FIXED_AMPLITUDE:
			threshold_method_value = DISTANCE_REG_THRESHOLD_METHOD_ENUM_FIXED_AMPLITUDE;
			break;
		case ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_FIXED_STRENGTH:
			threshold_method_value = DISTANCE_REG_THRESHOLD_METHOD_ENUM_FIXED_STRENGTH;
			break;
		case ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_RECORDED:
			threshold_method_value = DISTANCE_REG_THRESHOLD_METHOD_ENUM_RECORDED;
			break;
		case ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_CFAR:
			threshold_method_value = DISTANCE_REG_THRESHOLD_METHOD_ENUM_CFAR;
			break;
		default:
			break;
	}

	*value = threshold_method_value;
}


bool distance_reg_write_threshold_method(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		status = true;
		acc_detector_distance_threshold_method_t threshold_method = ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_FIXED_AMPLITUDE;

		switch (value)
		{
			case DISTANCE_REG_THRESHOLD_METHOD_ENUM_FIXED_AMPLITUDE:
				threshold_method = ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_FIXED_AMPLITUDE;
				break;
			case DISTANCE_REG_THRESHOLD_METHOD_ENUM_FIXED_STRENGTH:
				threshold_method = ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_FIXED_STRENGTH;
				break;
			case DISTANCE_REG_THRESHOLD_METHOD_ENUM_RECORDED:
				threshold_method = ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_RECORDED;
				break;
			case DISTANCE_REG_THRESHOLD_METHOD_ENUM_CFAR:
				threshold_method = ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_CFAR;
				break;
			default:
				status = false;
				break;
		}

		if (status)
		{
			acc_detector_distance_config_threshold_method_set(config, threshold_method);
		}
	}

	return status;
}


void distance_reg_read_peak_sorting(uint32_t *value)
{
	const acc_detector_distance_config_t *config            = i2c_distance_detector_get_config();
	acc_detector_distance_peak_sorting_t peak_sorting       = acc_detector_distance_config_peak_sorting_get(config);
	uint32_t                             peak_sorting_value = 0;

	switch (peak_sorting)
	{
		case ACC_DETECTOR_DISTANCE_PEAK_SORTING_CLOSEST:
			peak_sorting_value = DISTANCE_REG_PEAK_SORTING_ENUM_CLOSEST;
			break;
		case ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST:
			peak_sorting_value = DISTANCE_REG_PEAK_SORTING_ENUM_STRONGEST;
			break;
		default:
			break;
	}

	*value = peak_sorting_value;
}


bool distance_reg_write_peak_sorting(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		status = true;
		acc_detector_distance_peak_sorting_t peak_sorting = ACC_DETECTOR_DISTANCE_PEAK_SORTING_CLOSEST;

		switch (value)
		{
			case DISTANCE_REG_PEAK_SORTING_ENUM_CLOSEST:
				peak_sorting = ACC_DETECTOR_DISTANCE_PEAK_SORTING_CLOSEST;
				break;
			case DISTANCE_REG_PEAK_SORTING_ENUM_STRONGEST:
				peak_sorting = ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST;
				break;
			default:
				status = false;
				break;
		}

		if (status)
		{
			acc_detector_distance_config_peak_sorting_set(config, peak_sorting);
		}
	}

	return status;
}


void distance_reg_read_num_frames_recorded_threshold(uint32_t *value)
{
	const acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	*value = acc_detector_distance_config_num_frames_recorded_threshold_get(config);;
}


bool distance_reg_write_num_frames_recorded_threshold(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		acc_detector_distance_config_num_frames_recorded_threshold_set(config, value);
		status = true;
	}

	return status;
}


void distance_reg_read_fixed_amplitude_threshold_value(uint32_t *value)
{
	const acc_detector_distance_config_t *config               = i2c_distance_detector_get_config();
	float                                fixed_threshold_value = acc_detector_distance_config_fixed_amplitude_threshold_value_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(fixed_threshold_value);
}


bool distance_reg_write_fixed_amplitude_threshold_value(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		float fixed_threshold_value = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_distance_config_fixed_amplitude_threshold_value_set(config, fixed_threshold_value);
		status = true;
	}

	return status;
}


void distance_reg_read_fixed_strength_threshold_value(uint32_t *value)
{
	const acc_detector_distance_config_t *config               = i2c_distance_detector_get_config();
	float                                fixed_threshold_value = acc_detector_distance_config_fixed_strength_threshold_value_get(config);

	*value = acc_reg_protocol_float_to_int32_milli(fixed_threshold_value);
}


bool distance_reg_write_fixed_strength_threshold_value(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		float fixed_threshold_value = acc_reg_protocol_int32_milli_to_float(value);
		acc_detector_distance_config_fixed_strength_threshold_value_set(config, fixed_threshold_value);
		status = true;
	}

	return status;
}


void distance_reg_read_threshold_sensitivity(uint32_t *value)
{
	const acc_detector_distance_config_t *config               = i2c_distance_detector_get_config();
	float                                threshold_sensitivity = acc_detector_distance_config_threshold_sensitivity_get(config);

	*value = acc_reg_protocol_float_to_uint32_milli(threshold_sensitivity);
}


bool distance_reg_write_threshold_sensitivity(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		float threshold_sensitivity = acc_reg_protocol_uint32_milli_to_float(value);
		acc_detector_distance_config_threshold_sensitivity_set(config, threshold_sensitivity);
		status = true;
	}

	return status;
}


void distance_reg_read_reflector_shape(uint32_t *value)
{
	const acc_detector_distance_config_t    *config               = i2c_distance_detector_get_config();
	acc_detector_distance_reflector_shape_t reflector_shape       = acc_detector_distance_config_reflector_shape_get(config);
	uint32_t                                reflector_shape_value = 0;

	switch (reflector_shape)
	{
		case ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_GENERIC:
			reflector_shape_value = DISTANCE_REG_REFLECTOR_SHAPE_ENUM_GENERIC;
			break;
		case ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_PLANAR:
			reflector_shape_value = DISTANCE_REG_REFLECTOR_SHAPE_ENUM_PLANAR;
			break;
		default:
			break;
	}

	*value = reflector_shape_value;
}


bool distance_reg_write_reflector_shape(const uint32_t value)
{
	bool                           status  = false;
	acc_detector_distance_config_t *config = i2c_distance_detector_get_config();

	if (config != NULL)
	{
		status = true;
		acc_detector_distance_reflector_shape_t reflector_shape = ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_GENERIC;

		switch (value)
		{
			case DISTANCE_REG_REFLECTOR_SHAPE_ENUM_GENERIC:
				reflector_shape = ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_GENERIC;
				break;
			case DISTANCE_REG_REFLECTOR_SHAPE_ENUM_PLANAR:
				reflector_shape = ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_PLANAR;
				break;
			default:
				status = false;
				break;
		}

		if (status)
		{
			acc_detector_distance_config_reflector_shape_set(config, reflector_shape);
		}
	}

	return status;
}


void distance_reg_read_measure_on_wakeup(uint32_t *value)
{
	if (i2c_distance_detector_get_measure_on_wakeup())
	{
		*value = 1;
	}
	else
	{
		*value = 0;
	}
}


bool distance_reg_write_measure_on_wakeup(const uint32_t value)
{
	bool enable = (value != 0);

	i2c_distance_detector_measure_on_wakeup(enable);

	return true;
}


bool distance_reg_write_command(const uint32_t value)
{
	return i2c_distance_detector_command(value);
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


static uint32_t get_peak_distance(uint16_t peak_id)
{
	float distance = i2c_distance_detector_get_peak_distance(peak_id);

	return acc_reg_protocol_float_to_uint32_milli(distance);
}


static uint32_t get_peak_strength(uint16_t peak_id)
{
	float strength = i2c_distance_detector_get_peak_strength(peak_id);

	return acc_reg_protocol_float_to_int32_milli(strength);
}
