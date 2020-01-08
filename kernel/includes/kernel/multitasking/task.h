#pragma once

#include <stddef.h>
#include <macros.h>

#define TASK_STACK_BASE_ADDRESS 0x7FFFFFFF

enum task_creation_flags {
	NO_CREATE_STACK = 0x01
};

typedef struct {
	uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_registers_t;

typedef struct task_control_block {
	task_registers_t registers;
	uintptr_t tss_esp0;
	struct task_control_block *next;
} task_t;

extern task_t *current_task;

void task_init();
task_t *task_create(void (*main)(), uint32_t eflags, void *pagedir_physical, uint32_t creation_flags);

void task_yield();
void task_switch(task_t *next_task);