// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_reg_protocol.h"

/**
 * @brief Register access mode
 */
typedef enum
{
	REG_MODE_READ_WRITE,
	REG_MODE_READ,
	REG_MODE_WRITE,
	REG_MODE_ERROR,
} acc_reg_mode_t;


/**
 * @brief Register protocol state
 */
typedef enum
{
	REG_STATE_WAIT_FOR_ADDRESS,
	REG_STATE_WAIT_FOR_READ_OR_WRITE,
	REG_STATE_WAIT_FOR_WRITE,
	REG_STATE_WAIT_FOR_READ,
	REG_STATE_NACK_NEXT_WRITE,
} acc_reg_protocol_state_t;

#define REG_INVALID_ADDRESS      0xffffU
#define REG_ERROR_READ_ADDRESS   0xeeeeee00U
#define REG_ERROR_READ_OF_WO_REG 0xeeeeee01U

static const acc_reg_protocol_t *reg_protocol_struct = NULL;
static uint16_t                 reg_register_count   = 0;
static acc_reg_protocol_state_t reg_state            = REG_STATE_WAIT_FOR_ADDRESS;
static uint16_t                 reg_address          = REG_INVALID_ADDRESS;
static uint32_t                 reg_error_flags      = 0;


/**
 * @brief Get register struct from protocol struct
 *
 * @param[in] register_address The address of the register
 * @return A pointer to the register struct, NULL if the address is invalid
 */
static const acc_reg_protocol_t *get_register_struct(uint16_t register_address)
{
	const acc_reg_protocol_t *reg = NULL;

	if (reg_protocol_struct != NULL)
	{
		for (uint16_t idx = 0; idx < reg_register_count; idx++)
		{
			if (register_address == reg_protocol_struct[idx].address)
			{
				reg = &reg_protocol_struct[idx];
				break;
			}
		}
	}

	return reg;
}


/**
 * @brief Set current register address
 *
 * @param[in] buffer The input data buffer
 */
static void set_address(uint8_t *buffer)
{
	uint16_t register_address = (buffer[0] << 8) | buffer[1];

	const acc_reg_protocol_t *reg = get_register_struct(register_address);

	if (reg != NULL)
	{
		reg_address = register_address;
	}
	else
	{
		reg_error_flags |= ACC_REG_ERROR_FLAG_ADDRESS_ERROR;
		reg_address      = REG_INVALID_ADDRESS;
	}
}


/**
 * @brief Return true if register is writable
 */
static bool register_is_writable(void)
{
	const acc_reg_protocol_t *reg = get_register_struct(reg_address);

	return (reg != NULL) && (reg->write != NULL);
}


/**
 * @brief Read register at current register address and increase address
 *
 * @param[out] buffer The output data buffer
 */
static void read_register(uint8_t *buffer)
{
	uint32_t data = 0;

	const acc_reg_protocol_t *reg = get_register_struct(reg_address);

	if (reg != NULL)
	{
		if (reg->read != NULL)
		{
			reg->read(&data);
		}
		else
		{
			/* Register read error, read a write only register */
			data = REG_ERROR_READ_OF_WO_REG;
		}
	}
	else
	{
		/* Register read outside of address space */
		data = REG_ERROR_READ_ADDRESS;
	}

	buffer[0] = (data >> 24) & 0xff;
	buffer[1] = (data >> 16) & 0xff;
	buffer[2] = (data >> 8) & 0xff;
	buffer[3] = (data >> 0) & 0xff;

	/* Increase reg addess */
	if (reg_address < REG_INVALID_ADDRESS)
	{
		reg_address++;
	}
}


/**
 * @brief Write register at current register address and increase address
 *
 * @param[in] buffer The input data buffer
 */
static bool write_register(uint8_t *buffer)
{
	bool status = true;

	uint32_t data =
		(((uint32_t)buffer[0]) << 24) |
		(((uint32_t)buffer[1]) << 16) |
		(((uint32_t)buffer[2]) << 8) |
		(((uint32_t)buffer[3]) << 0);

	const acc_reg_protocol_t *reg = get_register_struct(reg_address);

	if (reg != NULL)
	{
		if (reg->write != NULL)
		{
			if (!reg->write(data))
			{
				reg_error_flags |= ACC_REG_ERROR_FLAG_WRITE_FAILED;
				status           = false;
			}
		}
		else
		{
			/* Register write error, write to read only register */
			reg_error_flags |= ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY;
			status           = false;
		}
	}
	else
	{
		/* Register read outside of address space */
		reg_error_flags |= ACC_REG_ERROR_FLAG_ADDRESS_ERROR;
		status           = false;
	}

	/* Increase reg addess */
	if (reg_address < REG_INVALID_ADDRESS)
	{
		reg_address++;
	}

	return status;
}


