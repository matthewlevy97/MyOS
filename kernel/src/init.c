#include <kernel/kprint.h>
#include <kernel/kpanic.h>
#include <kernel/i686/descriptor_tables.h>
#include <macros.h>

/**
 * @brief      Entry point into kernel for C code
 */
void FUNCTION_NO_RETURN kinit()
{
	// Initialization
	kprint_init();
	descriptor_tables_init();

	kprintf(KPRINT_SUCCESS "Kernel Loaded!\n");
	asm volatile ("int $0x8");

	while(1);
	__builtin_unreachable ();
}