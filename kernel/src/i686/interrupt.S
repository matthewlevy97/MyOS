.section .text

.extern isr_handler
.type isr_handler, @function

.global isr_common_stub
.type isr_common_stub, @function

/**
* Macros to help with the creation of ISRs
*/
.macro ISR_NOERRCODE isr_number
	.global isr\isr_number
	.type isr\isr_number, @function

	isr\isr_number:
		push $0
		push $\isr_number
		jmp isr_common_stub
	.size isr\isr_number, . - isr\isr_number 
.endm

.macro ISR_ERRCODE isr_number
	.global isr\isr_number
	.type isr\isr_number, @function

	isr\isr_number:
		push $\isr_number
		jmp isr_common_stub
	.size isr\isr_number, . - isr\isr_number
.endm

/**
* Macros to help with the creation of IRQs
*/
.macro IRQ irq_number isr_number
	.global irq\irq_number
	.type irq\irq_number, @function

	irq\irq_number:
		push $0
		push $\isr_number
		jmp isr_common_stub
	.size irq\irq_number, . - irq\irq_number
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

/*
* IRQ Numbers
* 0  - Programmable Interrupt Timer Interrupt
* 1  - Keyboard Interrupt
* 2  - Cascade (used internally by the two PICs. never raised)
* 3  - COM2 (if enabled)
* 4  - COM1 (if enabled)
* 5  - LPT2 (if enabled)
* 6  - Floppy Disk
* 7  - LPT1 / Unreliable "spurious" interrupt (usually)
* 8  - CMOS real-time clock (if enabled)
* 9  - Free for peripherals / legacy SCSI / NIC
* 10 - Free for peripherals / SCSI / NIC
* 11 - Free for peripherals / SCSI / NIC
* 12 - PS2 Mouse
* 13 - FPU / Coprocessor / Inter-processor
* 14 - Primary ATA Hard Disk
* 15 - Secondary ATA Hard Disk
*/
IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

/**
* This is our common ISR stub. It saves the processor state, sets
* up for kernel mode segments, and finally restores the stack frame.
*/

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

	// TODO: Determine what information I need to send to isr_handler
	push %esi
	push %edi
	call isr_handler
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