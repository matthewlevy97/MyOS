.section .rodata
kpanic_string:
	.ascii "Kernel Panic!\n\0"

.section .text

.extern kprintf
.type kprintf, @function

.global kpanic
.type kpanic, @function

kpanic:
	push $kpanic_string
	call kprintf

	cli
1:	hlt
	jmp 1b
.size kpanic, . - kpanic