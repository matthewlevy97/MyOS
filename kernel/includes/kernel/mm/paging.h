#pragma once

#include <stddef.h>

#define PAGE_ALIGN(x) ((x) & 0xFFFFF000)

#define KERNEL_ADDRESS_TO_PHYSICAL(x) ((x) - 0xC0000000)

#define PAGE_SIZE 4096
#define PAGE_DIRECTORY_ENTRIES 1024

#define REFLECTED_PAGE_TABLE_BASE_ADDRESS 0xFFC00000

enum page_entry_flags {
	PAGE_PRESENT        = 0x01,
	PAGE_READ_WRITE     = 0x02,
	PAGE_USER_ACCESS    = 0x04,
	PAGE_WRITE_THROUGH  = 0x08,
	PAGE_CACHE_DISABLED = 0x10,
	PAGE_ACCESSED       = 0x20,
	PAGE_DIRTY          = 0x40,
	PAGE_SIZE_4M        = 0x80,
	PAGE_GLOBAL         = 0x100,
};

enum page_mapping_flags {
	MAPPING_WIPE_PAGE  = 0x01,
};

void paging_init();
void * paging_virtual_to_physical(void *virtual_address);

void paging_map(void *virtual_address, uint32_t flags);
void paging_map2(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags);

void paging_unmap(void *virtual_address);

void paging_switch_directory(uint32_t * page_dir, uint32_t phys);

void page_fault_handler(struct isr_arguments *args);