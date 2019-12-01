#include <i686/isr.h>
#include <i686/descriptor_tables.h>
#include <i686/pic.h>
#include <portio.h>

void (*interrupt_handlers[MAX_ISR_NUMBER])(struct isr_arguments*);

void isr_init()
{
	for(int i = 0; i < MAX_ISR_NUMBER; i++) {
		if(i >= IRQ0 && i <= IRQ15)
			interrupt_handlers[i] = default_irq_handler;
		else
			interrupt_handlers[i] = default_isr_handler;
	}
}

void install_interrupt_handler(uint8_t interrupt_number, void (*handler)(struct isr_arguments*))
{
	interrupt_handlers[interrupt_number] = handler;
}

void default_isr_handler(struct isr_arguments *args)
{
}
void default_irq_handler(struct isr_arguments *args)
{
	outb(PIC1, PIC_ACK);

	// This is the slave that sent this
	if(args->interrupt_number - IRQ0 > 8)
		outb(PIC2, PIC_ACK);
}