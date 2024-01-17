// Copyright (c) Acconeer AB, 2020-2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include "acc_integration.h"


void acc_integration_critical_section_enter(void)
{
	__asm volatile ("cpsid i" ::: "memory");
	__asm volatile ("dsb");
	__asm volatile ("isb");
}


void acc_integration_critical_section_exit(void)
{
	__asm volatile ("cpsie i" ::: "memory");
	__asm volatile ("dsb");
	__asm volatile ("isb");
}
