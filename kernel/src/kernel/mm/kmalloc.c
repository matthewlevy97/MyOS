#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <mm/palloc.h>
#include <assert.h>
#include <string.h>
#include <kpanic.h>
#include <kprint.h>

static uint8_t * const heap_base_address = (uint8_t*)0xC0800000;
static uint8_t *       heap_top_address;

static void *kmalloc_implementation(size_t size, size_t alignment);
static struct block_meta *find_free_block(struct block_meta **last, size_t size, size_t alignment);
static struct block_meta *expand_heap(struct block_meta* last, size_t size, size_t alignment);

static inline size_t can_split_align(struct block_meta *block, size_t size, size_t alignment);
static inline struct block_meta *ptr_to_block(void *ptr);
static inline void map_heap_page();

/**
 * @brief      Initialize kmalloc and allocated the first few pages
 */
void kmalloc_init()
{
	heap_top_address = heap_base_address;
	for(int i = 0; i < PAGE_DIRECTORY_ENTRIES; i++) {
		map_heap_page();
	}
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
	return kmalloc_implementation(size, sizeof(int));
}

/**
 * @brief      Aligned kmalloc
 *
 * @param[in]  size  The size (in bytes) to allocate
 * @param[in]  size  The byte count to allign on (ex. PAGE_SIZE)
 *
 * @return     Pointer to allocated region, or NULL if failed
 */
inline void *kmalloc_a(size_t size, size_t alignment)
{
	return kmalloc_implementation(size, alignment);
}

void kfree(void *ptr)
{
	struct block_meta *block_ptr;

	if (!ptr)
		return;

	// TODO: consider merging blocks once splitting blocks is implemented.
	block_ptr = ptr_to_block(ptr);
	ASSERT(block_ptr->free == 0);
#ifdef MALLOC_USE_MAGIC
	ASSERT(block_ptr->magic == MALLOC_MAGIC_KMALLOC ||
		block_ptr->magic == MALLOC_MAGIC_EXPAND_HEAP ||
		block_ptr->magic == MALLOC_MAGIC_ALIGN_SPLIT);
	block_ptr->magic = MALLOC_MAGIC_FREE;
#endif
	block_ptr->free = 1;
}

void *kcalloc(size_t nmemb, size_t size)
{
	void *ptr;
	
	// Overflow check
	ASSERT(nmemb * size >= nmemb);

	ptr = kmalloc(nmemb * size);
	memset(ptr, 0, nmemb * size);

	return ptr;
}

void *krealloc(void *ptr, size_t size)
{
	struct block_meta *block_ptr;
	void *new_ptr;

	if(!ptr)
		return kmalloc(size);

	block_ptr = ptr_to_block(ptr);
	if (block_ptr->size >= size) {
		// We have enough space. Could free some once we implement split.
		return ptr;
	}

	// Need to really realloc. Malloc new space and free old space.
	// Then copy old data to new space.
	new_ptr = kmalloc(size);
	if (!new_ptr) {
		return NULL;
	}

	memcpy(new_ptr, ptr, block_ptr->size);
	kfree(ptr);

	return new_ptr;
}

static void *kmalloc_implementation(size_t size, size_t alignment)
{
	struct block_meta *block, *last;

	// Align to sizeof(int) boundary
	if(size & (sizeof(int) - 1))
		size += sizeof(int) - (size & (sizeof(int) - 1));

	last = (struct block_meta*)heap_base_address;
	block = find_free_block(&last, size, alignment);
	if (!block) {
		block = expand_heap(last, size, alignment);
		if (!block) {
			return NULL;
		}
	} else {
		ASSERT(block->size >= size);
		if(block->size - size >= sizeof(struct block_meta)) {
			// Create split upper block (no point creating a new variable / name re-use)
			last = (struct block_meta*)((uintptr_t)block + block->size - size);
			last->free = 1;
			last->next = block->next;
			last->size = block->size - size - sizeof(struct block_meta);
			ASSERT(last->size < block->size);
#ifdef MALLOC_USE_MAGIC
			last->magic = MALLOC_MAGIC_KMALLOC;
#endif

			block->size = size;
			block->next = last;
		}
		block->free = 0;
#ifdef MALLOC_USE_MAGIC
		block->magic = MALLOC_MAGIC_KMALLOC;
#endif
	}

	return ++block;
}

// NOTE: Not thread safe
static struct block_meta *find_free_block(struct block_meta **last, size_t size, size_t alignment)
{
	struct block_meta *current;

	current = *last;

	while (current && !(current->free && current->size >= size && can_split_align(current, size, alignment))) {
		*last = current;
		current = current->next;
	}

	return current;
}

static struct block_meta *expand_heap(struct block_meta* last, size_t size, size_t alignment)
{
	struct block_meta *block, *block2;
	uintptr_t bytes_until_alignment;

	// Move to end of last block
	block = (struct block_meta*)((uintptr_t)last + last->size + sizeof(struct block_meta));

	// Append block to previous
	last->next = block;

	if((uintptr_t)block + size >= (uintptr_t)heap_top_address) {
		kprintf(KPRINT_ERROR "Out of heap memory\n");
		kpanic();
	}
	
	// Is this block aligned correctly?
	if(((uintptr_t)block & (alignment - 1)) == 0x00) {
		block->size = size;
		block->next = NULL;
		block->free = 0;
#ifdef MALLOC_USE_MAGIC
		block->magic = MALLOC_MAGIC_EXPAND_HEAP;
#endif
	} else {
		// We need to create 2 blocks.....
		// How many bytes until next alignment
		bytes_until_alignment = alignment - ((uintptr_t)block & (alignment - 1));
		
		// Go to next alignment if not enough space
		if(bytes_until_alignment < sizeof(struct block_meta))
			bytes_until_alignment += alignment;

		block2 = (struct block_meta*)((uintptr_t)block + bytes_until_alignment - sizeof(struct block_meta));
		block2->size = size;
		block2->next = NULL;
#ifdef MALLOC_USE_MAGIC
		block2->magic = MALLOC_MAGIC_EXPAND_HEAP;
#endif
		
		block->size = (uintptr_t)block2 - (uintptr_t)block - sizeof(struct block_meta);
		block->next = block2;
		block->free = 1;
#ifdef MALLOC_USE_MAGIC
		block->magic = MALLOC_MAGIC_EXPAND_HEAP;
#endif

		block = block2;
	}

	return block;
}

static inline size_t can_split_align(struct block_meta *block, size_t size, size_t alignment)
{
	uintptr_t block_int;
	block_int = (uintptr_t)block;
	
	return ((block_int + sizeof(struct block_meta)) & (alignment - 1)) == 0 &&
		block->size >= size;
}

static inline struct block_meta *ptr_to_block(void *ptr)
{
	return (struct block_meta*)ptr - 1;
}

static inline void map_heap_page()
{
	paging_map((void*)palloc_physical(),
		heap_top_address,
		PAGE_PRESENT | PAGE_READ_WRITE);

	heap_top_address += PAGE_SIZE;
}