.section .rodata

kpanic_string:
	.asciz "--- snip ---\nKernel Panic:\n\tAddress: 0x%x\n\t%s\n"
kpanic_stack_trace:
	.asciz "Stack Trace:\n"

.section .text

.extern irq_disable
.type irq_disable, @function

.extern kprintf
.type kprintf, @function

.extern dump_hex
.type dump_hex, @function

.global kpanic_implementation
.type kpanic_implementation, @function

kpanic_implementation:
	call irq_disable
	
	// Display error message
	push $kpanic_string
	call kprintf
	add $8, %esp

	/*
	Stack Dump
	*/
	xor %edi, %edi
	mov %esp, %esi
1:	
	push $8 // Print 8 hex per line
	push %esi
	call dump_hex
	add $8, %esp

	inc %edi
	sub $32, %esi // Move to next 4 integers
	cmp $10, %edi // Only print 4 lines
	jbe 1b

2:	hlt
	jmp 2b
.size kpanic_implementation_size, . - kpanic_implementation