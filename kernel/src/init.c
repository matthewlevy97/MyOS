#include <helpers.h>
#include <stdint.h>

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

void FUNCTION_NO_RETURN kinit()
{
	uint16_t* terminal_buffer = VGA_MEMORY;

	terminal_buffer[0] = (uint16_t)('a') | (uint16_t)(4 << 8);

	while(1);
	__builtin_unreachable ();
}