// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stddef.h>

#include "main.h"

#include "acc_integration.h"
#include "acc_reg_protocol.h"
#include "i2c_application_system.h"

#define I2C_SLAVE_BUFFER_SIZE           4
#define WAIT_FOR_IDLE_RETRIES           10U
#define WAIT_FOR_IDLE_RETRY_INTERNAL_MS 10U
#define GPIO_BANK_COUNT                 3U

typedef struct
{
	uint32_t MODER;               /*!< GPIO port mode register              */
	uint32_t OTYPER;              /*!< GPIO port output type register       */
	uint32_t OSPEEDR;             /*!< GPIO port output speed register      */
	uint32_t PUPDR;               /*!< GPIO port pull-up/pull-down register */
	uint32_t AFR[2];              /*!< GPIO alternate function registers    */
	uint32_t ASCR;                /*!< GPIO analog switch control register  */
	uint32_t RCC_GPIO_CLK_ENABLE; /*!< GPIO Port Clock Enable               */
} gpio_config_t;

extern SPI_HandleTypeDef  A121_SPI_HANDLE;
extern I2C_HandleTypeDef  MODULE_I2C_HANDLE;
extern UART_HandleTypeDef MODULE_UART1_HANDLE;
extern UART_HandleTypeDef MODULE_UART2_HANDLE;
extern RTC_HandleTypeDef  MODULE_RTC_HANDLE;
static RCC_OscInitTypeDef low_power_saved_rcc_oscinitstruct;
static RCC_ClkInitTypeDef low_power_saved_rcc_clkinitstruct;
static uint32_t           low_power_saved_flatency;
static gpio_config_t      low_power_saved_gpio_bank[GPIO_BANK_COUNT] = { 0 };
static GPIO_TypeDef       *gpio_banks[GPIO_BANK_COUNT]               = { GPIOA, GPIOB, GPIOH };

static uint8_t       i2c_slave_buffer[I2C_SLAVE_BUFFER_SIZE];
static volatile bool i2c_idle = true;

static uint32_t      rtc_period_time_ms   = 0;
static volatile bool rtc_wakeup_triggered = false;

/**
 * @brief Disable interrupts
 */
static inline void disable_interrupts(void);


/**
 * @brief Enable interrupts
 */
static inline void enable_interrupts(void);


/**
 * @brief Prepare power down state
 */
static inline void prepare_power_down(void);


/**
 * @brief Resume power down state
 */
static inline void resume_power_down(void);


/**
 * @brief Get RTC ticks based on current RTC time
 *
 * @return The current RTC ticks in ms
 */
static uint32_t get_rtc_tick(void);


/**
 * @brief Convert RTC ticks to RTC time
 *
 * @param[in] tick rtc ticks in ms
 * @param[out] time RTC time
 */
static void rtc_tick_to_time(uint32_t tick, RTC_TimeTypeDef *time);


/**
 * @brief Function for setting the next wakeup time from the RTC interrupt.
 */
static void rtc_set_next_wakeup_time(void);


/**
 * @brief Wait for I2C interface to be idle
 */
static void wait_for_i2c_idle(void);


/**
 * @brief Helper function to prepare transmit of register data
 *
 * @param[in] hi2c The I2C handle
 */
static void prepare_register_data(I2C_HandleTypeDef *hi2c);


void i2c_application_system_init(void)
{
	/* Disable unused functionality in MCU */
	HAL_UART_DeInit(&MODULE_UART1_HANDLE);

	/* Start I2C */
	if (HAL_I2C_EnableListen_IT(&MODULE_I2C_HANDLE) != HAL_OK)
	{
		/**
		 * Calling this function with wrong parameters will return HAL_ERROR
		 * Calling this function when the i2c module is in the wrong state return HAL_BUSY
		 */
		Error_Handler();
	}

	i2c_application_system_set_ready_pin(true);
}


void i2c_application_system_reset(void)
{
	/**
	 * Wait for the i2c interface to be idle,
	 * if not idle after retries, reset anyway!
	 */
	wait_for_i2c_idle();

	NVIC_SystemReset();
}


void i2c_application_system_wait_for_interrupt(void)
{
	__WFI();
}


bool i2c_application_system_test_wakeup_pin(void)
{
	GPIO_PinState pin_state = HAL_GPIO_ReadPin(WAKE_UP_GPIO_Port, WAKE_UP_Pin);

	return pin_state == GPIO_PIN_SET;
}


