#include <mm/kmalloc.h>
#include <mm/palloc.h>
#include <mm/paging.h>
#include <multitasking/process.h>

struct process_control_block *current_process;

/*
 * TODO:
 * 	Disable interrupts in here
 */

static void * create_new_process_stack(uintptr_t *page_dir);

static pid_t pid_current = 0;

void process_init()
{
	current_process = process_create(NULL, 0, paging_directory_address(), NO_CREATE_STACK);
}

struct process_control_block *process_create(void (*main)(), uint32_t eflags,
	void *pagedir_virtual, uint32_t creation_flags)
{
	struct process_control_block *process;
	
	process = (struct process_control_block*)kmalloc(sizeof(struct process_control_block));

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
    	process->registers.esp = 0;
    } else {
    	process->registers.esp = (uintptr_t)create_new_process_stack(pagedir_virtual);
    }

    process->next = NULL;
    process->pid  = pid_current++;

    return process;
}

void process_yield()
{
	if(current_process->next)
		process_switch(current_process->next);
}

void dump_process(struct process_control_block *process)
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

/*
 * TODO: Create new page directory with new kernel stack
 * TODO: If not in kernel -> local stack, and local heap; else, local stack = kernel stack, no create local heap
*/
static inline void * create_new_process_stack(uintptr_t *page_dir)
{
	void * esp;

	esp = (void*)(PROCESS_STACK_BASE_ADDRESS - (PROCESS_STACK_BASE_ADDRESS & 3) - sizeof(int) * 16);
	paging_create_page_table(esp, PAGE_PRESENT | PAGE_READ_WRITE, page_dir);

	return esp;
}