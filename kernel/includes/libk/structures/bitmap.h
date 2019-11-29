#pragma once

#define BITMAP_BITS_PER_INT (sizeof(int) * 8 - 1)

#include <stddef.h>

void *bitmap_create(size_t size);

char bitmap_get(void *bitmap, size_t size, size_t index);
size_t bitmap_get_first_clear(void *bitmap, size_t size);

void *bitmap_set(void *bitmap, size_t size, size_t index);
void *bitmap_clear(void *bitmap, size_t size, size_t index);