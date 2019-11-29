#include <mm/palloc.h>
#include <mm/paging.h>
#include <structures/bitmap.h>

static void * allocated_pages_bitmap;
static uint32_t bitmap_size;

static uint32_t base_address;

void palloc_init(uint32_t low_address, uint32_t high_address)
{
	base_address = PAGE_ALIGN(low_address);
	bitmap_size  = (high_address - base_address) / PAGE_SIZE + 1;
	allocated_pages_bitmap = bitmap_create(bitmap_size);
}

uint32_t palloc_physical()
{
	uint32_t index;

	index = bitmap_get_first_clear(allocated_pages_bitmap, bitmap_size);
	if(index >= bitmap_size) {
		// Could not find any clear bits in the bitmap
		return 0;
	}

	bitmap_set(allocated_pages_bitmap, bitmap_size, index);

	return index * PAGE_SIZE + base_address;
}

void palloc_release(uint32_t address)
{
	uint32_t index;

	index = (address - base_address) / PAGE_SIZE;
	bitmap_clear(allocated_pages_bitmap, bitmap_size, index);
}