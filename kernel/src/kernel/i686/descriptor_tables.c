#include <i686/descriptor_tables.h>
#include <i686/isr.h>
#include <string.h>

struct gdt_entry_s gdt_entries[6];
struct gdt_ptr_s   gdt_ptr;

struct idt_entry_s idt_entries[MAX_ISR_NUMBER];
struct idt_ptr_s   idt_ptr;

struct task_state_segment_s tss_entry;

static void gdt_init();
static void idt_init();
static void irq_init();

static void gdt_set_gate(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
static void tss_set_gate(uint8_t index, struct task_state_segment_s *tss_entry, uint8_t granularity);
static void idt_set_gate(uint8_t index, uint32_t base, uint16_t sel, uint8_t flags);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/**
 * @brief      Initialize all descriptor tables
 */
void descriptor_tables_init()
{
	gdt_init();
	gdt_flush((uint32_t)&gdt_ptr);

	idt_init();
	irq_init();
	idt_flush((uint32_t)&idt_ptr);
}

/**
 * @brief      Sets the kernel stack in a TSS
 *
 * @param[in]  stack_address  The stack address
 */
void set_kernel_stack(uintptr_t stack_address)
{
	// TODO: Call this function
	tss_entry.esp0 = stack_address;
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
	 * 	Code and Data segments should NOT overlap
	 */
	// Null segment
	gdt_set_gate(0, 0, 0, 0, 0);

	// Code segment
	gdt_set_gate(1, 0, 0xFFFFFFFF,
		GDT_PRESENT | GDT_NOT_SYSTEM_SEGMENT  | GDT_EXECUTABLE | GDT_READ_WRITE, 0xCF);

	// Data segment
	gdt_set_gate(2, 0, 0xFFFFFFFF,
		GDT_PRESENT | GDT_NOT_SYSTEM_SEGMENT | GDT_READ_WRITE, 0xCF);

	// User mode code segment
	gdt_set_gate(3, 0, 0xFFFFFFFF,
		GDT_PRESENT | GDT_RING_3 | GDT_NOT_SYSTEM_SEGMENT  | GDT_EXECUTABLE | GDT_READ_WRITE, 0xCF);

	// User mode data segment
	gdt_set_gate(4, 0, 0xFFFFFFFF,
		GDT_PRESENT | GDT_RING_3 | GDT_NOT_SYSTEM_SEGMENT | GDT_READ_WRITE, 0xCF);

	// Task State Segment (TSS)
	tss_set_gate(5, &tss_entry, 0xCF);
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
	idt_set_gate(ISR0,  (uint32_t)isr0 , 0x08, 0x8E);
	idt_set_gate(ISR1,  (uint32_t)isr1 , 0x08, 0x8E);
	idt_set_gate(ISR2,  (uint32_t)isr2 , 0x08, 0x8E);
	idt_set_gate(ISR3,  (uint32_t)isr3 , 0x08, 0x8E);
	idt_set_gate(ISR4,  (uint32_t)isr4 , 0x08, 0x8E);
	idt_set_gate(ISR5,  (uint32_t)isr5 , 0x08, 0x8E);
	idt_set_gate(ISR6,  (uint32_t)isr6 , 0x08, 0x8E);
	idt_set_gate(ISR7,  (uint32_t)isr7 , 0x08, 0x8E);
	idt_set_gate(ISR8,  (uint32_t)isr8 , 0x08, 0x8E);
	idt_set_gate(ISR9,  (uint32_t)isr9 , 0x08, 0x8E);
	idt_set_gate(ISR10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(ISR11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(ISR12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(ISR13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(ISR14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(ISR15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(ISR16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(ISR17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(ISR18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(ISR19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(ISR20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(ISR21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(ISR22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(ISR23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(ISR24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(ISR25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(ISR26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(ISR27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(ISR28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(ISR29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(ISR30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(ISR31, (uint32_t)isr31, 0x08, 0x8E);
}

static void irq_init()
{
	/**
	 * irqXX defined in interrupt.S
	 */
	idt_set_gate(IRQ0,  (uint32_t)irq0, 0x08, 0x8E);
	idt_set_gate(IRQ1,  (uint32_t)irq1, 0x08, 0x8E);
	idt_set_gate(IRQ2,  (uint32_t)irq2, 0x08, 0x8E);
	idt_set_gate(IRQ3,  (uint32_t)irq3, 0x08, 0x8E);
	idt_set_gate(IRQ4,  (uint32_t)irq4, 0x08, 0x8E);
	idt_set_gate(IRQ5,  (uint32_t)irq5, 0x08, 0x8E);
	idt_set_gate(IRQ6,  (uint32_t)irq6, 0x08, 0x8E);
	idt_set_gate(IRQ7,  (uint32_t)irq7, 0x08, 0x8E);
	idt_set_gate(IRQ8,  (uint32_t)irq8, 0x08, 0x8E);
	idt_set_gate(IRQ9,  (uint32_t)irq9, 0x08, 0x8E);
	idt_set_gate(IRQ10, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(IRQ11, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(IRQ12, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(IRQ13, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(IRQ14, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(IRQ15, (uint32_t)irq15, 0x08, 0x8E);
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
static void gdt_set_gate(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
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
 * @brief      Populate a GDT entry with a Task State Segment
 *
 * @param[in]  index        The index into the gdt_entries array
 * @param      tss_entry    Pointer to the tss entry to load
 * @param[in]  granularity  The granularity of the segment
 */
static void tss_set_gate(uint8_t index, struct task_state_segment_s *tss_entry, uint8_t granularity)
{
	uint32_t base, limit;

	base  = (uint32_t)tss_entry;
	limit = sizeof(struct task_state_segment_s);

	gdt_entries[index].base_low    = (base & 0xFFFF);
	gdt_entries[index].base_middle = (base >> 16) & 0xFF;
	gdt_entries[index].base_high   = (base >> 24) & 0xFF;

	gdt_entries[index].limit_low   = (limit & 0xFFFF);
	gdt_entries[index].granularity = (limit >> 16) & 0x0F;

	gdt_entries[index].granularity |= granularity & 0xF0; // 0xCF is same granulatity for others
	gdt_entries[index].access      = GDT_PRESENT | GDT_RING_3 | GDT_EXECUTABLE | GDT_READ_WRITE | GDT_ACCESSED;

	memset(tss_entry, 0, sizeof(struct task_state_segment_s));
	tss_entry->ss0  = 0x10; // Kernel stack segment
	tss_entry->esp0 = 0x00; // Should be set before task switching
}

/**
 * @brief      Populate an IDT entry
 *
 * @param[in]  index     The index into the idt_entries array
 * @param[in]  base      The entry point for the ISR
 * @param[in]  selector  The selector
 * @param[in]  flags     The flags
 */
static void idt_set_gate(uint8_t index, uint32_t base, uint16_t sel, uint8_t flags)
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