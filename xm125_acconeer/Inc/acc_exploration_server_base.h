// Copyright (c) Acconeer AB, 2020-2022
// All rights reserved

#ifndef ACC_EXPLORATION_SERVER_BASE_H_
#define ACC_EXPLORATION_SERVER_BASE_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_definitions_common.h"

typedef enum
{
	ACC_EXPLORATION_SERVER_WAITING = 0,
	ACC_EXPLORATION_SERVER_STREAMING,
	ACC_EXPLORATION_SERVER_STOPPED,
}
acc_exploration_server_state_t;

/**
 * @brief Function that writes data back to the client
 *
 * @param[in] data The data to be written
 * @param[in] size The size of the data in bytes
 */
typedef void (write_data_function_t)(const void *data, uint32_t size);


/**
 * @brief Function that stops streaming
 */
typedef void (restart_input_function_t)(void);


/**
 * @brief Set baudrate of server
 */
typedef void (set_baudrate_function_t)(uint32_t baudrate);


/**
 * @brief Get tick function of server
 */
typedef uint32_t (get_tick_function_t)(void);


/**
 * @brief Struct to handle input for acc_exploration_server_process_cmds
 */
typedef struct
{
	write_data_function_t    *write;
	restart_input_function_t *restart_input;
	set_baudrate_function_t  *set_baudrate;
	uint32_t                 max_baudrate;
	get_tick_function_t      *get_tick;
	uint32_t                 ticks_per_second;
} exploration_server_interface_t;

/**
 * @brief Initialize the exploration server
 *
 * @param[in] buffer pointer to a command buffer
 * @param[in] buffer_size the size, in bytes, of the command buffer
 * @param[in] hw The hardware name (module/board/system)
 * @param[in] log_level The log level to use
 *
 * @return true if successful
 */
bool acc_exploration_server_init(char *buffer, size_t buffer_size, const char *hw, acc_log_level_t log_level);


/**
 * @brief Stop the streaming from the exploration server
 */
void acc_exploration_server_stop_streaming(void);


/**
 * @brief De-initialize the exploration server
 */
void acc_exploration_server_deinit(void);


/**
 * @brief Put string from client into input buffer
 *
 * @param[in] buffer data from client
 * @param[in] buffer_size the size, in bytes, of the buffer
 */
void acc_exploration_server_put_buffer_from_client(const void *buffer, size_t buffer_size);


/**
 * @brief Reset and clear the input buffer
 */
void acc_exploration_server_reset_input_buffer(void);


/**
 * @brief The exploration server process function
 *
 * This function will:
 *  - Check the input buffer and process any available commands.
 *  - It will also get frames from the sensor in the selected update rate
 *  - Send the sensor frames to the client.
 * If the server cannot call this function at the rate it expects (the returned ticks until next),
 * the client will not get the update rate it expects.
 * There is no harm in calling this function too often.
 *
 * When the returned state equals ACC_EXPLORATION_SERVER_WAITING the server should call the
 * acc_exploration_server_process function when new client data have been received.
 *
 * When the returned state equals ACC_EXPLORATION_SERVER_STREAMING the server should call the
 * acc_exploration_server_process after maximum ticks_until_next ticks.
 * The acc_exploration_server_process should be called immediately if state
 * equals ACC_EXPLORATION_SERVER_STREAMING and ticks_until_next equals 0
 *
 * @param[in] server_if server interface functions
 * @param[out] state The state the exploration server is in
 * @param[out] ticks_until_next the number of ticks until the next frame is ready
 *
 * @return true if successful
 */
bool acc_exploration_server_process(const exploration_server_interface_t *server_if,
                                    acc_exploration_server_state_t *state, int32_t *ticks_until_next);


/**
 * @brief End exploration server session, called to send end message to client
 *
 * @param[in] server_if server interface functions
 */
void acc_exploration_server_end(const exploration_server_interface_t *server_if);


/**
 * @brief Sends a log as a json package
 *
 * @param[in] write_func See write_data_function_t
 * @param[in] level The severity level for the log
 * @param[in] module The name of the SW module from where the log is called
 * @param[in] buffer The information to be logged
 *
 * @return true if successful
 */
bool acc_exploration_server_send_log(write_data_function_t write_func, acc_log_level_t level, const char *module, const char *buffer);


#endif
