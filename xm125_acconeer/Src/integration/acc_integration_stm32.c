// Copyright (c) Acconeer AB, 2019-2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

#include "acc_integration.h"


/**
 * @brief Maximum RTC time before wrapping occurs
 */
#define RTC_MAX_TIME_MS (24*60*60*1000)


/**
 * @brief Set to true when RTC alarm interrupt has triggered
 */
volatile bool rtc_alarm_triggered = false;


/**
 * @brief Set to true when RTC wakeup interrupt has triggered
 */
volatile bool rtc_wakeup_triggered = false;


/**
 * @brief Variable that holds the periodic wakeup time
 */
static uint32_t periodic_sleep_time_ms = 0;


/**
 * @brief GPIO config status, maps directly to the GPIO registers
 */
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


/**
 * @brief GPIO status of the system before going to sleep
 */
static gpio_config_t saved_gpio_status[3];


/**
 * @brief Handles for the blocks to control
 */
extern RTC_HandleTypeDef  MODULE_RTC_HANDLE;
extern SPI_HandleTypeDef  A121_SPI_HANDLE;
extern I2C_HandleTypeDef  MODULE_I2C_HANDLE;
extern UART_HandleTypeDef MODULE_UART1_HANDLE;
extern UART_HandleTypeDef MODULE_UART2_HANDLE;

extern __IO uint32_t uwTick;

static RCC_OscInitTypeDef prepare_saved_rcc_oscinitstruct;
static RCC_ClkInitTypeDef prepare_saved_rcc_clkinitstruct;
static uint32_t           prepare_saved_flatency;

static HAL_UART_StateTypeDef prepare_saved_uart1_rx_state;
static HAL_I2C_StateTypeDef  prepare_saved_i2c_state;


/**
 * @brief Convert RTC time to RTC ticks
 *
 * @param[in] time RTC time
 * @return rtc ticks in ms
 */
static uint32_t rtc_time_to_tick(RTC_TimeTypeDef *time)
{
	uint32_t rtc_ticks_ms = 0;

	if (time->Hours)
	{
		rtc_ticks_ms += time->Hours * 60 * 60 * 1000;
	}

	if (time->Minutes)
	{
		rtc_ticks_ms += time->Minutes * 60 * 1000;
	}

	if (time->Seconds)
	{
		rtc_ticks_ms += time->Seconds * 1000;
	}

	rtc_ticks_ms += ((time->SecondFraction - time->SubSeconds) * 1000) / (time->SecondFraction + 1);

	return rtc_ticks_ms;
}


/**
 * @brief Convert RTC ticks to RTC time
 *
 * @param[in] tick rtc ticks in ms
 * @param[out] time RTC time
 */
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


/**
 * @brief Get RTC ticks based on current RTC time
 *
 * @return The current RTC ticks in ms
 */
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

	return rtc_time_to_tick(&rtc_time);
}


/**
 * @brief Update system tick based on RTC ticks during sleep
 *
 * @param[in] rtc_ticks_pre_sleep  rtc ticks before sleep was entered
 * @param[in] rtc_ticks_post_sleep rtc ticks after sleep was exited
 */
static void update_system_tick(uint32_t rtc_ticks_pre_sleep, uint32_t rtc_ticks_post_sleep)
{
	// Take care of wrapping which will occur every 24h
	int32_t elapsed_ticks = (int32_t)rtc_ticks_post_sleep - rtc_ticks_pre_sleep;

	if (elapsed_ticks < 0)
	{
		elapsed_ticks = RTC_MAX_TIME_MS + elapsed_ticks;
	}

	uwTick += (uint32_t)elapsed_ticks;
}


/**
 * @brief Function for setting the next wakeup time from the RTC interrupt.
 */
static void rtc_set_next_wakeup_time(void)
{
	RTC_AlarmTypeDef alarm = { {0}, 0, 0, 0, 0, 0, 0};

	if (periodic_sleep_time_ms != 0)
	{
		rtc_tick_to_time(get_rtc_tick() + periodic_sleep_time_ms, &alarm.AlarmTime);

		alarm.Alarm              = RTC_ALARM_A;
		alarm.AlarmMask          = RTC_ALARMMASK_DATEWEEKDAY;
		alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_SS14;

		if (HAL_RTC_DeactivateAlarm(&MODULE_RTC_HANDLE, RTC_ALARM_A) != HAL_OK)
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
		if (HAL_RTC_DeactivateAlarm(&MODULE_RTC_HANDLE, RTC_ALARM_A) != HAL_OK)
		{
			Error_Handler();
		}

		rtc_alarm_triggered = false;
	}
}


