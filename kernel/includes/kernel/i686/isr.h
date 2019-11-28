#pragma once

#include <stddef.h>

#define MAX_ISR_NUMBER 256

struct isr_arguments {
	uint32_t interrupt_number, error_code;
};

void isr_init();
void install_interrupt_handler(uint8_t interrupt_number, void (*handler)(struct isr_arguments));

void default_isr_handler(struct isr_arguments args);
void default_irq_handler(struct isr_arguments args);