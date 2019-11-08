#pragma once

#include <stddef.h>

struct registers_s {
   uint32_t ds;                           // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;             // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

void isr_handler(struct registers_s regs);