#include <kprint.h>
#include <i686/isr.h>
#include <mm/kmalloc.h>
#include <mm/palloc.h>
#include <mm/paging.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>

process_t current_process;

/*
 * TODO:
 * 	Disable interrupts in here
 */

static uintptr_t stack_randomize_base();

// TODO: Get a better method for PID creation
static pid_t pid_current = 0;

/**
 * @brief      Initialize process handling
 */
void process_init()
{
	current_process = process_create2(NULL, 0, paging_directory_address(), NO_CREATE_STACK, PRIORITY_LOW);
}

/**
 * @brief      Create a new process control block
 *
 * @param[in]  main             The main function to set EIP to on first run of code
 * @param[in]  creation_flags   Flags used for the creation of the process
 * @param[in]  priority         The priority of the process
 *
 * @return     Pointer to the Process Control Block, or NULL on failure
 */
process_t process_create(void (*main)(), uint32_t creation_flags, priority_t priority)
{
	uint32_t eflags;
	void *pagedir_virtual;
	
	// XXX: Do I need to get the current process's eflags? Should I set to a static number? Set to 0?
	eflags = eflags_get();
	pagedir_virtual = paging_clone_directory(paging_directory_address(), CLONE_KERNEL_ONLY);

	return process_create2(main, eflags, pagedir_virtual, creation_flags, priority);
}

/**
 * @brief      Create a new process control block
 *
 * @param[in]  main             The main function to set EIP to on first run of code
 * @param[in]  eflags           The value to set to eflags on startup
 * @param      pagedir_virtual  The page dirirectory (virtual address) of the process
 * @param[in]  creation_flags   Flags used for the creation of the process
 * @param[in]  priority         The priority of the process
 *
 * @return     Pointer to the Process Control Block, or NULL on failure
 */
process_t process_create2(void (*main)(), uint32_t eflags,
	void *pagedir_virtual, uint32_t creation_flags,
	priority_t priority)
{
	process_t process;
	
	process = (process_t)kmalloc(sizeof(struct process_control_block));
	if(!process)
		return NULL;

	process->registers.eax = 0;
	process->registers.ebx = 0;
	process->registers.ecx = 0;
	process->registers.edx = 0;
	process->registers.esi = 0;
	process->registers.edi = 0;
	
	process->registers.eflags = eflags;
	
	process->registers.eip = (uintptr_t)main;
    process->registers.cr3 = (uintptr_t)paging_virtual_to_physical(pagedir_virtual);

    if(creation_flags & NO_CREATE_STACK) {
    	process->registers.esp      = 0;
    	process->tss_esp0 = 0;
    } else {
    	// Setup user stack
    	process->registers.esp = PROCESS_USER_STACK_BASE_ADDRESS - stack_randomize_base();
    	paging_create_page_table((void*)process->registers.esp,
    		PAGE_PRESENT | PAGE_READ_WRITE | PAGE_USER_ACCESS, pagedir_virtual);

    	// Setup kernel stack
    	process->tss_esp0 = PROCESS_KERNEL_STACK_BASE_ADDRESS - stack_randomize_base();
    	paging_create_page_table((void*)process->tss_esp0,
    		PAGE_PRESENT | PAGE_READ_WRITE, pagedir_virtual);
    }

    process->interrupt_sync_depth = 0;

    // Only allow running in kernel mode if the current process is also running in kernel mode
    // XXX: I think this is secure, might need to revisit
    if((creation_flags & KERNEL_MODE) && current_process->user_mode)
    	process->user_mode = 0;
    else
    	process->user_mode = 1;

    process->pid      = pid_current++;
    process->priority = priority;

    // Add the process to the scheduler
    scheduler_add_process(process);

    return process;
}

/**
 * @brief      Yield control of the current process to the next process in the PCB linked-list
 */
void process_yield()
{
	process_t next;

	next = scheduler_get_next();
	
	if(current_process->pid != next->pid) {
		process_switch(next);
	}
}

/**
 * @brief      Dump information about a provided process
 *
 * @param      process  The process to dump
 */
void dump_process(process_t process)
{
	kprintf("------------------------\n");
	kprintf("Process PID:   %d\n", process->pid);
	kprintf("\t> Registers:\n");
    kprintf("\t\tEAX:         0x%x\n", process->registers.eax);
    kprintf("\t\tEBX:         0x%x\n", process->registers.ebx);
    kprintf("\t\tECX:         0x%x\n", process->registers.ecx);
    kprintf("\t\tEDX:         0x%x\n", process->registers.edx);
    kprintf("\t\tESI:         0x%x\n", process->registers.esi);
    kprintf("\t\tEDI:         0x%x\n", process->registers.edi);
    kprintf("\t\tESP:         0x%x\n", process->registers.esp);
    kprintf("\t\tEBP:         0x%x\n", process->registers.ebp);
    kprintf("\t\tEIP:         0x%x\n", process->registers.eip);
    kprintf("\t\tCR3:         0x%x\n", process->registers.cr3);
    kprintf("\n\t> Misc:\n");
    kprintf("\t\tTSS_ESP0:    0x%x\n", process->tss_esp0);
    kprintf("\t\tPCB Address: 0x%x\n", process);
    kprintf("------------------------\n");
}

/**
 * @brief      Basis for ASLR
 *
 * @return     Return a random offset for the stack to start at
 */
static inline uintptr_t stack_randomize_base()
{
	// TODO: Return a random value here
	return 64 + pid_current * 4;
}