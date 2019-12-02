#pragma once

#include <stddef.h>

#define KMALLOC_BASE_ADDRESS (4 * MB)

void kmalloc_init();

void *kmalloc(size_t size);
void *kmalloc_a(size_t size);

void kfree(void *ptr);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);