void i2c_application_system_set_ready_pin(bool enable)
{
	GPIO_PinState pin_state = enable ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(MCU_INT_GPIO_Port, MCU_INT_Pin, pin_state);
}


void i2c_application_system_setup_generic_gpio_pin(bool enable)
{
	/* Setup generic gpio pin (MISC_GPIO0) */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin  = MISC_GPIO0_Pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	if (enable)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	}

	HAL_GPIO_Init(MISC_GPIO0_GPIO_Port, &GPIO_InitStruct);
}


void i2c_application_system_set_generic_gpio_pin(bool enable)
{
	GPIO_PinState pin_state = enable ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(MISC_GPIO0_GPIO_Port, MISC_GPIO0_Pin, pin_state);
}


void i2c_application_enter_low_power_state(void)
{
	/**
	 * Wait for the i2c interface to be idle,
	 * if not idle after retries, power down anyway!
	 */
	wait_for_i2c_idle();

	prepare_power_down();

	disable_interrupts();

	/* Do a final test of the wakeup pin and rtc wakeup before going to sleep */
	if (!i2c_application_system_test_wakeup_pin() && !rtc_wakeup_triggered)
	{
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	}

	enable_interrupts();

	resume_power_down();
}


void i2c_application_set_periodic_wakeup(uint32_t period_ms)
{
	rtc_period_time_ms = period_ms;
	rtc_set_next_wakeup_time();
}


bool i2c_application_is_periodic_wakeup(void)
{
	acc_integration_critical_section_enter();
	bool periodic_wakeup = rtc_wakeup_triggered;
	rtc_wakeup_triggered = false;
	acc_integration_critical_section_exit();

	if (periodic_wakeup)
	{
		rtc_set_next_wakeup_time();
	}

	return periodic_wakeup;
}


/**
 * @brief IRQ Handler for RTC B Alarm
 */
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *rtc)
{
	(void)rtc;
	rtc_wakeup_triggered = true;
}


static inline void disable_interrupts(void)
{
	__disable_irq();
	__DSB();
	__ISB();
}


static inline void enable_interrupts(void)
{
	__enable_irq();
	__DSB();
	__ISB();
}


static inline void prepare_power_down(void)
{
	HAL_RCC_GetOscConfig(&low_power_saved_rcc_oscinitstruct);
	HAL_RCC_GetClockConfig(&low_power_saved_rcc_clkinitstruct, &low_power_saved_flatency);

	HAL_I2C_DeInit(&MODULE_I2C_HANDLE);
	HAL_SPI_DeInit(&A121_SPI_HANDLE);
	HAL_UART_DeInit(&MODULE_UART1_HANDLE);

	/* Save GPIO bank state */
	for (size_t idx = 0; idx < GPIO_BANK_COUNT; idx++)
	{
		GPIO_TypeDef  *gpio_bank   = gpio_banks[idx];
		gpio_config_t *gpio_config = &low_power_saved_gpio_bank[idx];
		gpio_config->MODER   = READ_REG(gpio_bank->MODER);
		gpio_config->OTYPER  = READ_REG(gpio_bank->OTYPER);
		gpio_config->OSPEEDR = READ_REG(gpio_bank->OSPEEDR);
		gpio_config->PUPDR   = READ_REG(gpio_bank->PUPDR);
		gpio_config->AFR[0]  = READ_REG(gpio_bank->AFR[0]);
		gpio_config->AFR[1]  = READ_REG(gpio_bank->AFR[1]);
	}

	/* Turn GPIO off */
	GPIO_InitTypeDef GPIO_InitStructOff;

	/* Set all unused GPIO pins in the lowest power consuming state according to AN4899*/
	GPIO_InitStructOff.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructOff.Pull = GPIO_NOPULL;

	/* GPIO A */
	GPIO_InitStructOff.Pin  = GPIO_PIN_All;
	GPIO_InitStructOff.Pin &= (uint16_t) ~(WAKE_UP_Pin | SPI_SCK_Pin | SPI_MISO_Pin | SPI_MOSI_Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructOff);

	/* GPIO B */
	GPIO_InitStructOff.Pin  = GPIO_PIN_All;
	GPIO_InitStructOff.Pin &= (uint16_t) ~(MCU_INT_Pin | ENABLE_Pin | SPI_SS_Pin | MISC_GPIO0_Pin);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructOff);

	/* GPIO H */
	GPIO_InitStructOff.Pin = GPIO_PIN_All;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructOff);

	/* Disable Clocks */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();

	HAL_SuspendTick();
}