void acc_reg_protocol_setup(const acc_reg_protocol_t *protocol_struct, uint16_t register_count)
{
	reg_protocol_struct = protocol_struct;
	reg_register_count  = register_count;

	acc_reg_protocol_reset();
}


void acc_reg_protocol_reset(void)
{
	reg_state   = REG_STATE_WAIT_FOR_ADDRESS;
	reg_address = REG_INVALID_ADDRESS;
}


bool acc_reg_protocol_data_nack(void)
{
	if ((reg_state == REG_STATE_NACK_NEXT_WRITE) ||
	    (reg_state == REG_STATE_WAIT_FOR_READ))
	{
		return true;
	}

	return false;
}


void acc_reg_protocol_data_in(uint8_t *buffer, size_t data_in_length)
{
	if (data_in_length == ACC_REG_PROTOCOL_ADDRESS_LENGTH)
	{
		if (reg_state == REG_STATE_WAIT_FOR_ADDRESS)
		{
			set_address(buffer);

			if (register_is_writable())
			{
				/* Register can be read or written */
				reg_state = REG_STATE_WAIT_FOR_READ_OR_WRITE;
			}
			else
			{
				/**
				 * - Read only register
				 * - No valid address, read is still possoble
				 */
				reg_state = REG_STATE_WAIT_FOR_READ;
			}
		}
		else
		{
			/* FAIL: Incorrect state */
			reg_error_flags |= ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR;
			reg_state        = REG_STATE_NACK_NEXT_WRITE;
		}
	}
	else if (data_in_length == ACC_REG_PROTOCOL_REGDATA_LENGTH)
	{
		if ((reg_state == REG_STATE_WAIT_FOR_READ_OR_WRITE) ||
		    (reg_state == REG_STATE_WAIT_FOR_WRITE))
		{
			if (write_register(buffer) && register_is_writable())
			{
				/* OK: Another write is possible */
				reg_state = REG_STATE_WAIT_FOR_WRITE;
			}
			else
			{
				/**
				 * - Write register failed
				 * - Next register is read only
				 */
				reg_state = REG_STATE_NACK_NEXT_WRITE;
			}
		}
		else
		{
			/* FAIL: Incorrect state */
			reg_error_flags |= ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR;
			reg_state        = REG_STATE_NACK_NEXT_WRITE;
		}
	}
	else
	{
		/* FAIL: Incorrect packet length */
		reg_error_flags |= ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR;
		reg_state        = REG_STATE_NACK_NEXT_WRITE;
	}
}


void acc_reg_protocol_data_out(uint8_t *buffer, size_t data_out_length)
{
	if ((reg_state == REG_STATE_WAIT_FOR_READ_OR_WRITE) ||
	    (reg_state == REG_STATE_WAIT_FOR_READ))
	{
		reg_state = REG_STATE_WAIT_FOR_READ;

		if (data_out_length == ACC_REG_PROTOCOL_REGDATA_LENGTH)
		{
			read_register(buffer);
		}
		else
		{
			/* FAIL: Incorrect data length for register data */
			reg_error_flags |= ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR;
		}
	}
	else
	{
		/* FAIL: Incorrect state */
		reg_error_flags |= ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR;
	}
}


uint32_t acc_reg_protocol_get_error_flags(void)
{
	uint32_t error_flags = reg_error_flags;

	/* Clear flags */
	reg_error_flags = 0;

	return error_flags;
}


int32_t acc_reg_protocol_float_to_int32_milli(float value)
{
	return (int32_t)(value*1000.0f);
}


uint32_t acc_reg_protocol_float_to_uint32_milli(float value)
{
	return (uint32_t)(value*1000.0f);
}


float acc_reg_protocol_int32_milli_to_float(int32_t value)
{
	float val = (float)value;

	val = val / 1000.0f;
	return val;
}


float acc_reg_protocol_uint32_milli_to_float(uint32_t value)
{
	float val = (float)value;

	val = val / 1000.0f;
	return val;
}
