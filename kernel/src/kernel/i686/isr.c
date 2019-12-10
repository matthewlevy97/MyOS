#include <i686/isr.h>
#include <i686/descriptor_tables.h>
#include <i686/pic.h>
#include <portio.h>
#include <kprint.h>

void (*interrupt_handlers[MAX_ISR_NUMBER])(struct isr_arguments*);

static volatile uint32_t sync_depth;

/**
 * @brief      Initialize the interrupt handlers to the defaults below
 */
void isr_init()
{
	for(int i = 0; i < MAX_ISR_NUMBER; i++) {
		if(i >= IRQ0 && i <= IRQ15)
			interrupt_handlers[i] = default_irq_handler;
		else 
			interrupt_handlers[i] = default_isr_handler;
	}

	sync_depth = 0;
}

void irq_disable()
{
	uint32_t eflags;
	asm volatile("pushf\n\t"
				 "pop %%eax\n\t"
				 "movl %%eax, %0\n\t"
				 : "=r"(eflags)
				 :
				 : "%eax"
	);

	// Disable interrupts
	SYNC_CLI();

	// Is interrupt flag set
	if(eflags & (1 << 9))
		sync_depth = 1;
	else
		sync_depth++;
}
void irq_resume()
{
	// Only enable interrupts if returned to first call depth
	if(sync_depth == 0 || sync_depth == 1) {
		SYNC_STI();
	} else {
		sync_depth--;
	}
}
void irq_enable()
{
	sync_depth = 0;

	// Enable interrupts
	SYNC_STI();
}

/**
 * @brief      Installs a new interrupt service routine (ISR) handler
 *
 * @param[in]  interrupt_number  The interrupt number to attach to
 * @param[in]  handler           The handler function to be called
 */
void install_interrupt_handler(uint8_t interrupt_number, void (*handler)(struct isr_arguments*))
{
	interrupt_handlers[interrupt_number] = handler;
}

void default_isr_handler(struct isr_arguments *args)
{}
void default_irq_handler(struct isr_arguments *args)
{
	outb(PIC1, PIC_ACK);

	// This is the slave that sent this
	if(args->interrupt_number - IRQ0 > 8)
		outb(PIC2, PIC_ACK);
}