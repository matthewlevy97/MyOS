#include <acpi.h>
#include <macros.h>
#include <kprint.h>
#include <kpanic.h>
#include <serial.h>
#include <string.h>
#include <timer.h>
#include <i686/descriptor_tables.h>
#include <i686/isr.h>
#include <i686/pic.h>
#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <mm/palloc.h>
#include <multiboot/multiboot2.h>
#include <multiboot/multiboot_parser.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>

extern uint32_t _kernel_end, _kernel_start, _kernel_offset;

static uint32_t get_palloc_start_address(struct multiboot_tag_mmap *mb_mmap);

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit(void * mb_header, uint32_t mb_magic)
{
	struct multiboot_tag_string *mb_cmdline;
	struct multiboot_tag_mmap *mb_mmap;
	struct multiboot_tag_new_acpi *mb_acpi;

	uint32_t palloc_init_address;

	// Initialization
	kprint_init();
	serial_init();
	enable_serial_output();

	// Multiboot validation
	if(mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		kpanic("Not loaded with Multiboot 2!");
	}
	if(((uint32_t)mb_header) & 7) {
		kpanic("Unaligned multiboot information!");
	}

	// Multiboot parsing
	mb_cmdline = multiboot_get_tag(mb_header, MULTIBOOT_TAG_TYPE_CMDLINE);
	if(mb_cmdline) {
		kprintf(KPRINT_DEBUG "Command Line Arguments: %s\n", mb_cmdline->string);
	}

	mb_mmap = multiboot_get_tag(mb_header, MULTIBOOT_TAG_TYPE_MMAP);
	if(!mb_mmap) {
		kpanic("Could not get memory map from multiboot header");
	}
	palloc_init_address = get_palloc_start_address(mb_mmap);

	isr_init();
	kprintf(KPRINT_DEBUG "Installed default ISR handlers\n");

	palloc_init(palloc_init_address);
	kprintf(KPRINT_DEBUG "Page Allocator Initialized\n");
	// Mark multiboot header (page) in use
	palloc_mark_inuse(PAGE_ALIGN((uint32_t)paging_virtual_to_physical(mb_header)));

	paging_init();
	kprintf(KPRINT_DEBUG "Paging Initialized\n");

	kmalloc_init();
	kprintf(KPRINT_DEBUG "KMalloc Initialized\n");

	/* Stage 2 copies and loads information from the memory map into the heap */
	palloc_init2(palloc_init_address, mb_mmap);
	kprintf(KPRINT_DEBUG "Page Allocator (Stage 2) Initialized\n");

	pic_init();
	kprintf(KPRINT_DEBUG "PIC Initialized\n");

	descriptor_tables_init();
	kprintf(KPRINT_DEBUG "Descriptor Tables Initialized\n");

	timer_init(50);
	kprintf(KPRINT_DEBUG "PIT Initialized\n");

	mb_acpi = multiboot_get_tag(mb_header, MULTIBOOT_TAG_TYPE_ACPI_OLD);
	acpi_init(mb_acpi);
	kprintf(KPRINT_DEBUG "ACPI Initialized\n");

	scheduler_init();
	kprintf(KPRINT_DEBUG "Process Scheduler Initialized\n");

	process_init();
	kprintf(KPRINT_DEBUG "Multitasking Initialized\n");

	// Enable interrupts
	irq_enable();

	kprintf(KPRINT_SUCCESS "Kernel Loaded!\n");

	// Setup processes to run

	// Yield control over to scheduler
	process_yield();

	while(1) process_yield();
	__builtin_unreachable();
}

/**
 * @brief      Gets the start physical address for palloc to use from the multiboot memory map.
 *
 * @param      mb_mmap  The mememory map
 *
 * @return     The start address (page aligned) of the first physical page to use.
 */
static uint32_t get_palloc_start_address(struct multiboot_tag_mmap *mb_mmap)
{
	struct multiboot_mmap_entry mmap_entry;
	uint32_t start_address, number_entries;
	uint32_t kernel_start_address, kernel_end_address;

	number_entries = mb_mmap->size - ((uint32_t)&mb_mmap->entries - (uint32_t)&mb_mmap->type);
	number_entries /= mb_mmap->entry_size;

	kernel_start_address = (uint32_t)&_kernel_start - (uint32_t)&_kernel_offset;
	kernel_end_address = (uint32_t)&_kernel_end - (uint32_t)&_kernel_offset;

	// Determine start address to use (based on memory map)
	start_address = 0;
	for(uint32_t i = 0; i < number_entries; i++) {
		mmap_entry = mb_mmap->entries[i];
		
		/**
		* Only return an address if:
		* 	1) It is un-used/valid RAM
		* 	2) Starts after the kernel
		*/
		if(mmap_entry.type == MULTIBOOT_MEMORY_AVAILABLE && mmap_entry.addr) {
			if(mmap_entry.addr <= kernel_start_address && mmap_entry.addr + mmap_entry.len >= kernel_end_address)
				start_address = PAGE_ALIGN(kernel_end_address) + PAGE_SIZE;
			else
				start_address = PAGE_ALIGN(mmap_entry.addr);
			break;
		}
	}

	if(start_address == 0) {
		kpanic("Unable to determine a base address for palloc");
	}

	return start_address;
}