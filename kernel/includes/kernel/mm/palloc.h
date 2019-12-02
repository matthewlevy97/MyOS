#pragma once

#include <multiboot/multiboot2.h>
#include <stddef.h>

#define PALLOC_INITIAL_BITMAP_SIZE 256

void palloc_init(uint32_t low_address);
void palloc_init2(uint32_t low_address, struct multiboot_tag_mmap *mb_mmap);
uint32_t palloc_physical();
void palloc_release(uint32_t address);