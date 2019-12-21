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

static uintptr_t *paging_directory = (uint32_t*)0xFFFFF000;

static void map_implementation(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags);
static inline void native_flush_tlb_single(uintptr_t addr);

void paging_init()
{
	kprintf(KPRINT_DEBUG "Paging Directory Address: 0x%x\n", paging_directory);
    install_interrupt_handler(14, page_fault_handler);
}

void * paging_directory_address_physical()
{
    return paging_virtual_to_physical(paging_directory);
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

/*
 * Rewrite:
 *  paging_map() to do map_implementation(palloc_physical(), virt_addr, flags, mflags_default)
 *  paging_map2() to do {palloc_mark(phys_addr); map_implementation(phy, virt, pflags, mflags)}
 */

inline void paging_map(void *virtual_address, uint32_t flags)
{
    void * physical_address;
    
    physical_address = (void*)palloc_physical();
    if(physical_address == 0x00)
        return;

    map_implementation(physical_address, virtual_address, flags, MAPPING_WIPE_PAGE);
}
inline void paging_map2(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags)
{
    if(physical_address == 0x00)
        return;

    palloc_mark_inuse((uintptr_t)physical_address);

    map_implementation(physical_address, virtual_address, page_flags, mapping_flags);
}

static void map_implementation(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags)
{
	uint32_t pdindex, ptindex;
	uint32_t *pt, pt_entry;

    if(virtual_address == 0x00)
        return;

    pdindex = (uint32_t)virtual_address >> 22;
    ptindex = ((uint32_t)virtual_address >> 12) & 0x03FF;
    
    if((paging_directory[pdindex]) == 0x00) {
    	// Create a new page table entry and update page directory
    	kprintf(KPRINT_DEBUG "Creating new page table\n");
    	
    	pt = kmalloc_a(PAGE_SIZE, PAGE_SIZE);
    	if(!pt) {
    		kpanic("Failed to malloc region to create page table!");
    	}
    	
    	pt = paging_virtual_to_physical(pt);
    	if(!pt) {
    		kpanic("Failed to get physical address of new page table!");
    	}

    	paging_directory[pdindex] = ((uint32_t)pt & ~0xFFF) | PAGE_PRESENT | PAGE_READ_WRITE;
    }

    pt = (uint32_t*)(REFLECTED_PAGE_TABLE_BASE_ADDRESS + PAGE_SIZE * pdindex);
    if(pt == 0x00) {
    	// TODO: Page already exists, what do we do now???
    	kprintf(KPRINT_DEBUG "Page already exists! (Physical: 0x%x, Virtual: 0x%x)\n",
    		(uintptr_t)physical_address, (uintptr_t)virtual_address);
    	return;
    }
    
    pt_entry = page_flags & 0xFFF;
    pt_entry |= (uint32_t)physical_address; 
    pt[ptindex] = pt_entry;

    /**
     * Wipe page contents
    */
    if(mapping_flags & MAPPING_WIPE_PAGE)
        memset(virtual_address, 0, PAGE_SIZE);

    /**
     * Need to flush TLB changes
     * XXX: Is this needed actually?
     */
    paging_switch_directory(paging_directory, 0);
}

/**
 * @brief      Unmap a virtual address from memory
 *
 * @param      virtual_address  The virtual address
 */
void paging_unmap(void *virtual_address)
{
    uint32_t pdindex, ptindex, *pt;
    uintptr_t physical_address;

    pdindex = (uint32_t)virtual_address >> 22;
    ptindex = ((uint32_t)virtual_address >> 12) & 0x03FF;
    
    // Nothing to do here as it doesn't exist
    if((paging_directory[pdindex]) == 0x00)
        return;

    pt = (uint32_t*)(REFLECTED_PAGE_TABLE_BASE_ADDRESS + PAGE_SIZE * pdindex);
    if(pt != 0x00)
        return;
    
    native_flush_tlb_single((uintptr_t)virtual_address);

    physical_address = pt[ptindex] & ~0xFFF;
    pt[ptindex] = 0;

    palloc_release((uintptr_t)physical_address);
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
        kpanic("Attempted to access page 0x00000000");
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
        kprintf("READ\n");
    }
    
    // Load page
    // TODO: Determine which permissions/flags to set for the page
    paging_map((void*)(args->cr2),
        PAGE_PRESENT | PAGE_READ_WRITE);

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

static inline void native_flush_tlb_single(uintptr_t addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}