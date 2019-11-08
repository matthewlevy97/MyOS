#include <kernel/i686/isr.h>
#include <kernel/kprint.h>

void isr_handler(struct registers_s regs)
{
	kprintf("Unhandled Interrupt: 0x%x", regs.int_no);

	for(;;);
}