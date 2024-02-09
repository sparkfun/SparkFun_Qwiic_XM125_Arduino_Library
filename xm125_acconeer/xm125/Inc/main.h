/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INTERRUPT_Pin           GPIO_PIN_3
#define INTERRUPT_GPIO_Port     GPIOB
#define INTERRUPT_EXTI_IRQn     EXTI3_IRQn
#define MISC_GPIO2_Pin          GPIO_PIN_3
#define MISC_GPIO2_GPIO_Port    GPIOH
#define WAKE_UP_Pin             GPIO_PIN_11
#define WAKE_UP_GPIO_Port       GPIOA
#define WAKE_UP_EXTI_IRQn       EXTI15_10_IRQn
#define DEBUG_UART_RX_Pin       GPIO_PIN_10
#define DEBUG_UART_RX_GPIO_Port GPIOA
#define MCU_INT_Pin             GPIO_PIN_9
#define MCU_INT_GPIO_Port       GPIOB
#define DEBUG_UART_TX_Pin       GPIO_PIN_9
#define DEBUG_UART_TX_GPIO_Port GPIOA
#define I2C_ADDRESS_Pin         GPIO_PIN_0
#define I2C_ADDRESS_GPIO_Port   GPIOH
#define I2C_SCL_Pin             GPIO_PIN_10
#define I2C_SCL_GPIO_Port       GPIOB
#define UART_RX_Pin             GPIO_PIN_3
#define UART_RX_GPIO_Port       GPIOA
#define UART_TX_Pin             GPIO_PIN_2
#define UART_TX_GPIO_Port       GPIOA
#define ENABLE_Pin              GPIO_PIN_12
#define ENABLE_GPIO_Port        GPIOB
#define I2C_SDA_Pin             GPIO_PIN_11
#define I2C_SDA_GPIO_Port       GPIOB
#define SPI_MOSI_Pin            GPIO_PIN_7
#define SPI_MOSI_GPIO_Port      GPIOA
#define SPI_MISO_Pin            GPIO_PIN_6
#define SPI_MISO_GPIO_Port      GPIOA
#define SPI_SCK_Pin             GPIO_PIN_5
#define SPI_SCK_GPIO_Port       GPIOA
#define UART_CTS_Pin            GPIO_PIN_0
#define UART_CTS_GPIO_Port      GPIOA
#define MISC_GPIO0_Pin          GPIO_PIN_1
#define MISC_GPIO0_GPIO_Port    GPIOB
#define SPI_SS_Pin              GPIO_PIN_0
#define SPI_SS_GPIO_Port        GPIOB
#define MISC_GPIO1_Pin          GPIO_PIN_4
#define MISC_GPIO1_GPIO_Port    GPIOA
#define UART_RTS_Pin            GPIO_PIN_1
#define UART_RTS_GPIO_Port      GPIOA
/* USER CODE BEGIN Private defines */

#define A121_SPI_HANDLE     hspi1
#define MODULE_I2C_HANDLE   hi2c2
#define MODULE_RTC_HANDLE   hrtc
#define MODULE_UART1_HANDLE huart2
#define MODULE_UART2_HANDLE huart1

#define MAIN_UART_HANDLE  huart2
#define DEBUG_UART_HANDLE huart1

#define EXPLORATION_SERVER_UART_HANDLE huart2

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
