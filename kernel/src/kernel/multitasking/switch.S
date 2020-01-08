.section .text

.global current_task
.type current_task, @object

.global tss_entry
.type tss_entry, @object

.global task_switch
.type task_switch, @function

/*
TODO: Not saving EIP, or EFLAGS
*/

/* void task_switch(task_t *next_task); */
task_switch:
	// save edi to stack to be able to get its value below
	push %edi
    
    // TODO: Disable interrupts

	mov current_task, %edi	// address of current task_t structure

	// Save registers
	mov %eax, 0(%edi)
	mov %ebx, 4(%edi)
	mov %ecx, 8(%edi)
	mov %edx, 12(%edi)
	
	mov (%esp), %eax		// Get edi value from stack (pushed)
	mov %eax, 16(%edi)
	mov %esi, 20(%edi)

	// remove edi from stack
	// must be done before esp is saved
	pop %esi

    mov %esp, 24(%edi)
    mov %ebp, 28(%edi)

    // esi = next_task parameter
    mov 4(%esp), %esi
    // Set current_task to next_task
    mov %esi, current_task

    // Set new stack address
    mov 24(%esi), %esp

    // Load task state information
    mov 44(%esi), %ebx
    mov $tss_entry, %edi
    mov %ebx, 4(%edi)	// set ESP0 field in TSS

    // Get next task page directory address
    mov 40(%esi), %eax

    // Check if CR3 needed to be updated
    mov %cr3, %ecx
    cmp %eax, %ecx
    je .no_reload_page_dir
    mov %eax, %cr3		// Only reload if different

.no_reload_page_dir:
	// Reload registers
	mov 0(%esi), %eax
	mov 4(%esi), %ebx
	mov 8(%esi), %ecx
	mov 12(%esi), %edx

    mov 24(%esi), %esp
    mov 28(%esi), %ebp

    mov 16(%esi), %edi
    mov 20(%esi), %esi

	ret
.size task_switch, . - task_switch