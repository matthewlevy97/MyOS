#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <kprint.h>

static uint8_t *base_address = (uint8_t*)0xC0800000;
static const uint32_t eternal_size = 4 * MB;

static uint8_t *eternal_base_address;
static uint32_t eternal_bytes_left;

static uint8_t *kmalloc_base_address;

/**
 * @brief      Initialize kmalloc
 */
void kmalloc_init(void * start_physical_address)
{
	uint32_t pages_needed;
	pages_needed = eternal_size / PAGE_SIZE + (eternal_size % PAGE_SIZE ? 1 : 0);

	/**
	 * We have a chicken and an egg problem here
	 * We need to create a new page table to hold the information relating to
	 * these eternal pages, but cannot as palloc is not setup.
	 * Therefore we cannot get a physical page to load.
	 */

	eternal_base_address = (uint8_t*)base_address;
	eternal_bytes_left   = pages_needed * PAGE_SIZE;

	kmalloc_base_address = eternal_base_address + eternal_bytes_left;

	/**
	 * Map 4 MiB of pages starting after page of multiboot header as initial heap
	 */
	for(int i = 0; i < 1024; i++) {
		paging_map((void*)start_physical_address + i * PAGE_SIZE,
			base_address + i * PAGE_SIZE,
			PAGE_PRESENT | PAGE_READ_WRITE);
	}
}

void *kmalloc_eternal(size_t size)
{
	void *ptr;

	if(size > eternal_bytes_left) {
		kprintf(KPRINT_ERROR "Cannot kmalloc eternal memory. Out of space.\n");
		return NULL;
	}

	ptr = eternal_base_address;
	eternal_base_address += size;

	return ptr;
}

void *kmalloc(size_t size)
{
	// TODO: Write this function
	return NULL;
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
	return kmalloc_eternal(PAGE_SIZE);
}