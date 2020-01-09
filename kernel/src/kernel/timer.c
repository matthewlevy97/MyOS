#include <timer.h>
#include <portio.h>
#include <i686/pic.h>
#include <multitasking/process.h>

/**
 * @brief      Initialize the PIT timer
 *
 * @param[in]  frequency  The frequency to trigger the timer on in HZ
 */
void timer_init(uint32_t frequency)
{
	uint32_t divisor;
	uint8_t l, h;

	// The value we send to the PIT is the value to divide it's input clock
	// (1193180 Hz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.
	divisor = 1193180 / frequency;

	// 0x36 tells timer to repeat when it hits 0
	outb(PIT_CMD, 0x36);

	l = (uint8_t)(divisor & 0xFF);
	h = (uint8_t)((divisor >> 8) & 0xFF);

	// Send the frequency divisor.
	outb(PIT_CHAN0, l);
	outb(PIT_CHAN0, h);

	// Enable PIT IRQ
	enable_irq(0);
}

/**
 * @brief      Handles preemptive multitasking code
 *
 * @param      args  The arguments passed from the initial ISR handler
 */
void timer_interrupt_handler(struct isr_arguments *args)
{
	// Acknowledge PIC
	outb(PIC1, PIC_ACK);

	process_yield();
}