#pragma once

/**
 * Will need to tidy-up once getting ready for user-modes
 */

#include <stddef.h>
#include <kpanic.h>
#include <kprint.h>

#define ASSERT(condition) (void)((condition) || (panic("ASSERTION FAILED: " #condition),0))

#ifdef __KERNEL_CODE
#define panic(msg) kpanic(msg)
#else
#define panic(msg) kprintf("PANIC: %s\n", msg)
#endif