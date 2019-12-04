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

extern uint32_t _kernel_end, _kernel_start, _kernel_offset;

static uint32_t get_palloc_start_address(struct multiboot_tag_mmap *mb_mmap);

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit(void * mb_header, uint32_t mb_magic)
{
	struct multiboot_tag_string *mb_cmdline;
	struct multiboot_tag_mmap *mb_mmap;

	void * tmp_mb_header;

	uint32_t palloc_init_address;

	// Initialization
	kprint_init();
	serial_init();
	enable_serial_output();

	// Multiboot parsing
	if(mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		kprintf(KPRINT_ERROR "Not loaded with Multiboot 2!\n");
		kpanic();
	}
	if(((uint32_t)mb_header) & 7) {
		kprintf(KPRINT_ERROR "Unaligned multiboot information!\n");
		kpanic();
	}

	mb_cmdline = multiboot_get_tag(mb_header, MULTIBOOT_TAG_TYPE_CMDLINE);
	if(mb_cmdline) {
		kprintf(KPRINT_DEBUG "Command Line Arguments: %s\n", mb_cmdline->string);
	}

	mb_mmap = multiboot_get_tag(mb_header, MULTIBOOT_TAG_TYPE_MMAP);
	if(!mb_mmap) {
		kprintf(KPRINT_ERROR "Could not get memory map from multiboot header\n");
		kpanic();
	}
	palloc_init_address = get_palloc_start_address(mb_mmap);

	isr_init();
	kprintf(KPRINT_DEBUG "Installed default ISR handlers\n");

	palloc_init(palloc_init_address);
	kprintf(KPRINT_DEBUG "Page Allocator Initialized\n");

	paging_init();
	kprintf(KPRINT_DEBUG "Paging Initialized\n");

	kmalloc_init();
	kprintf(KPRINT_DEBUG "KMalloc Initialized\n");

	/**
	 * Stage 2 copies and loads information from the memory map into the heap
	 */
	palloc_init2(palloc_init_address, mb_mmap);
	kprintf(KPRINT_DEBUG "Page Allocator (Stage 2) Initialized\n");

	// Migrate multiboot header to heap
	tmp_mb_header = kmalloc(*(uint32_t*)mb_header);
	if(!tmp_mb_header) {
		kprintf(KPRINT_ERROR "Unable to allocate space in heap for multiboot header\n");
		kpanic();
	}
	kprintf(KPRINT_DEBUG "Moving multiboot header from 0x%x to 0x%x\n", mb_header, tmp_mb_header);
	memcpy(tmp_mb_header, mb_header, *(uint32_t*)mb_header);
	mb_header = tmp_mb_header;

	pic_init();
	kprintf(KPRINT_DEBUG "PIC Initialized\n");

	descriptor_tables_init();
	kprintf(KPRINT_DEBUG "Descriptor Tables Initialized\n");

	timer_init(50);
	kprintf(KPRINT_DEBUG "PIT Initialized\n");

	kprintf(KPRINT_SUCCESS "Kernel Loaded!\n");

	// Enable interrupts
	asm volatile ("sti");

	while(1);
	__builtin_unreachable ();
}

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
		kprintf(KPRINT_ERROR "Unable to determine a base address for palloc\n");
		kpanic();
	}

	return start_address;
}