#include <structures/bitmap.h>

#ifdef __KERNEL_CODE
#include <mm/kmalloc.h>
#define calloc(x, y) kcalloc(x, y)
#else
#include <stdlib.h>
#endif

void *bitmap_init(size_t size)
{
	if(size & (sizeof(int) - 1))
		size++;
	
	return calloc(size, sizeof(int));
}

char bitmap_get(void *bitmap, size_t index)
{
	int *map;
	size_t element, bit;

	map = bitmap;
	element = index / BITMAP_BITS_PER_INT;
	bit = BITMAP_BITS_PER_INT - (index & BITMAP_BITS_PER_INT);

	return (map[element] >> bit) & 0x1;
}

void *bitmap_set(void *bitmap, size_t index)
{
	int *map;
	size_t element, bit;

	map = bitmap;
	element = index / BITMAP_BITS_PER_INT;
	bit = BITMAP_BITS_PER_INT - (index & BITMAP_BITS_PER_INT);
	
	map[element] |= (1 << bit);

	return map;
}

void *bitmap_clear(void *bitmap, size_t index)
{
	int *map;
	size_t element, bit;

	map = bitmap;
	element = index / BITMAP_BITS_PER_INT;
	bit = BITMAP_BITS_PER_INT - (index & BITMAP_BITS_PER_INT);

	map[element] &= ~(1 << bit);

	return map;
}