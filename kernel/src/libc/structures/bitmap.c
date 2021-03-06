#include <structures/bitmap.h>
#include <string.h>

/**
 * Bitmap stores entries as arrays of integers.
 * Bits are inserted in BIG ENDIAN format.
 */

#ifdef __KERNEL_CODE
#include <kprint.h>
#include <mm/kmalloc.h>
#define calloc(x, y) kcalloc(x, y)
#else
#include <stdlib.h>
#endif

static inline size_t bitmap_determine_size(size_t size)
{
	size_t alloc_size;

	alloc_size = size / BITMAP_BITS_PER_INT;

	if(size & BITMAP_BITS_PER_INT)
		alloc_size++;

	return alloc_size;
}

void *bitmap_create(size_t size)
{
	size = bitmap_determine_size(size);

	return calloc(size, sizeof(int));
}

void *bitmap_copy(void *dst, void *src, size_t dst_size)
{
	dst_size = bitmap_determine_size(dst_size);
	memcpy(dst, src, dst_size);

	return dst;
}

char bitmap_get(void *bitmap, size_t size, size_t index)
{
	int *map;
	size_t element, bit;

	size = bitmap_determine_size(size);

	map = bitmap;
	element = index / BITMAP_BITS_PER_INT;
	bit = BITMAP_BITS_PER_INT - (index & BITMAP_BITS_PER_INT);

	if(element >= size) {
		return -1;
	}

	return (map[element] >> bit) & 0x1;
}

size_t bitmap_get_first_clear(void *bitmap, size_t size)
{
	int *map, bit;
	size_t element;

	size = bitmap_determine_size(size);

	map = bitmap;
	for(element = 0; element < size && map[element] == -1; element++);

	// Quick check to see if we found something or not
	if(element >= size) goto fail;
	
	// There is an empty bit in this entry
	for(bit = BITMAP_BITS_PER_INT; bit >= 0; bit--) {
		if(((map[element] >> bit) & 0x1) == 0) {
			return (element * (BITMAP_BITS_PER_INT+1)) + (BITMAP_BITS_PER_INT - bit);
		}
	}
	
fail:
	return -1;
}

void *bitmap_set(void *bitmap, size_t size, size_t index)
{
	int *map;
	size_t element, bit;

	size = bitmap_determine_size(size);

	map = bitmap;
	element = index / (BITMAP_BITS_PER_INT+1);
	bit = BITMAP_BITS_PER_INT - (index & BITMAP_BITS_PER_INT);
	
	if(element >= size) {
		return NULL;
	}

	map[element] |= (1 << bit);

	return map;
}

void *bitmap_clear(void *bitmap, size_t size, size_t index)
{
	int *map;
	size_t element, bit;

	size = bitmap_determine_size(size);

	map = bitmap;
	element = index / BITMAP_BITS_PER_INT;
	bit = BITMAP_BITS_PER_INT - (index & BITMAP_BITS_PER_INT);

	if(element >= size) {
		return NULL;
	}

	map[element] &= ~(1 << bit);

	return map;
}