static inline void resume_power_down(void)
{
	HAL_ResumeTick();

	HAL_RCC_OscConfig(&low_power_saved_rcc_oscinitstruct);
	HAL_RCC_ClockConfig(&low_power_saved_rcc_clkinitstruct, low_power_saved_flatency);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	/* Save GPIO bank state */
	for (size_t idx = 0; idx < GPIO_BANK_COUNT; idx++)
	{
		GPIO_TypeDef  *gpio_bank   = gpio_banks[idx];
		gpio_config_t *gpio_config = &low_power_saved_gpio_bank[idx];

		WRITE_REG(gpio_bank->MODER, gpio_config->MODER);
		WRITE_REG(gpio_bank->OTYPER, gpio_config->OTYPER);
		WRITE_REG(gpio_bank->OSPEEDR, gpio_config->OSPEEDR);
		WRITE_REG(gpio_bank->PUPDR, gpio_config->PUPDR);
		WRITE_REG(gpio_bank->AFR[0], gpio_config->AFR[0]);
		WRITE_REG(gpio_bank->AFR[1], gpio_config->AFR[1]);
	}

	HAL_I2C_Init(&MODULE_I2C_HANDLE);
	HAL_SPI_Init(&A121_SPI_HANDLE);
	HAL_UART_Init(&MODULE_UART2_HANDLE);

	/* Start I2C */
	if (HAL_I2C_EnableListen_IT(&MODULE_I2C_HANDLE) != HAL_OK)
	{
		/**
		 * Calling this function with wrong parameters will return HAL_ERROR
		 * Calling this function when the i2c module is in the wrong state return HAL_BUSY
		 */
		Error_Handler();
	}
}


