#include <kernel/serial.h>
#include <kernel/portio.h>

static int is_transmit_empty();

void serial_init()
{
	outb(COM1 + 1, 0x00);    // Disable all interrupts
	outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(COM1 + 1, 0x00);    //                  (hi byte)
	outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_write(char c)
{
	while (!is_transmit_empty());
	outb(COM1, c);
}

static int is_transmit_empty()
{
	return inb(COM1 + 5) & 0x20;
}
