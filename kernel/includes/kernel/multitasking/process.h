#pragma once

#include <stddef.h>
#include <macros.h>

/**
 * @brief      Flags for creation of a new process
 */
enum process_creation_flags {
	KERNEL_MODE     = 0x01,
	COPY_SYNC_DEPTH = 0x02,
};

/**
 * @brief      Priorities for a given process
 */
typedef enum {
		PRIORITY_CRITICAL,
		PRIORITY_ELEVATED,
		PRIORITY_NORMAL,
		PRIORITY_LOW,
		PRIORITY_NUMBER_OF_PRIORITIES
} priority_t;

/**
 * Typedef for the PID of a process
 */
typedef uint16_t pid_t;
#define MAX_PROCESS_PID ((1 << (sizeof(pid_t) * 8)) - 1)

/**
 * @brief      Register information for a process.
 * 
 * DO NOT MODIFY. Any changes need to be reflected in switch.S
 */
struct process_registers {
	uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} __attribute__((packed));

/**
 * @brief      Stores information about a process
 */
struct process_control_block {
	/* Update switch.S if modified */
	struct process_registers registers;
	uintptr_t tss_esp0;
	uint32_t interrupt_sync_depth;
	uint32_t user_mode;
	/* Can modify below this point freely */
	
	void *pagedir_virtual;
	
	uint32_t creation_flags;
	
	priority_t priority;
	pid_t pid;
} __attribute__((packed));

typedef struct process_control_block* process_t;

/**
 * @brief      Holds a pointer to the currently running process
 */
extern process_t current_process;

void process_init();

process_t process_create(uint32_t creation_flags, priority_t priority);
process_t process_create2(uint32_t eflags,
	void *pagedir_virtual, uint32_t creation_flags,
	priority_t priority);

void process_yield();

void process_creation_complete(void *eip);
void process_switch(process_t next_process);

void dump_process(process_t process);