/**
 * @brief Set the port clock status for a given GPIO bank
 *
 * @param[in] gpio_bank GPIO port clock to set
 * @param[in] enable True to enable clock, False to disable
 */
static void enable_gpio_port_clock(GPIO_TypeDef *gpio_bank, bool enable)
{
	if (gpio_bank == GPIOA)
	{
		if (enable)
		{
			__HAL_RCC_GPIOA_CLK_ENABLE();
		}
		else
		{
			__HAL_RCC_GPIOA_CLK_DISABLE();
		}
	}
	else if (gpio_bank == GPIOB)
	{
		if (enable)
		{
			__HAL_RCC_GPIOB_CLK_ENABLE();
		}
		else
		{
			__HAL_RCC_GPIOB_CLK_DISABLE();
		}
	}
	else if (gpio_bank == GPIOH)
	{
		if (enable)
		{
			__HAL_RCC_GPIOH_CLK_ENABLE();
		}
		else
		{
			__HAL_RCC_GPIOH_CLK_DISABLE();
		}
	}
}


/**
 * @brief Get the port clock status for a given GPIO bank
 *
 * @param[in] gpio_bank GPIO bank to save
 * @return True if the GPIO port clock is enabled
 */
static bool get_gpio_port_clock(GPIO_TypeDef *gpio_bank)
{
	if (gpio_bank == GPIOA)
	{
		return __HAL_RCC_GPIOA_IS_CLK_ENABLED();
	}
	else if (gpio_bank == GPIOB)
	{
		return __HAL_RCC_GPIOB_IS_CLK_ENABLED();
	}
	else if (gpio_bank == GPIOH)
	{
		return __HAL_RCC_GPIOH_IS_CLK_ENABLED();
	}

	/* We should never end up here */
	Error_Handler();
	return false;
}


/**
 * @brief Save all registers for a given GPIO bank including the original GPIO port clock
 *
 * The GPIO port clock needs to be enabled before calling this function
 *
 * @param[in] gpio_bank GPIO bank to save
 * @param[out] config Variable for storing the GPIO bank registers
 */
static void save_gpio_bank(GPIO_TypeDef *gpio_bank, gpio_config_t *config)
{
	config->MODER   = READ_REG(gpio_bank->MODER);
	config->OTYPER  = READ_REG(gpio_bank->OTYPER);
	config->OSPEEDR = READ_REG(gpio_bank->OSPEEDR);
	config->PUPDR   = READ_REG(gpio_bank->PUPDR);
	config->AFR[0]  = READ_REG(gpio_bank->AFR[0]);
	config->AFR[1]  = READ_REG(gpio_bank->AFR[1]);
}


/**
 * @brief Restore all registers for a given GPIO bank
 *
 * @param[in] gpio_bank GPIO bank to restore
 * @param[in] config Variable that contains all the saved GPIO bank registers
 */
static void restore_gpio_bank(GPIO_TypeDef *gpio_bank, gpio_config_t *config)
{
	/* Enable GPIO port clock to be able to change the configuration */
	enable_gpio_port_clock(gpio_bank, true);

	WRITE_REG(gpio_bank->MODER, config->MODER);
	WRITE_REG(gpio_bank->OTYPER, config->OTYPER);
	WRITE_REG(gpio_bank->OSPEEDR, config->OSPEEDR);
	WRITE_REG(gpio_bank->PUPDR, config->PUPDR);
	WRITE_REG(gpio_bank->AFR[0], config->AFR[0]);
	WRITE_REG(gpio_bank->AFR[1], config->AFR[1]);

	/* Restore GPIO port clock to what it was prior to suspending */
	enable_gpio_port_clock(gpio_bank, config->RCC_GPIO_CLK_ENABLE);
}


/**
 * @brief Suspend GPIO driver
 *
 * Set all GPIO pins in the lowest power consuming state according to AN4899
 * and disable all the port clocks.
 *
 */
