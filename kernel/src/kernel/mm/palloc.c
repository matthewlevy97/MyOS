#include <mm/kmalloc.h>
#include <mm/palloc.h>
#include <mm/paging.h>
#include <structures/bitmap.h>

#include <string.h>
#include <kpanic.h>

static uint32_t initial_palloc_bitmap[PALLOC_INITIAL_BITMAP_SIZE];

static uint32_t number_mmap_entries;
static struct multiboot_mmap_entry *mmap_entries;

static void * allocated_pages_bitmap;
static uint32_t bitmap_size;

static uintptr_t base_address;

static uint32_t find_index_by_address(uintptr_t address);

/**
 * @brief      Stage 1 initialization, sets-up palloc w/ a small number of pages
 *
 * @param[in]  low_address  The base address to start palloc from
 */
void palloc_init(uintptr_t low_address)
{
	// Zero bitmap
	memset(initial_palloc_bitmap, 0, sizeof(initial_palloc_bitmap));

	// Clear out mmap values
	number_mmap_entries = 0;
	mmap_entries = NULL;

	// Set values to match initial size
	allocated_pages_bitmap = initial_palloc_bitmap;
	bitmap_size = sizeof(initial_palloc_bitmap);

	// Set base address. Will not change between init stage 1 and stage 2.
	base_address = PAGE_ALIGN(low_address) + PAGE_SIZE;
}

/**
 * @brief      Stage 2 initialization, requires kmalloc to be setup first
 *
 * @param[in]  low_address  The base address to start palloc from
 * @param      mb_mmap      The memory map from multiboot header mmap
 */
void palloc_init2(uintptr_t low_address, struct multiboot_tag_mmap *mb_mmap)
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
		}
	}

	allocated_pages_bitmap = bitmap_create(bitmap_size);
	memcpy(allocated_pages_bitmap, initial_palloc_bitmap, sizeof(initial_palloc_bitmap));

	mmap_entries = kmalloc(sizeof(struct multiboot_mmap_entry) * number_mmap_entries);
	if(mmap_entries == NULL) {
		kpanic("Could not allocate space for memory map");
	}
	memcpy(mmap_entries, mb_mmap->entries, sizeof(struct multiboot_mmap_entry) * number_mmap_entries);
}

/**
 * @brief      Returns the physical address of an un-used page
 *
 * @return     Physical address of valid, unused page. NULL on failure.
 */
uintptr_t palloc_physical()
{
	uint32_t index;
	uintptr_t address;

	index = bitmap_get_first_clear(allocated_pages_bitmap, bitmap_size);
	if(index >= bitmap_size) {
		// Could not find any clear bits in the bitmap
		goto fail;
	}

	// Mark page as allocated
	bitmap_set(allocated_pages_bitmap, bitmap_size, index);

	// XXX: Should be end of thread un-safe region

	// Just need to find the base for this offset
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
			// Not in this region, subtract region pages from index
			index -= mmap_entries[i].len / PAGE_SIZE;
		} else if(mmap_entries[i].addr < base_address) {
			// To prevent using addresses below base_address
			address += base_address;
			goto success;
		} else {
			// Use address is above base_address
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
void palloc_release(uintptr_t address)
{
	uint32_t index;
	index = find_index_by_address(address);
	if(index >= bitmap_size) {
		return;
	}
	
	bitmap_clear(allocated_pages_bitmap, bitmap_size, index);
}

/**
 * @brief      Mark a page as in use / possession gained not through palloc_physical()
 *
 * @param[in]  address  The physical page address
 */
void palloc_mark_inuse(uintptr_t address)
{
	uint32_t index;
	index = find_index_by_address(address);
	if(index >= bitmap_size) {
		return;
	}
	
	bitmap_set(allocated_pages_bitmap, bitmap_size, index);
}

static uint32_t find_index_by_address(uintptr_t address)
{
	uint32_t index;

	index = 0;

	if(number_mmap_entries == 0) {
		index = (address - base_address) / PAGE_SIZE;
		goto return_index;
	}

	// TODO: Verify this works
	for(uint32_t i = 0; i < number_mmap_entries; i++) {
		if(mmap_entries[i].type != MULTIBOOT_MEMORY_AVAILABLE || mmap_entries[i].addr + mmap_entries[i].len < base_address)
			continue;

		if(address >= mmap_entries[i].addr + mmap_entries[i].len) {
			index += mmap_entries[i].len / PAGE_SIZE;
		} else {
			index += (address - mmap_entries[i].addr) / PAGE_SIZE;
			break;
		}
	}

return_index:
	return index;
}