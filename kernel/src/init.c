#include <kernel/kprint.h>
#include <kernel/i686/pic.h>
#include <kernel/i686/descriptor_tables.h>
#include <kernel/timer.h>
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
	timer_init(50);

	kprintf(KPRINT_SUCCESS "Kernel Loaded!\n");

	while(1);
	__builtin_unreachable ();
}