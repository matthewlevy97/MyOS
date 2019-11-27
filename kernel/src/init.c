#include <macros.h>
#include <kernel/kprint.h>
#include <kernel/kpanic.h>
#include <kernel/multiboot2.h>
#include <kernel/multiboot_parser.h>
#include <kernel/serial.h>
#include <kernel/timer.h>
#include <kernel/i686/descriptor_tables.h>
#include <kernel/i686/pic.h>
#include <kernel/mm/kmalloc.h>
#include <kernel/mm/paging.h>

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit(void * mb_header, uint32_t mb_magic)
{
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

	paging_init();

	kmalloc_init(4 * MB);
	kprintf(KPRINT_DEBUG "KMalloc Initialized\n");
	
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