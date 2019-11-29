#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <kprint.h>

static uint8_t * const heap_base_address = (uint8_t*)0xC0800000;

/**
 * @brief      Initialize kmalloc
 * 
 * We have a chicken and an egg problem here.
 * We need to create a new page table to hold the information relating to
 * these eternal pages, but cannot as palloc is not setup.
 * Therefore we cannot get a physical page to load.
 */
void kmalloc_init(void * start_physical_address)
{
	/**
	 * Map 4 MiB of pages starting after page of multiboot header as initial heap
	 */
	for(int i = 0; i < 1024; i++) {
		paging_map((void*)start_physical_address + i * PAGE_SIZE,
			heap_base_address + i * PAGE_SIZE,
			PAGE_PRESENT | PAGE_READ_WRITE);
	}
}

void *kmalloc(size_t size)
{
	// TODO: Write an actual function here
	static char *kmalloc_address = heap_base_address;
	void *ptr;

	ptr = kmalloc_address;
	kmalloc_address += size;

	return ptr;
}

/**
 * @brief      Page aligned kmalloc
 *
 * @param[in]  size  The size (in bytes) to allocate
 *
 * @return     Pointer to allocated region, or NULL if failed
 */
void *kmalloc_a(size_t size)
{
	// TODO: Write this function
	return kmalloc(PAGE_SIZE);
}

void kfree(void *ptr)
{
	return;
}

void *kcalloc(size_t nmemb, size_t size)
{
	return kmalloc(nmemb * size);
}

void *krealloc(void *ptr, size_t size)
{
	return NULL;
}