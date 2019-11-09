#include <kernel/i686/pic.h>
#include <kernel/i686/descriptor_tables.h>
#include <kernel/portio.h>
#include <kernel/kprint.h>

static void remap(uint16_t master_base, uint16_t slave_base);

void pic_init()
{
	uint8_t master_mask, slave_mask;

	remap(0x20, 0x28);

	// XXX: Enables timer interrupt only
	master_mask = 0xFF ^ 1;
	slave_mask  = 0xFF;

	set_pic_mask(PIC1_DATA, master_mask);
	set_pic_mask(PIC2_DATA, slave_mask);

	// Enable interrupts
	asm volatile ("sti");
}

/**
 * @brief      Sets the IRQ mask for the PICs
 *
 * @param[in]  mask  Setting a bit disables that IRQ
 */
void set_pic_mask(uint16_t port, uint8_t mask)
{
    outb(port, mask);
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