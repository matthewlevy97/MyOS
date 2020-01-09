#pragma once

#include <stddef.h>

#define MAX_ISR_NUMBER 256

#define SYNC_CLI() do {asm volatile("cli");} while(0);
#define SYNC_STI() do {asm volatile("sti");} while (0);

struct isr_arguments {
	uint32_t cr2, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
	uint32_t interrupt_number, error_code;
	uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

extern void (*interrupt_handlers[MAX_ISR_NUMBER])(struct isr_arguments*);

void isr_init();

void irq_set_sync_depth(uint32_t depth);
uint32_t irq_get_sync_depth();

void irq_disable();
void irq_resume();
void irq_enable();

void install_interrupt_handler(uint8_t interrupt_number, void (*handler)(struct isr_arguments*));

void default_isr_handler(struct isr_arguments *args);
void default_irq_handler(struct isr_arguments *args);

uint32_t eflags_get();