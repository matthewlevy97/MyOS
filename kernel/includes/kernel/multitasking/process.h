#pragma once

#include <stddef.h>
#include <macros.h>

#define PROCESS_STACK_BASE_ADDRESS 0x7FFFFFFF

/**
 * @brief      Flags for creation of a new process
 */
enum process_creation_flags {
	NO_CREATE_STACK = 0x01
};

/**
 * Typedef for the PID of a process
 */
typedef uint32_t pid_t;

/**
 * @brief      Register information for a process.
 * 
 * DO NOT MODIFY. Any changes need to be reflected in switch.S
 */
struct process_registers {
	uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
};

/**
 * @brief      Stores information about a process
 */
struct process_control_block {
	/* Update switch.S if modified */
	struct process_registers registers;
	uintptr_t tss_esp0;
	/* Can modify below this point freely */
	
	struct process_control_block *next;
	pid_t pid;
};

/**
 * @brief      Holds a pointer to the currently running process
 */
extern struct process_control_block *current_process;

void process_init();
struct process_control_block *process_create(void (*main)(), uint32_t eflags,
	void *pagedir_virtual, uint32_t creation_flags);

void process_yield();
void process_switch(struct process_control_block *next_process);

void dump_process(struct process_control_block *process);