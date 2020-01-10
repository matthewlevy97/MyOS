#include <i686/isr.h>
#include <mm/kmalloc.h>
#include <mm/palloc.h>
#include <mm/paging.h>
#include <assert.h>
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

static void map_implementation(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags);
static inline void native_flush_tlb_single(uintptr_t addr);

extern uintptr_t * kernel_page_directory;

/**
 * @brief      Initialize paging code
 */
void paging_init()
{
	kprintf(KPRINT_DEBUG "Paging Directory Address: 0x%x\n", paging_virtual_to_physical(&kernel_page_directory));
    install_interrupt_handler(14, page_fault_handler);
}

/**
 * @brief      Get the current paging directory (virtual) address
 *
 * @return     Pointer to the virtual address of the paging directory
 */
void * paging_directory_address()
{
    return (void*)REFLECTED_PAGE_DIRECTORY_ADDRESS;
}

/**
 * @brief      Convert a virtual address to a physical address
 *
 * @param      virtual_address  The virtual address to convert
 *
 * @return     Pointer to physical address
 */
void * paging_virtual_to_physical(void *virtual_address)
{
    uintptr_t *paging_directory, pdindex, ptindex;
	uintptr_t *pt;

    paging_directory = (uintptr_t*)REFLECTED_PAGE_DIRECTORY_ADDRESS;

	pdindex = (uint32_t)virtual_address >> 22;
    ptindex = (uint32_t)virtual_address >> 12 & 0x03FF;
    
    if((paging_directory[pdindex] & PAGE_PRESENT) == 0x00) {
    	kprintf(KPRINT_ERROR "Page table does not exist/loaded 0x%x\n", paging_directory[pdindex]);
    	return NULL;
    }

    pt = (uintptr_t*)(REFLECTED_PAGE_TABLE_BASE_ADDRESS + PAGE_SIZE * pdindex);
    if(pt == 0x00) {
    	kprintf(KPRINT_ERROR "Page does not exist/loaded\n");
    	return NULL;
    }
	
    return (void *)((pt[ptindex] & ~0xFFF) + ((uintptr_t)virtual_address & 0xFFF));
}

/**
 * @brief      Clone a page directory
 *
 * @param      directory_virtual  The page directory to copy (virtual address)
 *
 * @return     Clone of the page directory (virtual address)
 */
void * paging_clone_directory(void *directory_virtual, uint32_t clone_flags)
{
    uintptr_t *dst, *src;
    uint32_t entry;

    dst = (uintptr_t*)kmalloc(PAGE_SIZE);
    src = (uintptr_t*)directory_virtual;
    
    // Start entry to copy over from passed page directory
    entry = 0;

    if(clone_flags & CLONE_KERNEL_ONLY) {
        entry = KERNEL_CODE_START_PAGE_DIRECTORY_INDEX;
    }

    // Copy entries
    for(; entry < PAGE_DIRECTORY_ENTRIES - 1; entry++)
        dst[entry] = src[entry];

    // Setup reflection
    dst[REFLECTED_PAGE_DIRECTORY_ENTRY] = (uintptr_t)paging_virtual_to_physical(dst);
    dst[REFLECTED_PAGE_DIRECTORY_ENTRY] |= PAGE_PRESENT | PAGE_READ_WRITE;

    return (void*)dst;
}

/**
 * @brief      Creates a new page table entry for a given virtual address in a provided page directory
 *
 * @param      virtual_address           The virtual address to map
 * @param[in]  page_flags                The flags for the page when it is created
 * @param      paging_directory_virtual  The paging directory (virtual address) to use
 */
void paging_create_page_table(void *virtual_address, uint32_t page_flags, uint32_t *paging_directory_virtual)
{
    uint32_t pdindex, ptindex, physical_address;
    uint32_t *pt, *pt_physical, pt_entry;

    physical_address = (uint32_t)palloc_physical();
    if(physical_address == 0x00)
        return;

    pdindex = (uint32_t)virtual_address >> 22;
    ptindex = ((uint32_t)virtual_address >> 12) & 0x3FF;

    // XXX: Page table entry already exists
    if(paging_directory_virtual[pdindex] != 0x00)
        return;

    pt = kmalloc(PAGE_SIZE);
    if(!pt)
        kpanic("Failed to malloc region to create page table!");

    pt_physical = paging_virtual_to_physical(pt);
    if(!pt_physical)
        kpanic("Failed to get physical address of new page table!");
    paging_directory_virtual[pdindex] = ((uint32_t)pt_physical & ~0xFFF) | PAGE_PRESENT | PAGE_READ_WRITE;

    pt_entry = page_flags & 0xFFF;
    pt_entry |= physical_address; 
    pt[ptindex] = pt_entry;
}

