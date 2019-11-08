#include <macros.h>
#include <kernel/kprint.h>
#include <kernel/kpanic.h>

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit()
{
	// Initialize output
	kprint_init();

	kprintf("Kernel Loaded!\n");

	while(1);
	__builtin_unreachable ();
}