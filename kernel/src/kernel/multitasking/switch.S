// Constants for segment selectors
.set KERNEL_CODE_SEGMENT, 0x8
.set KERNEL_DATA_SEGMENT, 0x10

.set USER_CODE_SEGMENT, 0x1B
.set USER_DATA_SEGMENT, 0x23

.section .text

.extern current_process
.type current_process, @object

.extern tss_entry
.type tss_entry, @object

.extern irq_get_sync_depth
.type irq_get_sync_depth, @function

.extern irq_set_sync_depth
.type irq_set_sync_depth, @function

.global process_switch
.type process_switch, @function

.global process_creation_complete
.type process_creation_complete, @function

/* void process_creation_complete(void *eip); */
process_creation_complete:
    cli

    // Get EIP
    mov 4(%esp), %eax
    
    // Save EIP
    mov current_process, %esi
    mov %eax, 32(%esi)

    // Reload
    jmp reload_registers
.size process_creation_complete, . - process_creation_complete

/* void process_switch(struct process_control_block *next_process); */
process_switch:
    pushf

    // Disable interrupts
    cli

	// save edi to stack to be able to get its value below
	push %edi

	mov current_process, %edi	// address of current process_control_block structure

    // Get EFLAGS
    pop 36(%edi)

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
    je reload_registers
    mov %eax, %cr3		// Only reload if different
    jmp reload_registers	
.size process_switch, . - process_switch

reload_registers:
    // Reload registers
    mov 24(%esi), %esp
    mov 28(%esi), %ebp

    // Get interrupt sync depth
    push 48(%esi)
    call irq_set_sync_depth
    add $4, %esp

    /*
    Do we need to switch segment selectors?
    */
    mov 52(%esi), %eax
    cmp $0, %eax // if(current_process->user_mode == 0)
    je .kernel_mode_selectors
    mov $USER_DATA_SEGMENT, %ebx
    mov $USER_CODE_SEGMENT, %ecx
    jmp .load_selectors

.kernel_mode_selectors:
    mov $KERNEL_DATA_SEGMENT, %ebx
    mov $KERNEL_CODE_SEGMENT, %ecx

    /*
    Setup for IRET
    */
.load_selectors:
    mov %bx, %ax
    mov %ax, %ds
    mov %ax, %es 
    mov %ax, %fs 
    mov %ax, %gs

    // Push Data Segment
    push %ebx

    // Push SS
    push %esp

    // Push EFLAGS
    push 36(%esi)

    // Push User Code Segment
    push %ecx

    // Push return address
    push 32(%esi)

    mov 0(%esi), %eax
    mov 4(%esi), %ebx
    mov 8(%esi), %ecx
    mov 12(%esi), %edx

    mov 16(%esi), %edi
    mov 20(%esi), %esi

    iret
.size reload_registers, . - reload_registers