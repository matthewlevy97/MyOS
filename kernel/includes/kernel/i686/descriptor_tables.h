#pragma once

#include <stddef.h>

struct task_state_segment_s {
   uint32_t link; // The previous TSS - linked list if using hardware multitasking
   uint32_t esp0; // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;  // The stack segment to load when we change to kernel mode.
   uint32_t esp1;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;
   uint32_t cs;
   uint32_t ss;
   uint32_t ds;
   uint32_t fs;
   uint32_t gs;
   uint32_t ldtr;
   uint16_t trap;
   uint16_t iomap_base;
} __attribute__((packed));

enum gdt_access_bits {
  GDT_ACCESSED             = 0x01,
  GDT_READ_WRITE           = 0x02,
  GDT_DIRECTION_CONFORMING = 0x04,
  GDT_EXECUTABLE           = 0x08,
  GDT_NOT_SYSTEM_SEGMENT   = 0x10,
  GDT_RING_0               = 0x00,
  GDT_RING_3               = 0x60,
  GDT_PRESENT              = 0x80
};

struct gdt_entry_s {
   uint16_t limit_low;           // The lower 16 bits of the limit.
   uint16_t base_low;            // The lower 16 bits of the base.
   uint8_t  base_middle;         // The next 8 bits of the base.
   uint8_t  access;              // Access flags, determine what ring this segment can be used in.
   uint8_t  granularity;
   uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed));

struct gdt_ptr_s {
   uint16_t limit;               // The upper 16 bits of all selector limits.
   uint32_t base;                // The address of the first gdt_entry_t struct.
} __attribute__((packed));

struct idt_entry_s {
   uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
   uint16_t sel;                 // Kernel segment selector.
   uint8_t  always0;             // This must always be zero.
   uint8_t  flags;               // More flags. See documentation.
   uint16_t base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));

struct idt_ptr_s {
   uint16_t limit;
   uint32_t base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));

enum {
   ISR0 = 0,
   ISR1,
   ISR2,
   ISR3,
   ISR4,
   ISR5,
   ISR6,
   ISR7,
   ISR8,
   ISR9,
   ISR10,
   ISR11,
   ISR12,
   ISR13,
   ISR14,
   ISR15,
   ISR16,
   ISR17,
   ISR18,
   ISR19,
   ISR20,
   ISR21,
   ISR22,
   ISR23,
   ISR24,
   ISR25,
   ISR26,
   ISR27,
   ISR28,
   ISR29,
   ISR30,
   ISR31
};
enum {
   IRQ0 = 32,
   IRQ1,
   IRQ2,
   IRQ3,
   IRQ4,
   IRQ5,
   IRQ6,
   IRQ7,
   IRQ8,
   IRQ9,
   IRQ10,
   IRQ11,
   IRQ12,
   IRQ13,
   IRQ14,
   IRQ15,
};

extern void gdt_flush(uint32_t address);
extern void idt_flush(uint32_t address);
extern void tss_flush();

void descriptor_tables_init();
void set_kernel_stack(uintptr_t stack_address);