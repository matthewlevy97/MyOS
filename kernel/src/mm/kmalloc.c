#include <kernel/mm/kmalloc.h>
#include <kernel/mm/paging.h>
#include <kernel/kprint.h>

static const char *base_address = (char*)0xC0800000;

static char *eternal_base_address;
static size_t eternal_bytes_left;

static char *kmalloc_base_address;

/**
 * @brief      Initialize kmalloc
 */
void kmalloc_init(size_t eternal_size)
{
	size_t pages_needed;
	pages_needed = eternal_size / PAGE_SIZE + (eternal_size % PAGE_SIZE ? 1 : 0);

	/**
	 * We have a chicken and an egg problem here
	 * We need to create a new page table to hold the information relating to
	 * these eternal pages, but cannot as palloc is not setup.
	 * Therefore we cannot get a physical page to load.
	 */

	eternal_base_address = base_address;
	eternal_bytes_left   = pages_needed * PAGE_SIZE;

	kmalloc_base_address = eternal_base_address + eternal_bytes_left;
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
}