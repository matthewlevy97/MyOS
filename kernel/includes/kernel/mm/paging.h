#pragma once

#include <stddef.h>

#define PAGE_ALIGN(x) ((x) & 0xFFFFF000)

#define KERNEL_ADDRESS_TO_PHYSICAL(x) ((x) - 0xC0000000)

#define PAGE_SIZE 4096
#define PAGE_DIRECTORY_ENTRIES 1024

enum {
	PAGE_PRESENT     = 0x01,
	PAGE_READ_WRITE  = 0x02,
	PAGE_USER_ACCESS = 0x04,
	PAGE_ACCESSED    = 0x08,
	PAGE_DIRTY       = 0x10
};

struct page_directory_entry {
	char present        : 1;
	char read_write     : 1;
	char user_level     : 1;
	char write_through  : 1;
	char cache_disabled : 1;
	char accessed       : 1;
	char _zero          : 1;
	char page_size      : 1;
	char _ignore        : 1;
	char _os_reserved   : 2;
	uint32_t address    : 20;
};

struct page_table_entry {
	char present        : 1;
	char read_write     : 1;
	char user_level     : 1;
	char write_through  : 1;
	char cache_disabled : 1;
	char accessed       : 1;
	char dirty          : 1;
	char _zero          : 1;
	char global         : 1;
	char _os_reserved   : 2;
	uint32_t address    : 20;
};

void paging_init();
uint32_t paging_virtual_to_physical(void *page_directory, void *virtual_address);
void paging_load_physical(void *page_directory, void *physical_address, void *virtual_address, uint32_t flags);