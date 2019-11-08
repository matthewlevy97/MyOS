.section .rodata
isr_ascii:
	.asciz "ISR: 0x%x\n"

.section .text

/**
* Macros to help with the creation of ISRs
*/
.macro ISR_NOERRCODE isr_number
	.global isr\isr_number
	.type isr\isr_number, @function

	isr\isr_number :
		cli
		push $0
		push $\isr_number
		jmp isr_common_stub
	.size isr\isr_number, . - isr\isr_number 
.endm

.macro ISR_ERRCODE isr_number
	.global isr\isr_number
	.type isr\isr_number, @function

	isr\isr_number :
		cli
		push $\isr_number
		jmp isr_common_stub
	.size isr\isr_number, . - isr\isr_number
.endm

/**
* ISR Numbers
* 0  - Division by zero exception
* 1  - Debug exception
* 2  - Non maskable interrupt
* 3  - Breakpoint exception
* 4  - 'Into detected overflow'
* 5  - Out of bounds exception
* 6  - Invalid opcode exception
* 7  - No coprocessor exception
* 8  - Double fault (pushes an error code)
* 9  - Coprocessor segment overrun
* 10 - Bad TSS (pushes an error code)
* 11 - Segment not present (pushes an error code)
* 12 - Stack fault (pushes an error code)
* 13 - General protection fault (pushes an error code)
* 14 - Page fault (pushes an error code)
* 15 - Unknown interrupt exception
* 16 - Coprocessor fault
* 17 - Alignment check exception (pushes an error code)
* 18 - Machine check exception
* 19-31 - Reserved (pushes an error code for 30)
*/

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

.extern kprintf
.type kprintf, @function

/**
* This is our common ISR stub. It saves the processor state, sets
* up for kernel mode segments, calls the C-level fault handler,
* and finally restores the stack frame.
*/
.global isr_common_stub
.type isr_common_stub, @function

isr_common_stub:
	pusha           // Push: edi,esi,ebp,esp,ebx,edx,ecx,eax

	mov %ds, %ax    // Lower 16-bits of eax = ds.
	push %eax       // save the data segment descriptor

	mov $0x10, %ax  // load the kernel data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	// Get the ISR number
	mov 36(%esp), %edi
	// Get the error code
	mov 40(%esp), %esi

	cmp $0xD, %edi
	jne .1
	call kpanic

.1:
	push %edi
	push $isr_ascii
	call kprintf
	add $8, %esp

	pop %eax        // reload the original data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa
	add $8, %esp   // Clean up ISR number and error code
	iret           // pops: CS, EIP, EFLAGS, SS, and ESP
.size isr_common_stub, . - isr_common_stub