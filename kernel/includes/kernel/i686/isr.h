#pragma once

#include <stddef.h>

#define MAX_ISR_NUMBER 256

void isr_handler(uint8_t isr_number, uint8_t error_code);