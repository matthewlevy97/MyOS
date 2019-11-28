#pragma once

#include <stddef.h>

#define KMALLOC_BASE_ADDRESS (4 * MB)

void kmalloc_init(void * start_physical_address);
void *kmalloc_eternal(size_t size);

void *kmalloc(size_t size);
void *kmalloc_a(size_t size);