static uint32_t get_rtc_tick(void)
{
	RTC_DateTypeDef rtc_date = { 0 };
	RTC_TimeTypeDef rtc_time = { 0 };

	/*  Wait until any pending shift operation is completed */
	while ((MODULE_RTC_HANDLE.Instance->ISR & RTC_ISR_SHPF) != RESET)
	{
		;
	}

	if (HAL_RTC_GetTime(&MODULE_RTC_HANDLE, &rtc_time, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_RTC_GetDate(&MODULE_RTC_HANDLE, &rtc_date, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	uint32_t rtc_ticks_ms = 0;

	if (rtc_time.Hours)
	{
		rtc_ticks_ms += rtc_time.Hours * 60 * 60 * 1000;
	}

	if (rtc_time.Minutes)
	{
		rtc_ticks_ms += rtc_time.Minutes * 60 * 1000;
	}

	if (rtc_time.Seconds)
	{
		rtc_ticks_ms += rtc_time.Seconds * 1000;
	}

	rtc_ticks_ms += ((rtc_time.SecondFraction - rtc_time.SubSeconds) * 1000) / (rtc_time.SecondFraction + 1);

	return rtc_ticks_ms;
}


static void rtc_tick_to_time(uint32_t tick, RTC_TimeTypeDef *time)
{
	uint32_t rtc_ticks_ms = tick;

	time->SecondFraction = MODULE_RTC_HANDLE.Init.SynchPrediv;

	time->Hours = (rtc_ticks_ms / (60 * 60 * 1000)) % 24;

	rtc_ticks_ms = rtc_ticks_ms % (60 * 60 * 1000);

	time->Minutes = (rtc_ticks_ms / (60 * 1000)) % 60;

	rtc_ticks_ms = rtc_ticks_ms % (60 * 1000);

	time->Seconds = (rtc_ticks_ms / 1000) % 60;

	rtc_ticks_ms = rtc_ticks_ms % 1000;

	time->SubSeconds = time->SecondFraction - (rtc_ticks_ms * (time->SecondFraction + 1)) / 1000;
}


static void rtc_set_next_wakeup_time(void)
{
	RTC_AlarmTypeDef alarm = { {0}, 0, 0, 0, 0, 0, 0};

	if (rtc_period_time_ms > 0)
	{
		rtc_tick_to_time(get_rtc_tick() + rtc_period_time_ms, &alarm.AlarmTime);

		alarm.Alarm              = RTC_ALARM_B;
		alarm.AlarmMask          = RTC_ALARMMASK_DATEWEEKDAY;
		alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_SS14;

		if (HAL_RTC_DeactivateAlarm(&MODULE_RTC_HANDLE, RTC_ALARM_B) != HAL_OK)
		{
			Error_Handler();
		}

		if (HAL_RTC_SetAlarm_IT(&MODULE_RTC_HANDLE, &alarm, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
	}
	else
	{
		if (HAL_RTC_DeactivateAlarm(&MODULE_RTC_HANDLE, RTC_ALARM_B) != HAL_OK)
		{
			Error_Handler();
		}
	}
}


static void wait_for_i2c_idle(void)
{
	for (size_t idx = 0U; idx < WAIT_FOR_IDLE_RETRIES; idx++)
	{
		/* Make sure we do not have a race for i2c_idle */
		acc_integration_critical_section_enter();
		bool idle = i2c_idle;
		acc_integration_critical_section_exit();

		if (idle)
		{
			break;
		}

		/* Wait before next retry */
		HAL_Delay(WAIT_FOR_IDLE_RETRY_INTERNAL_MS);
	}
}


static void prepare_register_data(I2C_HandleTypeDef *hi2c)
{
	/* Read register and put in buffer */
	acc_reg_protocol_data_out(i2c_slave_buffer, ACC_REG_PROTOCOL_REGDATA_LENGTH);

	/* Prepare buffer for transmit */
	if (HAL_I2C_Slave_Seq_Transmit_IT(hi2c,
	                                  i2c_slave_buffer,
	                                  ACC_REG_PROTOCOL_REGDATA_LENGTH,
	                                  I2C_NEXT_FRAME) != HAL_OK)
	{
		/**
		 * Calling this function with wrong parameters will return HAL_ERROR
		 * Calling this function when the i2c module is in the wrong state return HAL_BUSY
		 */
		Error_Handler();
	}
}


/*
 * STM32 I2C Interrupt callbacks
 */


void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	(void)AddrMatchCode;

	i2c_idle = false;

	/* Our i2c address was detected, transmit or receive? */
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)
	{
		/* Reset protocol before receiving new bytes from master */
		acc_reg_protocol_reset();

		/* Always start by getting register address, 2 bytes */
		if (HAL_I2C_Slave_Seq_Receive_IT(hi2c,
		                                 i2c_slave_buffer,
		                                 ACC_REG_PROTOCOL_ADDRESS_LENGTH,
		                                 I2C_NEXT_FRAME) != HAL_OK)
		{
			/**
			 * Calling this function with wrong parameters will return HAL_ERROR
			 * Calling this function when the i2c module is in the wrong state return HAL_BUSY
			 */
			Error_Handler();
		}
	}
	else
	{
		/* Prepare register data to be read by master */
		prepare_register_data(hi2c);
	}
}


void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	/* Prepare register data to be read by master */
	prepare_register_data(hi2c);
}


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	/* Get number of received bytes */
	uint16_t input_length = (uint16_t)((uintptr_t)hi2c->pBuffPtr - (uintptr_t)i2c_slave_buffer);

	if (input_length == ACC_REG_PROTOCOL_ADDRESS_LENGTH)
	{
		/* Handle receive register address */
		acc_reg_protocol_data_in(i2c_slave_buffer, input_length);
	}
	else if (input_length == ACC_REG_PROTOCOL_REGDATA_LENGTH)
	{
		/* Handle receive register data (write register) */
		acc_reg_protocol_data_in(i2c_slave_buffer, input_length);
	}

	/* NACK next write if an error has occured */
	if (acc_reg_protocol_data_nack())
	{
		__HAL_I2C_GENERATE_NACK(hi2c);
	}

	/* Prepare to receive register data (4 bytes) */
	if (HAL_I2C_Slave_Seq_Receive_IT(hi2c,
	                                 i2c_slave_buffer,
	                                 ACC_REG_PROTOCOL_REGDATA_LENGTH,
	                                 I2C_NEXT_FRAME) != HAL_OK)
	{
		/**
		 * Calling this function with wrong parameters will return HAL_ERROR
		 * Calling this function when the i2c module is in the wrong state return HAL_BUSY
		 */
		Error_Handler();
	}
}


void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2c_idle = true;

	/* I2C transaction done, start listening for new */
	if (HAL_I2C_EnableListen_IT(hi2c) != HAL_OK)
	{
		/**
		 * Calling this function with wrong parameters will return HAL_ERROR
		 * Calling this function when the i2c module is in the wrong state return HAL_BUSY
		 */
		Error_Handler();
	}
}


void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	/* Has an I2C error occured? */
	if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF)
	{
		/**
		 * The data NACK is the only valid error code
		 */
		Error_Handler();
	}
}
