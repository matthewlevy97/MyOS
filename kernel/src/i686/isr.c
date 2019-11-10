#include <kernel/i686/isr.h>
#include <kernel/i686/descriptor_tables.h>
#include <kernel/i686/pic.h>
#include <kernel/portio.h>
#include <kernel/kprint.h>

void isr_handler(uint8_t isr_number, uint8_t error_code)
{
	if(isr_number >= IRQ0 && isr_number <= IRQ15) {
		isr_number -= IRQ0;

		// TODO: Handle IRQ

		// Send ack
		outb(PIC1, PIC_ACK);

		// This is the slave that sent this
		if(isr_number > 8)
			outb(PIC2, PIC_ACK);

	} else {
		// TODO: Handle ISR
		kprintf("ISR: 0x%x\n", isr_number);
	}
}