static void gpio_suspend(void)
{
	GPIO_InitTypeDef GPIO_InitStructOff;

	/* Save Clocks */
	saved_gpio_status[0].RCC_GPIO_CLK_ENABLE = get_gpio_port_clock(GPIOA);
	saved_gpio_status[1].RCC_GPIO_CLK_ENABLE = get_gpio_port_clock(GPIOB);
	saved_gpio_status[2].RCC_GPIO_CLK_ENABLE = get_gpio_port_clock(GPIOH);

	/* Enable all GPIO port clocks now when we have saved the status */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	/* Save all GPIO banks */
	save_gpio_bank(GPIOA, &saved_gpio_status[0]);
	save_gpio_bank(GPIOB, &saved_gpio_status[1]);
	save_gpio_bank(GPIOH, &saved_gpio_status[2]);

	/* Set all unused GPIO pins in the lowest power consuming state according to AN4899*/
	GPIO_InitStructOff.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructOff.Pull = GPIO_NOPULL;

	GPIO_InitStructOff.Pin = GPIO_PIN_All;

	/* Leave the following pins on bank A unchanged */
	GPIO_InitStructOff.Pin &= (uint16_t) ~(SPI_SCK_Pin |
	                                       SPI_MISO_Pin |
	                                       SPI_MOSI_Pin);

	HAL_GPIO_Init(GPIOA, &GPIO_InitStructOff);

	GPIO_InitStructOff.Pin = GPIO_PIN_All;

	/* Leave the following pins on bank B unchanged */
	GPIO_InitStructOff.Pin &= (uint16_t) ~(INTERRUPT_Pin |
	                                       ENABLE_Pin |
	                                       MISC_GPIO1_Pin |
	                                       MISC_GPIO0_Pin);

	HAL_GPIO_Init(GPIOB, &GPIO_InitStructOff);

	GPIO_InitStructOff.Pin = GPIO_PIN_All;

	/* Leave the following pins on bank H unchanged */
	GPIO_InitStructOff.Pin &= (uint16_t) ~(I2C_ADDRESS_Pin |
	                                       MISC_GPIO2_Pin);

	HAL_GPIO_Init(GPIOH, &GPIO_InitStructOff);

	/* Disable all GPIO port clocks */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
}


/**
 * @brief Resume GPIO driver
 *
 * @return Status
 */
static void gpio_resume(void)
{
	/* Restore all GPIOS */
	restore_gpio_bank(GPIOA, &saved_gpio_status[0]);
	restore_gpio_bank(GPIOB, &saved_gpio_status[1]);
	restore_gpio_bank(GPIOH, &saved_gpio_status[2]);
}