/**
 * @brief      Map a virtual address into memory
 *
 * @param      virtual_address   The virtual address to map the page to
 * @param[in]  page_flags        The flags for the page when it is created
 * @param[in]  mapping_flags     The flags for how to map the page
 */
inline void paging_map(void *virtual_address, uint32_t flags, uint32_t mapping_flags)
{
    void * physical_address;
    
    physical_address = (void*)palloc_physical();
    if(physical_address == 0x00)
        return;
    
    map_implementation(physical_address, virtual_address, flags, mapping_flags);
}

/**
 * @brief      Map a provided physical page to a provided virtual address
 *
 * @param      physical_address  The physical address of the page to use. Will mark the page in-use.
 * @param      virtual_address   The virtual address to map the page to
 * @param[in]  page_flags        The flags for the page when it is created
 * @param[in]  mapping_flags     The flags for how to map the page
 */
inline void paging_map2(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags)
{
    if(physical_address == 0x00)
        return;

    palloc_mark_inuse((uintptr_t)physical_address);

    map_implementation(physical_address, virtual_address, page_flags, mapping_flags);
}

/**
 * @brief      Actual implementation of code to map a page into memory
 *
 * @param      physical_address  The physical address of the page to map
 * @param      virtual_address   The virtual address to map the page to
 * @param[in]  page_flags        The flags for the page when it is created
 * @param[in]  mapping_flags     The flags for how to map the page
 */
static void map_implementation(void *physical_address, void *virtual_address, uint32_t page_flags, uint32_t mapping_flags)
{
	uint32_t *paging_directory, pdindex, ptindex;
	uint32_t *pt, pt_entry;
    
    paging_directory = (uint32_t*)REFLECTED_PAGE_DIRECTORY_ADDRESS;

    if(virtual_address == 0x00)
        return;
    
    pdindex = (uint32_t)virtual_address >> 22;
    ptindex = ((uint32_t)virtual_address >> 12) & 0x3FF;
    
    if((paging_directory[pdindex]) == 0x00) {
    	// Create a new page table entry and update page directory
    	pt = kmalloc(PAGE_SIZE);
    	if(!pt)
    		kpanic("Failed to malloc region to create page table!");
    	
    	pt = paging_virtual_to_physical(pt);
    	if(!pt)
    		kpanic("Failed to get physical address of new page table!");

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
     * Switch to the page table, flushing changes
     */
    if(mapping_flags & MAPPING_FLUSH_CHANGES)
        paging_switch_directory(paging_directory, 0);
}

/**
 * @brief      Unmap a virtual address from memory
 *
 * @param      virtual_address  The virtual address
 */
void paging_unmap(void *virtual_address)
{
    uint32_t *paging_directory, pdindex, ptindex, *pt;
    uintptr_t physical_address;

    paging_directory = (uintptr_t*)REFLECTED_PAGE_DIRECTORY_ADDRESS;

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

    // XXX: Might need reference counts when a page is mapped into multiple virtual / page directories
    //palloc_release((uintptr_t)physical_address);
}

/**
 * @brief      Switch out the current page directory
 *
 * @param      page_dir  The page directory's address
 * @param[in]  phys      Is the page directory's address physical? (boolean)
 */
void paging_switch_directory(uint32_t * page_dir, uint32_t phys)
{
    if(!phys)
        page_dir = paging_virtual_to_physical(page_dir);

    if(page_dir == 0x00)
        kpanic("Trying to switch to page table at address 0x00000000!");

    asm volatile("mov %0, %%cr3" :: "r"((uint32_t)page_dir));
}

/**
 * @brief      Handles page faults when they occur (via interrupt)
 *
 * @param      args  The arguments passed from the initial ISR handler
 */
void page_fault_handler(struct isr_arguments *args)
{
    uintptr_t accessed_page;

    // Move to page boundary
    accessed_page = PAGE_ALIGN((uintptr_t)args->cr2);

    if(accessed_page == 0x00) {
        kprintf("Page Fault at Address: 0x%x\n", args->eip);
        kpanic("Attempted to access page 0x00000000");
    }
    
    // Does page exist, but not present?
    if(args->error_code & 0x1) {
        // Page protection violation
        // TODO: Make page present
        kprintf("Need to make present\n");
    }

    if(args->error_code & 0x2) {
        // Write access
    } else {
        // Read access
    }
    
    // Load page
    // TODO: Determine which permissions/flags to set for the page
    paging_map((void*)(accessed_page), PAGE_PRESENT | PAGE_READ_WRITE, MAPPING_WIPE_PAGE | MAPPING_FLUSH_CHANGES);

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

/**
 * @brief      Invalidate a TLB entry given an address. (Perform TLB shootdown).
 *
 * @param[in]  addr  The address (physical) to invalidate
 */
static inline void native_flush_tlb_single(uintptr_t addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}