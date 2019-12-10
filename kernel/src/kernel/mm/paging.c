#include <i686/isr.h>
#include <mm/kmalloc.h>
#include <mm/palloc.h>
#include <mm/paging.h>
#include <kpanic.h>
#include <kprint.h>
#include <string.h>

/**
 * Note:
 * 	page_directory -> physical address of table table
 * 	page table     -> physical address of mapped page
 * 	
 * 	page_directory recursively mapped to 0xFFFFF000
*/

static uint32_t *paging_directory = (uint32_t*)0xFFFFF000;

void paging_init()
{
	kprintf(KPRINT_DEBUG "Paging Directory Address: 0x%x\n", paging_directory);
    install_interrupt_handler(14, page_fault_handler);
}

void * paging_virtual_to_physical(void *virtual_address)
{
	uint32_t pdindex, ptindex;
	uint32_t *pt;

	pdindex = (uint32_t)virtual_address >> 22;
    ptindex = (uint32_t)virtual_address >> 12 & 0x03FF;
    
    if((paging_directory[pdindex] & PAGE_PRESENT) == 0x00) {
    	kprintf(KPRINT_ERROR "Page table does not exist/loaded\n");
    	return NULL;
    }

    pt = (uint32_t*)(REFLECTED_PAGE_TABLE_BASE_ADDRESS + PAGE_SIZE * pdindex);
    if(pt == 0x00) {
    	kprintf(KPRINT_ERROR "Page does not exist/loaded\n");
    	return NULL;
    }
	
    return (void *)((pt[ptindex] & ~0xFFF) + ((uint32_t)virtual_address & 0xFFF));
}

void paging_map(void *physical_address, void *virtual_address, uint32_t flags)
{
	uint32_t pdindex, ptindex;
	uint32_t *pt, pt_entry;

	pdindex = (uint32_t)virtual_address >> 22;
    ptindex = (uint32_t)virtual_address >> 12 & 0x03FF;
    
    if((paging_directory[pdindex]) == 0x00) {
    	// Create a new page table entry and update page directory
    	kprintf(KPRINT_DEBUG "Creating new page table\n");
    	
    	pt = kmalloc_a(PAGE_SIZE, PAGE_SIZE);
    	if(!pt) {
    		kpanic("Failed to malloc region to create page table!");
    	}
    	
    	pt = paging_virtual_to_physical(pt);
    	if(!pt) {
    		kpanic("Failed to get physical address of new page table");
    	}

    	paging_directory[pdindex] = ((uint32_t)pt & ~0xFFF) | PAGE_PRESENT | PAGE_READ_WRITE;
    }

    pt = (uint32_t*)(REFLECTED_PAGE_TABLE_BASE_ADDRESS + PAGE_SIZE * pdindex);
    if(pt == 0x00) {
    	// TODO: Page already exists, what do we do now???
    	kprintf(KPRINT_DEBUG "Page already exists! (Physical: 0x%x, Virtual: 0x%x)\n",
    		(uint32_t)physical_address, (uint32_t)virtual_address);
    	return;
    }
    
    pt_entry = flags & 0xFFF;
    pt_entry |= (uint32_t)physical_address; 
    pt[ptindex] = pt_entry;

    /**
     * Wipe page contents
    */
    memset(virtual_address, 0, PAGE_SIZE);

    /**
     * Need to flush TLB changes
     * XXX: Is this needed actually?
     */
    paging_switch_directory(paging_directory, 0);
}

void paging_switch_directory(uint32_t * page_dir, uint32_t phys)
{
    if(!phys)
        page_dir = paging_virtual_to_physical(page_dir);

    asm volatile("mov %0, %%cr3" :: "r"((uint32_t)page_dir));
}

void page_fault_handler(struct isr_arguments *args)
{
    if(args->cr2 == 0x00) {
        kpanic("Page Fault: Attempted to access page 0x00000000");
    }

    // Does page exist, but not present?
    if(args->error_code & 0x1) {
        // Page protection violation
        kprintf("Need to make present\n");
    }

    if(args->error_code & 0x2) {
        // Write access
        kprintf("WRITE\n");
    } else {
        // Read access
        uint32_t addr;
        // TODO: Determine which permissions/flags to set for the page
        addr = palloc_physical();
        paging_map((void*)addr,
            (void*)(args->cr2),
            PAGE_PRESENT | PAGE_READ_WRITE);
    }

    if(args->error_code & 0x4) {
        // When set, the page fault was caused while CPL = 3.
        // This does not necessarily mean that the page fault was a privilege violation.
        kprintf("PRIV VIOLATE\n");
    }

    if(args->error_code & 0x8) {
        // Reserved bit set to 1. Should never happen
        kprintf(KPRINT_ERROR "Reserved bit set on page!\n");
    }

    if(args->error_code & 0x10) {
        // Instruction fetch from NX-page
        kprintf("NX\n");
    }
}