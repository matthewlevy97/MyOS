#include <kernel/i686/descriptor_tables.h>
#include <string.h>

extern void gdt_flush(uint32_t address);
extern void idt_flush(uint32_t address);

struct gdt_entry_s gdt_entries[5];
struct gdt_ptr_s   gdt_ptr;

struct idt_entry_s idt_entries[256];
struct idt_ptr_s   idt_ptr;

static void gdt_init();
static void idt_init();

static void gdt_set_gate(uint8_t index,
	uint32_t base, uint32_t limit,
	uint8_t access, uint8_t granularity);
static void idt_set_gate(uint8_t index,
	uint32_t base, uint16_t sel, uint8_t flags);

/**
 * @brief      Initialize all descriptor tables
 */
void descriptor_tables_init()
{
	gdt_init();
	idt_init();
}

/**
 * @brief      Initialize the GDT
 */
static void gdt_init()
{
	gdt_ptr.limit = (sizeof(gdt_entries)) - 1;
	gdt_ptr.base  = (uint32_t)&gdt_entries;

	/**
	 * TODO: Update this
	 * 	User mode should not have full access to all memory
	 * 	Need a TSS segment
	 * 	Code and Data segments should NOT overlap
	 */
	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	gdt_flush((uint32_t)&gdt_ptr);
}

/**
 * @brief      Initialize the IDT
 */
static void idt_init()
{
	idt_ptr.limit = sizeof(idt_entries) -1;
	idt_ptr.base  = (uint32_t)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entries));

	/**
	 * isrXX defined in interrupt.S
	 */
	idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
	
	idt_flush((uint32_t)&idt_ptr);
}

/**
 * @brief      Populate a GDT entry
 *
 * @param[in]  index        The index into the gdt_entries array
 * @param[in]  base         The base address of the segment
 * @param[in]  limit        The limit, or length, of segment
 * @param[in]  access       The access rights/permissions for the segment
 * @param[in]  granularity  The granularity of the segment
 */
static void gdt_set_gate(uint8_t index,
	uint32_t base, uint32_t limit,
	uint8_t access, uint8_t granularity)
{
	gdt_entries[index].base_low    = (base & 0xFFFF);
	gdt_entries[index].base_middle = (base >> 16) & 0xFF;
	gdt_entries[index].base_high   = (base >> 24) & 0xFF;

	gdt_entries[index].limit_low   = (limit & 0xFFFF);
	gdt_entries[index].granularity = (limit >> 16) & 0x0F;

	gdt_entries[index].granularity |= granularity & 0xF0;
	gdt_entries[index].access      = access;
}

/**
 * @brief      Populate an IDT entry
 *
 * @param[in]  index     The index into the idt_entries array
 * @param[in]  base      The entry point for the ISR
 * @param[in]  selector  The selector
 * @param[in]  flags     The flags
 */
static void idt_set_gate(uint8_t index,
	uint32_t base, uint16_t sel, uint8_t flags)
{
	idt_entries[index].base_lo = base & 0xFFFF;
	idt_entries[index].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[index].sel     = sel;
	idt_entries[index].always0 = 0;

	/*
	* TODO:
	* 	We must uncomment the OR below when we get to using user-mode.
	* 	It sets the interrupt gate's privilege level to 3.
	*/
	idt_entries[index].flags   = flags /* | 0x60 */;
}