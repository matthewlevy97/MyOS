#pragma once

#include <multiboot/multiboot2.h>
#include <stddef.h>

#define PALLOC_INITIAL_BITMAP_SIZE 256

void palloc_init(uintptr_t low_address);
void palloc_init2(uintptr_t low_address, struct multiboot_tag_mmap *mb_mmap);

uintptr_t palloc_physical();
void palloc_release(uintptr_t address);

void palloc_mark_inuse(uintptr_t address);