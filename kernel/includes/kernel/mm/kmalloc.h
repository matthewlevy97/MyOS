#pragma once

#include <stddef.h>

#define KMALLOC_BASE_ADDRESS (4 * MB)

void kmalloc_init(size_t eternal_size);
void *kmalloc_eternal(size_t size);
void *kmalloc(size_t size);