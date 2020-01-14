#include <string.h>
#include <i686/isr.h>
#include <mm/paging.h>
#include <multitasking/elf.h>

static uintptr_t stack_randomize_base();

// TODO: Remove test program once actually loading things
void test_program()
{
	while(1);
}

/**
 * @brief      Load an elf into the current process's memory space
 */
void FUNCTION_NO_RETURN elf_load()
{
	uint32_t page_permissions;

	// Get page permissions to set
	page_permissions = PAGE_PRESENT | PAGE_READ_WRITE;
	if(!(current_process->creation_flags & KERNEL_MODE))
		page_permissions |= PAGE_USER_ACCESS;
	
	// Load ELF code, etc
	paging_map((void*)ELF_USER_CODE_BASE_ADDRESS,
		page_permissions, MAPPING_WIPE_PAGE | MAPPING_FLUSH_CHANGES);

	// TODO: Remove test program once actually loading things
	memcpy((void*)ELF_USER_CODE_BASE_ADDRESS, test_program, PAGE_SIZE);

	// TODO: Remove PAGE_READ_WRITE permission

	// Is program to run in usermode or kernel mode?
	if(!(current_process->creation_flags & KERNEL_MODE))
		current_process->user_mode = 1;

	// Jump execution to new EIP
	process_creation_complete((void*)ELF_USER_CODE_BASE_ADDRESS);

	// XXX: Should never reach here
	while(1);
}

/**
 * @brief      Setup the initial stack for a program
 *
 * @param[in]  process  The process to create the stack for
 */
void elf_setup_stack(process_t process)
{
	uint32_t page_permissions;

	// Get page permissions to set
	page_permissions = PAGE_PRESENT | PAGE_READ_WRITE;
	if(!(process->creation_flags & KERNEL_MODE))
		page_permissions |= PAGE_USER_ACCESS;

	// Setup user/program stack
	process->registers.esp = ELF_USER_STACK_BASE_ADDRESS - stack_randomize_base();
    paging_create_page_table((void*)process->registers.esp,
        page_permissions, process->pagedir_virtual);

    // Setup kernel/interrupt stack
    process->tss_esp0 = ELF_KERNEL_STACK_BASE_ADDRESS - stack_randomize_base();
    paging_create_page_table((void*)process->tss_esp0,
        PAGE_PRESENT | PAGE_READ_WRITE, process->pagedir_virtual);
}

/**
 * @brief      Basis for ASLR
 *
 * @return     Return a random offset for the stack to start at
 */
static inline uintptr_t stack_randomize_base()
{
	// TODO: Return a random value here
	return 64;
}