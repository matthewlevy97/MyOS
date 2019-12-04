#include <mm/kmalloc.h>
#include <mm/palloc.h>
#include <mm/paging.h>
#include <structures/bitmap.h>

#include <string.h>
#include <kpanic.h>
#include <kprint.h>

static uint32_t initial_palloc_bitmap[PALLOC_INITIAL_BITMAP_SIZE];

static uint32_t number_mmap_entries;
static struct multiboot_mmap_entry *mmap_entries;

static void * allocated_pages_bitmap;
static uint32_t bitmap_size;

static uint32_t base_address;

/**
 * @brief      Stage 1 initialization, sets-up palloc w/ a small number of pages
 *
 * @param[in]  low_address  The base address to start palloc from
 */
void palloc_init(uint32_t low_address)
{
	// Zero bitmap
	memset(initial_palloc_bitmap, 0, sizeof(initial_palloc_bitmap));

	// Clear out mmap values
	number_mmap_entries = 0;
	mmap_entries = NULL;

	// Set values to match initial size
	allocated_pages_bitmap = initial_palloc_bitmap;
	bitmap_size = sizeof(initial_palloc_bitmap);

	// Set base address.
	base_address = PAGE_ALIGN(low_address) + PAGE_SIZE;
}

/**
 * @brief      Stage 2 initialization, requires kmalloc to be setup first
 *
 * @param[in]  low_address  The base address to start palloc from
 * @param      mb_mmap      The memory map from multiboot header mmap
 */
void palloc_init2(uint32_t low_address, struct multiboot_tag_mmap *mb_mmap)
{
	struct multiboot_mmap_entry mmap_entry;

	bitmap_size = 0;

	number_mmap_entries = mb_mmap->size - ((uint32_t)&mb_mmap->entries - (uint32_t)&mb_mmap->type);
	number_mmap_entries /= mb_mmap->entry_size;

	for(uint32_t i = 0; i < number_mmap_entries; i++) {
		mmap_entry = mb_mmap->entries[i];
		
		/**
		 * Only return an address if:
		 * 	1) It is un-used/valid RAM
		 * 	2) Starts after the kernel
		 */
		if(mmap_entry.type == MULTIBOOT_MEMORY_AVAILABLE && mmap_entry.addr) {
			if(mmap_entry.addr <= low_address)
				bitmap_size += (PAGE_ALIGN(low_address) + mmap_entry.len) / PAGE_SIZE;
			else
				bitmap_size += (mmap_entry.addr + mmap_entry.len) / PAGE_SIZE;
			break;
		}
	}

	base_address = PAGE_ALIGN(low_address) + PAGE_SIZE;
	allocated_pages_bitmap = bitmap_create(bitmap_size);
	bitmap_copy(allocated_pages_bitmap, initial_palloc_bitmap, bitmap_size);

	mmap_entries = kmalloc(sizeof(struct multiboot_mmap_entry) * number_mmap_entries);
	if(mmap_entries == NULL) {
		kprintf(KPRINT_ERROR "Could not allocate space for memory map\n");
		kpanic();
	}

	memcpy(mmap_entries, mb_mmap->entries, sizeof(struct multiboot_mmap_entry) * number_mmap_entries);
}

/**
 * @brief      Returns the physical address of an un-used page
 *
 * @return     Physical address of valid, unused page
 */
uint32_t palloc_physical()
{
	uint32_t index, address;
	
	index = bitmap_get_first_clear(allocated_pages_bitmap, bitmap_size);
	if(index >= bitmap_size) {
		// Could not find any clear bits in the bitmap
		goto fail;
	}

	bitmap_set(allocated_pages_bitmap, bitmap_size, index);

	address = index * PAGE_SIZE;

	/**
	 * If no entries are in the memory map, we can just add the base
	 * 	address and return that
	 */
	if(mmap_entries == NULL) {
		address += base_address;
		goto success;
	}

	for(uint32_t i = 0; i < number_mmap_entries; i++) {
		if(mmap_entries[i].type != MULTIBOOT_MEMORY_AVAILABLE || mmap_entries[i].addr + mmap_entries[i].len < base_address)
			continue;

		if(index > mmap_entries[i].len / PAGE_SIZE) {
			index -= mmap_entries[i].len / PAGE_SIZE;
		} else {
			address += mmap_entries[i].addr;
			goto success;
		}
	}

fail:
	return 0;
success:
	return address;
}

/**
 * @brief      Release possession of a physical page
 *
 * @param[in]  address  The physical page address
 * 
 * Reverses the algorithm of palloc_physical()
 */
void palloc_release(uint32_t address)
{
	uint32_t index;

	index = 0;
	for(uint32_t i = 0; i < number_mmap_entries; i++) {
		if(mmap_entries[i].type != MULTIBOOT_MEMORY_AVAILABLE || mmap_entries[i].addr + mmap_entries[i].len < base_address)
			continue;

		if(index > mmap_entries[i].len / PAGE_SIZE) {
			index += mmap_entries[i].len / PAGE_SIZE;
		} else {
			address -= mmap_entries[i].addr;
			break;
		}
	}
	index += address / PAGE_SIZE;
	
	bitmap_clear(allocated_pages_bitmap, bitmap_size, index);
}