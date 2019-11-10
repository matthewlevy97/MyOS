/*
	General Constants
*/
.set KB, 1024             /* 1kb */
.set STACK_SIZE, 32*KB    /* 32 kb */

/*
	Setup Stack
*/
.section .bss
.align 16

stack_bottom:
.skip STACK_SIZE
stack_top:

.section .text

.extern kinit
.type kinit, @function

.global _start
.type _start, @function

_start:
	mov $stack_top, %esp

	push %eax   /* magic value */
	push %ebx   /* multiboot struct addr */
	call kinit

	cli
1:	hlt
	jmp 1b
.size _start, . - _start