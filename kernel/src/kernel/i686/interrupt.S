.section .text

.extern interrupt_handlers
.type interrupt_handlers, @object

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
		cli
		push $0
		push $\isr_number
		jmp isr_common_stub
	.size isr\isr_number, . - isr\isr_number 
.endm

.macro ISR_ERRCODE isr_number
	.global isr\isr_number
	.type isr\isr_number, @function

	isr\isr_number:
		cli
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
		cli
		push $0
		push $\isr_number
		jmp isr_common_stub
	.size irq\irq_number, . - irq\irq_number
.endm

/**
* ISR Numbers
* ISR 0  - Division by zero exception
* ISR 1  - Debug exception
* ISR 2  - Non maskable interrupt
* ISR 3  - Breakpoint exception
* ISR 4  - 'Into detected overflow'
* ISR 5  - Out of bounds exception
* ISR 6  - Invalid opcode exception
* ISR 7  - No coprocessor exception
* ISR 8  - Double fault (pushes an error code)
* ISR 9  - Coprocessor segment overrun
* ISR 10 - Bad TSS (pushes an error code)
* ISR 11 - Segment not present (pushes an error code)
* ISR 12 - Stack fault (pushes an error code)
* ISR 13 - General protection fault (pushes an error code)
* ISR 14 - Page fault (pushes an error code)
* ISR 15 - Unknown interrupt exception
* ISR 16 - Coprocessor fault
* ISR 17 - Alignment check exception (pushes an error code)
* ISR 18 - Machine check exception
* ISR 19-31 - Reserved (pushes an error code for 30)
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
* IRQ 0  - Programmable Interrupt Timer Interrupt
* IRQ 1  - Keyboard Interrupt
* IRQ 2  - Cascade (used internally by the two PICs. never raised)
* IRQ 3  - COM2 (if enabled)
* IRQ 4  - COM1 (if enabled)
* IRQ 5  - LPT2 (if enabled)
* IRQ 6  - Floppy Disk
* IRQ 7  - LPT1 / Unreliable "spurious" interrupt (usually)
* IRQ 8  - CMOS real-time clock (if enabled)
* IRQ 9  - Free for peripherals / legacy SCSI / NIC
* IRQ 10 - Free for peripherals / SCSI / NIC
* IRQ 11 - Free for peripherals / SCSI / NIC
* IRQ 12 - PS2 Mouse
* IRQ 13 - FPU / Coprocessor / Inter-processor
* IRQ 14 - Primary ATA Hard Disk
* IRQ 15 - Secondary ATA Hard Disk
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
	mov $interrupt_handlers, %eax
	mov (%eax, %edi, 4), %eax

	// Push CR2
	mov %cr2, %ecx
	push %ecx

	cld
	push %esp
	call *%eax
	add $8, %esp // Cleanup ESP + CR2

	pop %eax        // reload the original data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa
	add $8, %esp   // Clean up ISR number and error code
	iret           // pops: CS, EIP, EFLAGS, SS, and ESP
.size isr_common_stub, . - isr_common_stub