#pragma once

#include <stddef.h>

#define KMALLOC_BASE_ADDRESS (4 * MB)

void kmalloc_init();
void *kmalloc(size_t size);