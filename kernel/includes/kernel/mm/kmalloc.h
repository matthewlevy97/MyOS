#pragma once

#include <i686/isr.h>
#include <stddef.h>

////////////////////////////////////////////////////////////////

#define HEAP_SIZE_EXPONENT 22			// Heap Size           == 4 MiB
#define MIN_ALLOCATION_SIZE_EXPONENT 4	// Min Allocation Size == 16 bytes

////////////////////////////////////////////////////////////////

#define HEAP_MAX_SIZE       (1 << HEAP_SIZE_EXPONENT)
#define BUDDY_TREE_SIZE   ((1 << (HEAP_SIZE_EXPONENT - (MIN_ALLOCATION_SIZE_EXPONENT - 1))) - 1)

#define MIN_ALLOCATION_SIZE (1 << MIN_ALLOCATION_SIZE_EXPONENT)

#define BUDDY_LEFT_CHILD(index)  (index*2 + 1)
#define BUDDY_RIGHT_CHILD(index) (index*2 + 2)

#define BUDDY_NODE_NOT_FOUND ((uint32_t)-1)

struct buddy_metadata {
	char in_use : 1;
	char split  : 1;
};

void kmalloc_init();

void *kmalloc(size_t size);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);

void kfree(void *ptr);