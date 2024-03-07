// Copyright (c) Acconeer AB, 2019-2024
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "acc_definitions_common.h"
#include "acc_integration.h"
#include "acc_integration_log.h"

#define LOG_BUFFER_MAX_SIZE 150

#define LOG_FORMAT "%02u:%02u:%02u.%03u (%c) (%s) %s\n"


void acc_integration_log(acc_log_level_t level, const char *module, const char *format, ...)
{
	char    log_buffer[LOG_BUFFER_MAX_SIZE];
	va_list ap;

	va_start(ap, format);

	int ret = vsnprintf(log_buffer, LOG_BUFFER_MAX_SIZE, format, ap);

	if (ret >= LOG_BUFFER_MAX_SIZE)
	{
		log_buffer[LOG_BUFFER_MAX_SIZE - 4] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 3] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 2] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 1] = 0;
	}

	uint32_t time_ms = acc_integration_get_time();
	char     level_ch;

	unsigned int timestamp    = time_ms;
	unsigned int hours        = timestamp / 1000 / 60 / 60;
	unsigned int minutes      = timestamp / 1000 / 60 % 60;
	unsigned int seconds      = timestamp / 1000 % 60;
	unsigned int milliseconds = timestamp % 1000;

	level_ch = (level <= ACC_LOG_LEVEL_DEBUG) ? "EWIVD"[level] : '?';

	printf(LOG_FORMAT, hours, minutes, seconds, milliseconds, level_ch, module, log_buffer);

	va_end(ap);
}
