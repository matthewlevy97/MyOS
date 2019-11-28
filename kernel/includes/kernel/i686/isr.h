#pragma once

#include <stddef.h>

#define MAX_ISR_NUMBER 256

struct isr_arguments {
	uint32_t interrupt_number, error_code;
};

void isr_handler(struct isr_arguments args);