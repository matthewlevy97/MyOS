#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <mm/palloc.h>
#include <string.h>

static uint8_t * const heap_base_address = (uint8_t*)0xC0800000;
static uint8_t *       heap_top_address;

static void *kmalloc_implementation(size_t size, uint8_t aligned);

/**
 * @brief      Initialize kmalloc and allocated the first few pages
 */
void kmalloc_init()
{
	uint32_t heap_page_number;

	/**
	 * Map 4 MiB of pages starting after page of multiboot header as initial heap
	 */
	for(heap_page_number = 0; heap_page_number < 3; heap_page_number++) {
		paging_map((void*)palloc_physical(),
			heap_base_address + heap_page_number * PAGE_SIZE,
			PAGE_PRESENT | PAGE_READ_WRITE);
	}
	
	heap_top_address = heap_base_address + heap_page_number * PAGE_SIZE;
}

/**
 * @brief      Allocated memory in the kernel heap
 *
 * @param[in]  size  The size of the region to allocate
 *
 * @return     Pointer to a region of the size to allocate, or NULL if it failed
 */
inline void *kmalloc(size_t size)
{
	return kmalloc_implementation(size, 0);
}

/**
 * @brief      Page aligned kmalloc
 *
 * @param[in]  size  The size (in bytes) to allocate
 *
 * @return     Pointer to allocated region, or NULL if failed
 */
inline void *kmalloc_a(size_t size)
{
	return kmalloc_implementation(size, 1);
}

void kfree(void *ptr)
{
	return;
}

void *kcalloc(size_t nmemb, size_t size)
{
	void * ptr;

	ptr = kmalloc(nmemb * size);
	memset(ptr, 0, nmemb * size);

	return ptr;
}

void *krealloc(void *ptr, size_t size)
{
	return NULL;
}

static void *kmalloc_implementation(size_t size, uint8_t aligned)
{
	// TODO: Write an actual function here
	static uint8_t *kmalloc_address = heap_base_address;
	void *ptr;

	// XXX: Super memory inefficent
	if(aligned)
		kmalloc_address = (void*)(PAGE_ALIGN((uint32_t)kmalloc_address) + PAGE_SIZE);

	if(kmalloc_address >= heap_top_address) {
		return NULL;
	}

	ptr = kmalloc_address;
	kmalloc_address += size;

	return ptr;
}