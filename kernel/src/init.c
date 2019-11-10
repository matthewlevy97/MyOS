#include <macros.h>
#include <kernel/kprint.h>
#include <kernel/kpanic.h>
#include <kernel/multiboot.h>
#include <kernel/serial.h>
#include <kernel/timer.h>
#include <kernel/i686/descriptor_tables.h>
#include <kernel/i686/pic.h>
#include <kernel/mm/kmalloc.h>

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit(struct multiboot_tag * mb_header, uint32_t mb_magic)
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

	/**
	 * Parse the multiboot header.
	 * The header is in the following format:
	 * 			+-------------------+
	 *	u32     | total_size        |
	 *	u32     | reserved          |
     *   	 	+-------------------+
	 * We cheat and use the multiboot_tag struct here. Ignore names
	 */
	kprintf(KPRINT_DEBUG "Multiboot Header Length: 0x%x\n", mb_header->type);

	pic_init();
	kprintf(KPRINT_DEBUG "PIC Initialized\n");

	descriptor_tables_init();
	kprintf(KPRINT_DEBUG "Descriptor Tables Initialized\n");

	timer_init(50);
	kprintf(KPRINT_DEBUG "PIT Initialized\n");

	kmalloc_init();
	kprintf(KPRINT_DEBUG "KMalloc Initialized\n");

	kprintf(KPRINT_SUCCESS "Kernel Loaded!\n");

	// Enable interrupts
	asm volatile ("sti");

	while(1);
	__builtin_unreachable ();
}