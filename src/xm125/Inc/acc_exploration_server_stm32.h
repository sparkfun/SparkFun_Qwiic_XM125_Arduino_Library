// Copyright (c) Acconeer AB, 2021-2022
// All rights reserved

#ifndef ACC_EXPLORATION_SERVER_STM32_H_
#define ACC_EXPLORATION_SERVER_STM32_H_

#include "acc_exploration_server_base.h"

/**
 * @brief Init the stm32 exploration server
 *
 * @param[in] hw The name of the hardware platform
 * @param[in] buffer The buffer used for receiving commands from the client
 * @param[in] buffer_size The size of the command buffer
 */
void acc_exploration_server_stm32_init(const char *hw, char *buffer, size_t buffer_size);


/**
 * @brief Start the stm32 exploration server
 */
void acc_exploration_server_stm32_main(void);


#endif
