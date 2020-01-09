.section .text

.extern current_process
.type current_process, @object

.extern tss_entry
.type tss_entry, @object

.extern irq_disable
.type irq_disable, @function

.extern irq_resume
.type irq_resume, @function

.global process_switch
.type process_switch, @function

/* void process_switch(struct process_control_block *next_process); */
process_switch:
	// save edi to stack to be able to get its value below
	push %edi
    
    // Disable interrupts
    call irq_disable

	mov current_process, %edi	// address of current process_control_block structure

	// Save registers
	mov %eax, 0(%edi)
	mov %ebx, 4(%edi)
	mov %ecx, 8(%edi)
	mov %edx, 12(%edi)
	
	mov (%esp), %eax		// Get edi value from stack (pushed)
    add $4, %esp
	mov %eax, 16(%edi)
	mov %esi, 20(%edi)

    // Get return address
    mov (%esp), %eax
    mov %eax, 32(%edi)

    // Get EFLAGS
    pushf
    pop 36(%edi)

    // Save interrupt sync depth
    call irq_get_sync_depth
    mov %eax, 48(%edi)

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

    // Get interrupt sync depth
    push 48(%esi)
    call irq_get_sync_depth
    add $4, %esp

    // Set EFLAGS
    push 36(%esi)
    popf

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
    call irq_resume

	ret
.size process_switch, . - process_switch