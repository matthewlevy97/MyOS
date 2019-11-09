#include <kernel/kmalloc.h>

/**
 * Most basic kmalloc implementation currently
*/

static char *kmalloc_base;

/**
 * @brief      Initialize kmalloc
 */
void kmalloc_init()
{
	kmalloc_base = (char*)KMALLOC_BASE_ADDRESS;
}

/**
 * @brief      Allocate size bytes in the heap
 *
 * @param[in]  size  The size of the region to allocate
 *
 * @return     Pointer to a region allocated for useage
 */
void *kmalloc(size_t size)
{
	void *ptr;

	ptr = kmalloc_base;
	kmalloc_base += size;

	return ptr;
}