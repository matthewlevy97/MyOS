#pragma once

#include <stddef.h>
#include <macros.h>

#define PROCESS_STACK_BASE_ADDRESS 0x7FFFFFFF

enum process_creation_flags {
	NO_CREATE_STACK = 0x01
};

typedef uint32_t pid_t;

struct process_registers {
	uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
};

struct process_control_block {
	struct process_registers registers;
	uintptr_t tss_esp0;
	struct process_control_block *next;
	pid_t pid;
};

extern struct process_control_block *current_process;

void process_init();
struct process_control_block *process_create(void (*main)(), uint32_t eflags,
	void *pagedir_virtual, uint32_t creation_flags);

void process_yield();
void process_switch(struct process_control_block *next_process);

void dump_process(struct process_control_block *process);