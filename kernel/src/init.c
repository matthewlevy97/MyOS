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
	pic_init();
	descriptor_tables_init();
	serial_init();
	timer_init(50);

	enable_serial_output();
	kprintf(KPRINT_SUCCESS "Kernel Loaded!\n");

	// Enable interrupts
	asm volatile ("sti");

	// Enable timer IRQ
	enable_irq(0);

	while(1);
	__builtin_unreachable ();
}