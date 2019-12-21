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
    	task->registers.esp = (uintptr_t)create_new_task_stack();

    	stack_ptr = task->registers.esp;
    	stack_ptr[0] = 0x42424242;      // ebx
    	stack_ptr[1] = 0xCAFEBABE;      // esi
    	stack_ptr[2] = 0x41414141;      // edi
    	stack_ptr[3] = 0xDEADBEEF;      // ebp
    	stack_ptr[4] = (uintptr_t)main; // eip
    }

    task->next = NULL;

    return task;
}

void task_yield()
{
	task_t *current;

	task_switch(current_task->next);
}

static inline void * create_new_task_stack()
{
	paging_map((void*)TASK_STACK_BASE_ADDRESS - PAGE_SIZE, PAGE_PRESENT | PAGE_READ_WRITE);

	return (void*)TASK_STACK_BASE_ADDRESS;
}