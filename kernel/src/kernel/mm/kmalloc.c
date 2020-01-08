#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <assert.h>
#include <string.h>
#include <kpanic.h>

/**
 * Simple buddy based memory allocator
 * 
 * Can use some improvement
 * 	- Memory boundary regions to check for overflows
 */

static struct buddy_metadata * const buddy_nodes = (struct buddy_metadata *)0xC0800000;
static uint8_t *heap_base_address, *heap_top_address;

static size_t free_node_from_offset(size_t ptr_offset, size_t current_size, size_t current_offset, size_t current_index);
static size_t find_free_node(size_t desired_size, size_t current_size, size_t index);
static size_t size_round_up(size_t size);

/**
 * @brief      Initialize kmalloc and allocate the first few pages
 */
void kmalloc_init()
{
	size_t buddy_size_paged;

	if(BUDDY_TREE_SIZE >= PAGE_SIZE - 1) {
		buddy_size_paged = PAGE_ALIGN(BUDDY_TREE_SIZE + 1);
	} else {
		buddy_size_paged = PAGE_SIZE;
	}

	// Base address for the heap is right after tree data
	heap_base_address = (uint8_t*)buddy_nodes + buddy_size_paged;

	// Map a full page directory of entries (4 MiB)
	heap_top_address = (uint8_t*)buddy_nodes;
	for(size_t i = 0; i < PAGE_TABLE_ENTRIES; i++) {
		paging_map(heap_top_address, PAGE_PRESENT | PAGE_READ_WRITE);
		heap_top_address += PAGE_SIZE;
	}
}

/**
 * @brief      Allocate memory on the heap. Memory is "size" aligned
 *
 * @param[in]  size  The size of the region to allocate
 *
 * @return     Pointer to allocated region, or NULL on error
 */
void *kmalloc(size_t size)
{
	size_t index, tree_level, block_number, block_size;
	
	// Align to 2^x size or MIN_ALLOCATION_SIZE
	if(size < MIN_ALLOCATION_SIZE)
		size = MIN_ALLOCATION_SIZE;
	else
		size = size_round_up(size);
	
	// Can't return memory larger than max heap size, so don't even try
	if(size > HEAP_MAX_SIZE)
		return NULL;

	index = find_free_node(size, HEAP_MAX_SIZE, 0);
	
	if(index == BUDDY_NODE_NOT_FOUND)
		return NULL;
	
	// Used for determining which block into block_size is being returned
	block_number = index;
	
	// Get level of tree to determine block_size
	tree_level = 0;
	while(index) {
		tree_level++;
		index = (index-1) / 2;
	}
	
	// Determine block_size and block_number of free node
	block_number = block_number - (1 << tree_level) + 1;
	block_size = HEAP_MAX_SIZE >> tree_level;

	return heap_base_address + (block_size * block_number);
}

/**
 * @brief      Allocates memory for an array of nmemb elements of size bytes each. Memory region set to zeros.
 *
 * @param[in]  nmemb  The number of elements
 * @param[in]  size   The size of each element
 *
 * @return     Pointer to allocated region, or NULL on error
 */
void *kcalloc(size_t nmemb, size_t size)
{
	void * ptr;
	size_t combined_size;

	combined_size = nmemb * size;
	if(nmemb != combined_size / size) {
		kpanic("Integer overflow on call to kcalloc!");
	}

	ptr = kmalloc(combined_size);
	if(ptr == NULL) return NULL;

	memset(ptr, 0, combined_size);

	return ptr;
}

// TODO: Create krealloc() function
void *krealloc(void *ptr, size_t size)
{
	return NULL;
}

/**
 * @brief      Frees the memory region pointed to by ptr. Region must have been created by kmalloc, kcalloc, or krealloc.
 *
 * @param      ptr   The pointer to the memory region
 */
