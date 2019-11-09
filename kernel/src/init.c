#include <kernel/kprint.h>
#include <kernel/timer.h>
#include <kernel/serial.h>
#include <kernel/i686/pic.h>
#include <kernel/i686/descriptor_tables.h>
#include <macros.h>

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit()
{
	// Initialization
	kprint_init();
	serial_init();
	enable_serial_output();

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