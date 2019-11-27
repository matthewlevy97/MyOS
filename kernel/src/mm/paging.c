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
	struct page_table_entry *pt;
	kprintf(KPRINT_DEBUG "Paging Directory Address: 0x%x\n", paging_directory);
}

uint32_t paging_virtual_to_physical(void *virtual_address)
{
	uint32_t pdindex, ptindex;
	struct page_table_entry *pt;

	pdindex = (uint32_t)virtual_address >> 22;
    ptindex = (uint32_t)virtual_address >> 12 & 0x03FF;
    
    if(paging_directory[pdindex].present == 0x00) {
    	kprintf(KPRINT_ERROR "Page table does not exist/loaded\n");
    	return 0;
    }

    pt = (struct page_table_entry*)(REFLECTED_PAGE_TABLE_BASE_ADDRESS + 1024 * 4 * pdindex);
    if(pt == 0x00) {
    	// XXX: Are there any cases where the above checks passes and this one fails???
    	kprintf(KPRINT_ERROR "Page does not exist/loaded\n");
    	return 0;
    }
	
    return (void *)((pt[ptindex].address << 11) + ((uint32_t)virtual_address & 0xFFF));
}

void paging_load_physical(void *physical_address, void *virtual_address, uint32_t flags)
{
}