.section .rodata

kpanic_string:
	.asciz "Kernel Panic!\n"
kpanic_register_dump:
	.asciz "EAX: %x  EBX: %x  ECX: %x  EDX: %x\nEDI: %x  ESI: %x  EBP: %x  ESP: %x\nEIP: %x\n"
kpanic_stack_dump:
	.asciz "0x%x  "

.section .text

.extern kprintf
.type kprintf, @function

.global kpanic
.type kpanic, @function

kpanic:
	pusha // Save registers for next part
	push $kpanic_string
	call kprintf
	popa // Reload registers

	/*
	Dump Registers
	*/
	push %ebp
	push %esi
	push %edi
	push %edx
	push %ecx
	push %ebx
	push %eax
	push $kpanic_register_dump
	call kprintf
	add $32, %esp

	/*
	Stack Dump

	kpanic_stack_dump prints 8 hex values
	*/
	xor %edi, %edi
	mov %esp, %esi
.1:	
	push (%esi)
	push $kpanic_stack_dump
	call kprintf
	add $8, %esp

	inc %edi
	dec %esi
	cmp $64, %edi
	jbe .1

	cli
1:	hlt
	jmp 1b
.size kpanic, . - kpanic