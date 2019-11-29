#pragma once

#define BITMAP_BITS_PER_INT (sizeof(int) * 8 - 1)

#include <stddef.h>

void *bitmap_init(size_t size);
char bitmap_get(void *bitmap, size_t index);
void *bitmap_set(void *bitmap, size_t index);
void *bitmap_clear(void *bitmap, size_t index);