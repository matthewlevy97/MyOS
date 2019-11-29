#include <macros.h>
#include <kprint.h>
#include <kpanic.h>
#include <serial.h>
#include <timer.h>
#include <i686/descriptor_tables.h>
#include <i686/isr.h>
#include <i686/pic.h>
#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <mm/palloc.h>
#include <multiboot/multiboot2.h>
#include <multiboot/multiboot_parser.h>

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit(void * mb_header, uint32_t mb_magic)
{
	uint32_t kmalloc_init_start_address, kmalloc_init_end_address;
	uint32_t max_memory_address;

	// Initialization
	kprint_init();
	serial_init();
	enable_serial_output();

	// Ensure everything is good
	if(mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		kprintf(KPRINT_ERROR "Not loaded with Multiboot 2!\n");
		kpanic();
	}
	
	struct multiboot_tag_string *mb_cmdline;
	mb_cmdline = multiboot_get_tag(mb_header, MULTIBOOT_TAG_TYPE_CMDLINE);
	if(mb_cmdline) {
		kprintf(KPRINT_DEBUG "Command Line Arguments: %s\n", mb_cmdline->string);
	}

	isr_init();
	kprintf(KPRINT_DEBUG "Installed default ISR handlers\n");

	paging_init();
	kprintf(KPRINT_DEBUG "Paging Initialized\n");

	kmalloc_init_start_address = PAGE_ALIGN((uint32_t)paging_virtual_to_physical(mb_header)) + PAGE_SIZE;
	kmalloc_init_end_address = kmalloc_init((void*)kmalloc_init_start_address);
	kprintf(KPRINT_DEBUG "KMalloc Initialized\n");
	
	// TODO: Get the actual high_memory address from multiboot
	max_memory_address = kmalloc_init_end_address + 1024 * PAGE_SIZE;
	palloc_init(kmalloc_init_end_address, max_memory_address);
	kprintf(KPRINT_DEBUG "Page Allocator Initialized\n");

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