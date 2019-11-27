#include <kernel/mm/paging.h>
#include <kernel/kprint.h>

/**
 * Note:
 * 	page_directory -> physical address of table table
 * 	page table     -> physical address of mapped page
 * 	
 * 	page_directory recursively mapped to 0xFFFFF000
*/

static struct page_directory_entry *paging_directory = (struct page_directory_entry*)0xFFFFF000;

void paging_init()
{
	kprintf("0x%x\n", paging_directory);
	kprintf("0x%x\n", paging_directory[768].address << 11);
}

uint32_t paging_virtual_to_physical(void *page_directory, void *virtual_address)
{
	return 0;
}

void paging_load_physical(void *page_directory, void *physical_address, void *virtual_address, uint32_t flags)
{
}