/**
 * @brief IRQ Handler for RTC Alarm
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc)
{
	(void)rtc;

	rtc_alarm_triggered = true;
	rtc_set_next_wakeup_time();
}


/**
 * @brief IRQ Handler for RTC Wakeup
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *rtc)
{
	(void)rtc;

	rtc_wakeup_triggered = true;
}


static void acc_integration_enable_wake_up(uint32_t time_usec)
{
	uint32_t wakeup_clock;
	uint32_t wakeup_counter;

	rtc_wakeup_triggered = false;

	// Calculate proper clock source and counter values from sleep time
	// RTC_WAKEUPCLOCK_RTCCLK_DIV16 gives 1÷32000×16 = 500  us resolution and max 32s
	// RTC_WAKEUPCLOCK_RTCCLK_DIV8  gives 1÷32000×8  = 250  us resolution and max 16s
	// RTC_WAKEUPCLOCK_RTCCLK_DIV4  gives 1÷32000×4  = 125  us resolution and max 8s
	// RTC_WAKEUPCLOCK_RTCCLK_DIV2  gives 1÷32000×2  = 62.5 us resolution and max 4s
	// RTC_WAKEUPCLOCK_CK_SPRE_16BITS gives            1000 ms resolution and max 18h
	if (time_usec > 30000000)
	{
		wakeup_clock   = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;
		wakeup_counter = (time_usec / 1000000) - 1;
	}
	else if (time_usec > 100000)
	{
		wakeup_clock   = RTC_WAKEUPCLOCK_RTCCLK_DIV16;
		wakeup_counter = (time_usec * 2 / 1000) - 1;
	}
	else
	{
		wakeup_clock   = RTC_WAKEUPCLOCK_RTCCLK_DIV2;
		wakeup_counter = (time_usec * 16 / 1000) - 1;
	}

	if (HAL_RTCEx_SetWakeUpTimer_IT(&MODULE_RTC_HANDLE, wakeup_counter, wakeup_clock) != HAL_OK)
	{
		Error_Handler();
	}
}


static void acc_integration_disable_wake_up(void)
{
	if (HAL_RTCEx_DeactivateWakeUpTimer(&MODULE_RTC_HANDLE) != HAL_OK)
	{
		Error_Handler();
	}
}


void acc_integration_sleep_ms(uint32_t time_msec)
{
	acc_integration_sleep_us(time_msec * 1000);
}


void acc_integration_sleep_us(uint32_t time_usec)
{
	acc_integration_enable_wake_up(time_usec);

	while (!rtc_wakeup_triggered)
	{
		// Turn off interrupts
		disable_interrupts();

		if (!rtc_wakeup_triggered)
		{
			__WFI();
		}

		// Enable interrupt again, the ISR will execute directly after this
		enable_interrupts();
	}

	rtc_wakeup_triggered = false;

	acc_integration_disable_wake_up();
}


void acc_integration_set_periodic_wakeup(uint32_t time_msec)
{
	periodic_sleep_time_ms = time_msec;
	rtc_set_next_wakeup_time();
}


static void acc_integration_prepare_stop_1(void)
{
	HAL_RCC_GetOscConfig(&prepare_saved_rcc_oscinitstruct);
	HAL_RCC_GetClockConfig(&prepare_saved_rcc_clkinitstruct, &prepare_saved_flatency);

	prepare_saved_i2c_state = MODULE_I2C_HANDLE.State;
	HAL_I2C_DeInit(&MODULE_I2C_HANDLE);
	HAL_SPI_DeInit(&A121_SPI_HANDLE);
	prepare_saved_uart1_rx_state = MODULE_UART1_HANDLE.RxState;
	HAL_UART_DeInit(&MODULE_UART1_HANDLE);
	HAL_UART_DeInit(&MODULE_UART2_HANDLE);

	gpio_suspend();

	HAL_SuspendTick();
}


static void acc_integration_resume_stop_1(void)
{
	HAL_ResumeTick();

	HAL_RCC_OscConfig(&prepare_saved_rcc_oscinitstruct);
	HAL_RCC_ClockConfig(&prepare_saved_rcc_clkinitstruct, prepare_saved_flatency);

	gpio_resume();

	HAL_I2C_Init(&MODULE_I2C_HANDLE);
	if (prepare_saved_i2c_state == HAL_I2C_STATE_LISTEN)
	{
		HAL_I2C_EnableListen_IT(&MODULE_I2C_HANDLE);
	}

	HAL_SPI_Init(&A121_SPI_HANDLE);
	HAL_UART_Init(&MODULE_UART1_HANDLE);

	if (prepare_saved_uart1_rx_state == HAL_UART_STATE_BUSY_RX)
	{
		if (MODULE_UART1_HANDLE.hdmarx != NULL)
		{
			HAL_UART_Receive_DMA(&MODULE_UART1_HANDLE, MODULE_UART1_HANDLE.pRxBuffPtr, MODULE_UART1_HANDLE.RxXferSize);
		}
		else
		{
			HAL_UART_Receive_IT(&MODULE_UART1_HANDLE, MODULE_UART1_HANDLE.pRxBuffPtr, MODULE_UART1_HANDLE.RxXferSize);
		}
	}

	HAL_UART_Init(&MODULE_UART2_HANDLE);
}


void acc_integration_sleep_until_periodic_wakeup(void)
{
	// The periodic timer must be set prior to invoking this function
	if (periodic_sleep_time_ms != 0)
	{
		acc_integration_prepare_stop_1();

		uint32_t rtc_ticks_pre_sleep = get_rtc_tick();

		while (!rtc_alarm_triggered)
		{
			// Turn off interrupts
			disable_interrupts();

			if (!rtc_alarm_triggered)
			{
				HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			}

			// Enable interrupt again, the ISR will execute directly after this
			enable_interrupts();
		}

		rtc_alarm_triggered = false;

		update_system_tick(rtc_ticks_pre_sleep, get_rtc_tick());

		acc_integration_resume_stop_1();
	}
	else
	{
		printf("acc_integration_set_periodic_wakeup must be called prior to calling this function\n");
	}
}


uint32_t acc_integration_get_time(void)
{
	return HAL_GetTick();
}


void *acc_integration_mem_alloc(size_t size)
{
	return malloc(size);
}


void *acc_integration_mem_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}


void acc_integration_mem_free(void *ptr)
{
	free(ptr);
}
