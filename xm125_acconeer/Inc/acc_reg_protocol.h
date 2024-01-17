// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef ACC_REG_PROTOCOL_H_
#define ACC_REG_PROTOCOL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ACC_REG_PROTOCOL_ADDRESS_LENGTH (2U)
#define ACC_REG_PROTOCOL_REGDATA_LENGTH (4U)

#define ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR (1U << 0U)
#define ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR  (1U << 1U)
#define ACC_REG_ERROR_FLAG_ADDRESS_ERROR        (1U << 2U)
#define ACC_REG_ERROR_FLAG_WRITE_FAILED         (1U << 3U)
#define ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY   (1U << 4U)

/**
 * @brief Function to read a register
 *
 * @param[out] data The data to be read
 */
typedef void (acc_reg_read_func_t)(uint32_t *data);

/**
 * @brief Function to writes a register
 *
 * @param[in] data The data to be written
 * @return The access result OK/ERROR
 */
typedef bool (acc_reg_write_func_t)(const uint32_t data);

/**
 * @brief Register access mode and functions struct
 */
typedef struct
{
	uint16_t             address;
	acc_reg_read_func_t  *read;
	acc_reg_write_func_t *write;
} acc_reg_protocol_t;

/**
 * @brief Setup register protocol
 *
 * @param[in] protocol_struct The protocol struct with read/write functions
 * @param[in] register_count The number of registers in the struct
 */
void acc_reg_protocol_setup(const acc_reg_protocol_t *protocol_struct, uint16_t register_count);


/**
 * @brief Reset register protocol
 *
 * This function should be called when a new transaction start.
 */
void acc_reg_protocol_reset(void);


/**
 * @brief Should protocol NACK the next data
 *
 * Return true if protocol should nack the next data write
 */
bool acc_reg_protocol_data_nack(void);


/**
 * @brief Handle data input to the register protocol
 *
 * @param[in] buffer The data to be processed by the protocol
 * @param[in] data_in_length The byte length of the data to be processed
 */
void acc_reg_protocol_data_in(uint8_t *buffer, size_t data_in_length);


/**
 * @brief Handle data input from the register protocol
 *
 * @param[in] buffer The data buffer to be filled with data
 * @param[in] data_out_length The byte length of the data to be read
 */
void acc_reg_protocol_data_out(uint8_t *buffer, size_t data_out_length);


/**
 * @brief Get the error flags for the register protocol
 *
 * The error flags will be cleared by this call
 *
 * ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR
 *   The flag will be set if for example the protocol got unexpected data
 *
 * ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR
 *   The flag will be set if for example the protocol got data with unexpected length
 *
 * ACC_REG_ERROR_FLAG_ADDRESS_ERROR
 *   The flag will be set if a register address outside the address space is used
 *
 * ACC_REG_ERROR_FLAG_WRITE_FAILED
 *   The flag will be set if a register write failed
 *
 * ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY
 *   The flag will be set if a read only register is written
 *
 * Return The protocol error flags
 */
uint32_t acc_reg_protocol_get_error_flags(void);


//
// Register protocol utility functions used for
// conversion between uint32/int32 and float
//


/**
 * @brief Convert 1000 * float to int32
 *
 * @param[in] value A float value
 * @return The float value * 1000 as an int32
 */
int32_t acc_reg_protocol_float_to_int32_milli(float value);


/**
 * @brief Convert 1000 * float to uint32
 *
 * @param[in] value A float value
 * @return The float value * 1000 as an uint32
 */
uint32_t acc_reg_protocol_float_to_uint32_milli(float value);


/**
 * @brief Convert int32 / 1000 to float
 *
 * @param[in] value An int32 value
 * @return The int32 value / 1000 as a float
 */
float acc_reg_protocol_int32_milli_to_float(int32_t value);


/**
 * @brief Convert uint32 / 1000 to float
 *
 * @param[in] value An uint32 value
 * @return The uint32 value / 1000 as a float
 */
float acc_reg_protocol_uint32_milli_to_float(uint32_t value);


#endif
