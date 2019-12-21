#include <mm/kmalloc.h>
#include <mm/paging.h>
#include <multitasking/task.h>

task_t *current_task;

/*
 * TODO:
 * 	Disable interrupts in here
 */

static void * create_new_task_stack();

void task_init()
{
	current_task = task_create(NULL, 0, paging_directory_address_physical(), NO_CREATE_STACK);
}

task_t *task_create(void (*main)(), uint32_t eflags, void *pagedir_physical, uint32_t creation_flags)
{
	task_t *task;
	uint32_t *stack_ptr;

	task = (task_t*)kmalloc(sizeof(task_t));

	task->registers.eax = 0;
	task->registers.ebx = 0;
	task->registers.ecx = 0;
	task->registers.edx = 0;
	task->registers.esi = 0;
	task->registers.edi = 0;
	
	task->registers.eflags = eflags;
	
	task->registers.eip = (uintptr_t)main;
    task->registers.cr3 = (uintptr_t)pagedir_physical;

    if(creation_flags & NO_CREATE_STACK) {
    	task->registers.esp = 0;
    } else {
    	stack_ptr = create_new_task_stack();
    	*stack_ptr = (uintptr_t)main; // eip

    	task->registers.esp = (uint32_t)stack_ptr;
    }

    task->next = NULL;

    return task;
}

void task_yield()
{
	task_switch(current_task->next);
}

/*
 * TODO: Create new page directory with new kernel stack
 * TODO: If not in kernel -> local stack, and local heap; else, local stack = kernel stack, no create local heap
*/
static inline void * create_new_task_stack()
{
	paging_map((void*)PAGE_ALIGN(TASK_STACK_BASE_ADDRESS), PAGE_PRESENT | PAGE_READ_WRITE);

	return (void*)(TASK_STACK_BASE_ADDRESS - (TASK_STACK_BASE_ADDRESS & 3) - sizeof(int) * 16);
}