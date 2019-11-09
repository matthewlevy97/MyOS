#include <kernel/i686/pic.h>
#include <kernel/i686/descriptor_tables.h>
#include <kernel/portio.h>
#include <kernel/kprint.h>

static void remap(uint16_t master_base, uint16_t slave_base);

static uint8_t master_mask, slave_mask;

void pic_init()
{
	remap(0x20, 0x28);

	// Disable everything
	master_mask = 0xFF;
	slave_mask  = 0xFF;

	outb(PIC1_DATA, master_mask);
	outb(PIC2_DATA, slave_mask);
}

void enable_irq(uint8_t irq)
{
	if(irq < 8) {
		// Enable on master PIC
		master_mask &= ~(1 << irq);
		outb(PIC1_DATA, master_mask);
	} else {
		// Enable on slave PIC
		irq -= 8;

		// IRQ2 on master enables cascade
		master_mask &= ~(1 << PIC_CASCADE_BIT);
		slave_mask &= ~(1 << irq);

		outb(PIC1_DATA, master_mask);
		outb(PIC2_DATA, slave_mask);
	}
}

void disable_irq(uint8_t irq)
{
	if(irq < 8) {
		// Disable on master PIC
		master_mask |= (1 << irq);
		outb(PIC1_DATA, master_mask);
	} else {
		// Disable on slave PIC
		irq -= 8;
		slave_mask |= (1 << irq);
		outb(PIC2_DATA, slave_mask);

		if(slave_mask == 0xFF) {
			// If nothing on slave, we can disable cascade
			master_mask |= (1 << PIC_CASCADE_BIT);
			outb(PIC1_DATA, master_mask);
		}
	}
}

/**
 * @brief      Remap the Slave and Master PICs to new ISRs.
 * 				Each PIC uses 8 ISR entries. No checks are done
 * 				in code to ensure overlap doesn't exist.
 *
 * @param[in]  master_base  The master base ISR for the master PIC
 * @param[in]  slave_base   The slave base ISR for the slave PIC
 */
static void remap(uint16_t master_base, uint16_t slave_base)
{
	uint8_t a1, a2;

	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, master_base);               // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, slave_base);                // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                         // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                         // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}