void kfree(void * ptr)
{
	size_t heap_offset, node;
	
	// NULL check is implied
	if((uintptr_t)ptr < (uintptr_t)heap_base_address || (uintptr_t)ptr > (uintptr_t)heap_top_address) return;
	
	// Get offset into start of heap region
	heap_offset = ((uintptr_t)ptr - (uintptr_t)heap_base_address);
	node = free_node_from_offset(heap_offset, HEAP_MAX_SIZE, 0, 0);
	
	if(node == BUDDY_NODE_NOT_FOUND) {
		// XXX: Node not found, do we care???
	}
}

static size_t free_node_from_offset(size_t ptr_offset, size_t current_size, size_t current_offset, size_t current_index)
{
	size_t index;
	
	// Return if correct block found
	if(buddy_nodes[current_index].in_use && !buddy_nodes[current_index].split) {
		buddy_nodes[current_index].in_use = 0;
		return current_index;
	}
	
	// Does this block have children?
	current_size /= 2;
	if(current_size < MIN_ALLOCATION_SIZE)
		return BUDDY_NODE_NOT_FOUND;
	
	// Is it in left or right region
	if(ptr_offset < current_offset + current_size) {
		// Left side
		index = free_node_from_offset(ptr_offset, current_size,
			current_offset, BUDDY_LEFT_CHILD(current_index));
		
		// Can we coalesce the left and right blocks?
		if(index != BUDDY_NODE_NOT_FOUND && !buddy_nodes[BUDDY_RIGHT_CHILD(current_index)].in_use) {
			buddy_nodes[current_index].in_use = 0;
			buddy_nodes[current_index].split  = 0;
		}
	} else {
		// Right side
		index = free_node_from_offset(ptr_offset, current_size,
			current_offset + current_size, BUDDY_RIGHT_CHILD(current_index));
		
		// Can we coalesce the left and right blocks?
		if(index != BUDDY_NODE_NOT_FOUND && !buddy_nodes[BUDDY_LEFT_CHILD(current_index)].in_use) {
			buddy_nodes[current_index].in_use = 0;
			buddy_nodes[current_index].split  = 0;
		}
	}
	
	return index;
}

static size_t find_free_node(size_t desired_size, size_t current_size, size_t index)
{
	size_t ret;

	// Prevent looking outside buffer for the tree
	ASSERT(index < BUDDY_TREE_SIZE);
	
	// Block is allocated and not be split
	if(buddy_nodes[index].in_use && !(buddy_nodes[index].split))
		goto not_found;
	
	// Check if current block works
	if(!(buddy_nodes[index].in_use) && desired_size == current_size) {
		buddy_nodes[index].in_use = 1;
		buddy_nodes[index].split  = 0; // XXX: Is this needed?
		ret = index;
		goto successful_find;
	}
	
	// Ensure we haven't gone smaller than requested
	current_size /= 2;
	if(current_size < desired_size || current_size < MIN_ALLOCATION_SIZE)
		goto not_found;
	
	// Recurse left side
	ret = find_free_node(desired_size, current_size, BUDDY_LEFT_CHILD(index));
	if(ret != BUDDY_NODE_NOT_FOUND) {
		buddy_nodes[index].in_use = 1;
		buddy_nodes[index].split = 1;
		goto successful_find;
	}
	
	// Recurse left side
	ret = find_free_node(desired_size, current_size, BUDDY_RIGHT_CHILD(index));
	if(ret != BUDDY_NODE_NOT_FOUND) {
		buddy_nodes[index].in_use = 1;
		buddy_nodes[index].split = 1;
		goto successful_find;
	}

not_found:
	return BUDDY_NODE_NOT_FOUND;
successful_find:
	return ret;
}

static size_t size_round_up(size_t size)
{
	size_t count;
	
	count = 0;
	if(size && !(size & (size-1)))
		return size;
	
	while(size) {
		size >>= 1;
		count += 1;
	}
	
	return 1 << count;
}