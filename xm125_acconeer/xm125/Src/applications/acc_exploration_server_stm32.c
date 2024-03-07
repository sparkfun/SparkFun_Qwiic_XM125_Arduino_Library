// Copyright (c) Acconeer AB, 2021-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


#include "acc_exploration_server_base.h"
#include "acc_exploration_server_stm32.h"
#include "acc_integration.h"
#include "acc_integration_log.h"

#define MODULE "exploration_server_stm32"

#define LOG_BUFFER_MAX_SIZE 150

#define STM32_EXPLORATION_SERVER_MAX_BAUDRATE 2000000

extern UART_HandleTypeDef EXPLORATION_SERVER_UART_HANDLE;

static char   *command_buffer     = NULL;
static size_t command_buffer_size = 0;


#define UART_DMA_BUFFER_SIZE (8192)

static volatile bool uart_tx_active = false;
static uint8_t       uart_dma_buffer[UART_DMA_BUFFER_SIZE];

static char exploration_server_command_buffer[2048];


static void uart_wait_for_tx_done(void)
{
	while (true)
	{
		__disable_irq();
		bool active = uart_tx_active;
		__enable_irq();
		if (active)
		{
			__WFI();
		}
		else
		{
			break;
		}
	}
}


/**
 * @brief Write data to client (UART)
 *
 * @param[in] data pointer to data
 * @param[in] size data size in bytes
 *
 * @return true if successful
 */
static void write_to_client(const void *data, uint32_t size)
{
	const uint8_t *data8 = (const uint8_t *)data;

	size_t pos = 0;

	while (pos < size)
	{
		/* Wait for previous transmit to be done */
		uart_wait_for_tx_done();

		size_t this_size = ((size - pos) < UART_DMA_BUFFER_SIZE) ? (size - pos) : UART_DMA_BUFFER_SIZE;
		memcpy(uart_dma_buffer, &data8[pos], this_size);

		uart_tx_active = true;
		HAL_UART_Transmit_DMA(&EXPLORATION_SERVER_UART_HANDLE, uart_dma_buffer, this_size);
		pos += this_size;
	}
}


/**
 * @brief Restart input of new command
 */
static void restart_input(void)
{
	memset(command_buffer, 0, command_buffer_size);

	/**
	 * Stop previous reception
	 * Clear buffer
	 * Re-start receive after a line has been received.
	 */
	HAL_UART_AbortReceive(&EXPLORATION_SERVER_UART_HANDLE);
	HAL_UART_Receive_DMA(&EXPLORATION_SERVER_UART_HANDLE, (uint8_t *)command_buffer, command_buffer_size);
}


static void set_baudrate(uint32_t baudrate)
{
	/* Wait for previous transmit to be done before changing baudrate */
	uart_wait_for_tx_done();

	HAL_UART_AbortReceive(&EXPLORATION_SERVER_UART_HANDLE);
	HAL_UART_DeInit(&EXPLORATION_SERVER_UART_HANDLE);
	EXPLORATION_SERVER_UART_HANDLE.Init.BaudRate = baudrate;
	if (HAL_UART_Init(&EXPLORATION_SERVER_UART_HANDLE) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_UART_Receive_DMA(&EXPLORATION_SERVER_UART_HANDLE, (uint8_t *)command_buffer, command_buffer_size);
}


static uint32_t get_tick(void)
{
	return acc_integration_get_time();
}


/**
 * @brief Server interface functions
 */
static const exploration_server_interface_t server_if = {
	.write            = write_to_client,
	.restart_input    = restart_input,
	.set_baudrate     = set_baudrate,
	.max_baudrate     = STM32_EXPLORATION_SERVER_MAX_BAUDRATE,
	.get_tick         = get_tick,
	.ticks_per_second = 1000,
};


void acc_exploration_server_stm32_init(const char *hw, char *buffer, size_t buffer_size)
{
	command_buffer      = buffer;
	command_buffer_size = buffer_size;
	acc_exploration_server_init(command_buffer, command_buffer_size, hw, ACC_LOG_LEVEL_INFO);
}


void acc_exploration_server_stm32_main(void)
{
	/**
	 * The embedded exploration server for STM32 can stream UART input data
	 * directly into the input buffer.
	 *
	 * Setup DMA into buffer with buffer_size.
	 */
	HAL_UART_Receive_DMA(&EXPLORATION_SERVER_UART_HANDLE, (uint8_t *)command_buffer, command_buffer_size);

	while (true)
	{
		/* Default values */
		acc_exploration_server_state_t state            = ACC_EXPLORATION_SERVER_WAITING;
		int32_t                        ticks_until_next = 0;

		if (acc_exploration_server_process(&server_if, &state, &ticks_until_next))
		{
			switch (state)
			{
				case ACC_EXPLORATION_SERVER_STOPPED:
					/* Stop exploration server, not implemented for this target */
					ACC_LOG_ERROR("Exploration server command 'stop_application' not supported.");
					__WFI();
					break;
				case ACC_EXPLORATION_SERVER_WAITING:
					/* Idle time, just wait for next interrupt */
					__WFI();
					break;
				case ACC_EXPLORATION_SERVER_STREAMING:
					if (ticks_until_next > 1)
					{
						/* More than 1 tick to sleep, wait for next interrupt (at the most 1ms away) */
						__WFI();
					}

					break;
			}
		}
		else
		{
			/* Error */
			Error_Handler();
		}
	}
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart == &EXPLORATION_SERVER_UART_HANDLE)
	{
		/* Check framing error bit to detect break condition */
		if ((huart->ErrorCode & HAL_UART_ERROR_FE) == HAL_UART_ERROR_FE)
		{
			HAL_NVIC_SystemReset();
		}
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *h_uart)
{
	if (h_uart == &EXPLORATION_SERVER_UART_HANDLE)
	{
		uart_tx_active = false;
	}
}


void acc_integration_log(acc_log_level_t level, const char *module, const char *format, ...)
{
	char    log_buffer[LOG_BUFFER_MAX_SIZE];
	va_list ap;

	va_start(ap, format);
	int ret = vsnprintf(log_buffer, LOG_BUFFER_MAX_SIZE, format, ap);

	va_end(ap);

	if (ret >= LOG_BUFFER_MAX_SIZE)
	{
		log_buffer[LOG_BUFFER_MAX_SIZE - 4] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 3] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 2] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 1] = 0;
	}

	acc_exploration_server_send_log(server_if.write, level, module, log_buffer);
}


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	printf("Acconeer Exploration Server\n");

	// Wait for host line break to end before starting exploration server
	while (HAL_GPIO_ReadPin(UART_RX_GPIO_Port, UART_RX_Pin) == GPIO_PIN_RESET)
	{
		printf("Waiting for host line break to end...\n");
		HAL_Delay(10);
	}

	// Clear out any pending garbage and errors on the UART line by deinit/init
	// it again. This avoids that the HAL_UART_ErrorCallback() is called (leading
	// to an additional reset cycle) if the RX line was previously low.
	HAL_UART_DeInit(&EXPLORATION_SERVER_UART_HANDLE);
	if (HAL_UART_Init(&EXPLORATION_SERVER_UART_HANDLE) != HAL_OK)
	{
		Error_Handler();
	}

	acc_exploration_server_stm32_init("xm125",
	                                  exploration_server_command_buffer,
	                                  sizeof(exploration_server_command_buffer));
	acc_exploration_server_stm32_main();

	return 0;
}
