// Copyright (c) Acconeer AB, 2019
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <printf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * This file contains wrapper functions used to replace the default
 * printf and friends implementation with a minimal which is thread safe
 * and memory efficient when used on embedded targets.
 */

#define BUF_SIZE 200


int _write(int file, const char *ptr, int len);


typedef struct
{
	char     buffer[BUF_SIZE];
	uint16_t position;
} print_buffer_t;


static void out_func(char character, void *arg)
{
	print_buffer_t *buf = arg;

	buf->buffer[buf->position++] = character;
	if (buf->position == BUF_SIZE)
	{
		_write(0, buf->buffer, BUF_SIZE);
		buf->position = 0;
	}
}


int __wrap_puts(const char *str)
{
	size_t len = strlen(str);

	_write(0, str, len);
	_write(0, "\n", 1);
	return 0;
}


int __wrap_fputs(const char *str, FILE *stream)
{
	(void)stream;
	size_t len = strlen(str);

	_write(0, str, len);
	_write(0, "\n", 1);
	return 0;
}


int __wrap_printf(const char *format, ...)
{
	print_buffer_t buf = {
		.position = 0,
		.buffer   = {0}
	};
	va_list        va;

	va_start(va, format);
	int ret = fctvprintf(out_func, &buf, format, va);
	if (buf.position != 0)
	{
		_write(0, buf.buffer, buf.position);
	}

	va_end(va);

	return ret;
}


int __wrap_sprintf(char *buffer, const char *format, ...)
{
	va_list va;

	va_start(va, format);
	const int ret = vsnprintf_(buffer, (size_t)-1, format, va);
	va_end(va);
	return ret;
}


int __wrap_snprintf(char *buffer, size_t count, const char *format, ...)
{
	va_list va;

	va_start(va, format);
	const int ret = vsnprintf_(buffer, count, format, va);
	va_end(va);
	return ret;
}


int __wrap_vsnprintf(char *buffer, size_t count, const char *format, va_list va)
{
	return vsnprintf_(buffer, count, format, va);
}
