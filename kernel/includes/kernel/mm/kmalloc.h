#pragma once

#include <stddef.h>

#define MALLOC_MAGIC_KMALLOC      0xDEADBEEF
#define MALLOC_MAGIC_EXPAND_HEAP  0xCAFEBABE
#define MALLOC_MAGIC_FREE         0x12345678
#define MALLOC_MAGIC_ALIGN_SPLIT  0xDEAFF00D

struct block_meta {
	size_t size;
	struct block_meta *next;
	size_t free;
#ifdef MALLOC_USE_MAGIC
	size_t magic; // For debugging only. TODO: remove this in non-debug mode.
#endif
};

void kmalloc_init();

void *kmalloc(size_t size);
void *kmalloc_a(size_t size, size_t alignment);

void kfree(void *ptr);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);