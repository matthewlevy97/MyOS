.section .text

.global current_process
.type current_process, @object

.global tss_entry
.type tss_entry, @object

.global process_switch
.type process_switch, @function

/*
TODO: Not saving EFLAGS on switch
*/

/* void process_switch(struct process_control_block *next_process); */
process_switch:
	// save edi to stack to be able to get its value below
	push %edi
    
    // Disable interrupts
    cli

	mov current_process, %edi	// address of current process_control_block structure

	// Save registers
	mov %eax, 0(%edi)
	mov %ebx, 4(%edi)
	mov %ecx, 8(%edi)
	mov %edx, 12(%edi)
	
	mov (%esp), %eax		// Get edi value from stack (pushed)
	mov %eax, 16(%edi)
	mov %esi, 20(%edi)

	// Remove edi from stack
	// Must be done before esp is saved
    // Value is just ignored as it is already saved
	pop %esi

    // Get return address
    mov (%esp), %eax
    mov %eax, 32(%edi)

    mov %esp, 24(%edi)
    mov %ebp, 28(%edi)

    // esi = next_process parameter
    mov 4(%esp), %esi
    // Set current_process to next_process
    mov %esi, current_process

    // Set new stack address
    mov 24(%esi), %esp

    // Load process state information
    mov 44(%esi), %ebx
    mov $tss_entry, %edi
    mov %ebx, 4(%edi)	// set ESP0 field in TSS

    // Get next process page directory address
    mov 40(%esi), %eax

    // Check if CR3 needed to be updated
    mov %cr3, %ecx
    cmp %eax, %ecx
    je .no_reload_page_dir
    mov %eax, %cr3		// Only reload if different

.no_reload_page_dir:
	// Reload registers
    mov 24(%esi), %esp
    mov 28(%esi), %ebp

    // Set return address
    mov 32(%esi), %eax
    mov %eax, (%esp)

    mov 0(%esi), %eax
    mov 4(%esi), %ebx
    mov 8(%esi), %ecx
    mov 12(%esi), %edx

    mov 16(%esi), %edi
    mov 20(%esi), %esi

    // Re-enable interrupts
    sti

	ret
.size process_switch, . - process_switch