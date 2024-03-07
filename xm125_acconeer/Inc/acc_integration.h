// Copyright (c) Acconeer AB, 2019-2023
// All rights reserved

#ifndef ACC_INTEGRATION_H_
#define ACC_INTEGRATION_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


typedef void (*acc_integration_uart_read_func_t)(uint8_t data, uint32_t status);


/**
 * @brief Sleep for a specified number of microseconds
 *
 * @param time_usec Time in microseconds to sleep
 */
void acc_integration_sleep_us(uint32_t time_usec);


/**
 * @brief Sleep for a specified number of milliseconds
 *
 * @param time_msec Time in milliseconds to sleep
 */
void acc_integration_sleep_ms(uint32_t time_msec);


/**
 * @brief Set up a periodic timer used to wake up the system from sleep
 *
 * This function will start a periodic timer with the specified time.
 * This is useful when the drift of the wakeup interval should be kept
 * at a minimum.
 *
 * If the time_msec is set to zero the periodic wakeup will be disabled.
 *
 * @param time_msec Time in milliseconds
 */
void acc_integration_set_periodic_wakeup(uint32_t time_msec);


/**
 * @brief Put the system in sleep until the periodic timer triggers
 *
 * The periodic timer must be started using
 * @ref acc_integration_set_periodic_wakeup prior to
 * invoking this function.
 * The target specific implementation of this function will determine
 * the sleep depth based on the set sleep interval and it will be a
 * trade-off between wake-up latency and power consumption.
 */
void acc_integration_sleep_until_periodic_wakeup(void);


/**
 * @brief Allocate dynamic memory
 *
 * @param[in]  size The bytesize of the reuested memory block
 * @return Returns either NULL or a unique pointer to a memory block
 */
void *acc_integration_mem_alloc(size_t size);


/**
 * @brief Allocate dynamic memory
 *
 * Allocate an array of nmemb elements of size bytes each.
 *
 * @param[in]  nmemb The number of elements in the array
 * @param[in]  size The bytesize of the element
 * @return Returns either NULL or a unique pointer to a memory block
 */
void *acc_integration_mem_calloc(size_t nmemb, size_t size);


/**
 * @brief Free dynamic memory
 *
 * @param[in]  ptr A pointer to the memory space to be freed
 */
void acc_integration_mem_free(void *ptr);


/**
 * Enter a critical section
 */
void acc_integration_critical_section_enter(void);


/**
 * Exit a critical section
 */
void acc_integration_critical_section_exit(void);


/**
 * @brief Get current time
 *
 * It is important that this value wraps correctly and uses all bits. I.e. it should count
 * upwards to 2^32 - 1 and then 0 again.
 *
 * @returns Current time as milliseconds
 */
uint32_t acc_integration_get_time(void);


#endif
