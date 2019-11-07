/*
	Multiboot Constants
*/
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

/*
	General Constants
*/
.set STACK_SIZE, 16384          /* 16 kb */
 
/* 
	Multiboot Header
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
	Setup Stack
*/
.section .bss
.align 16
stack_bottom:
.skip STACK_SIZE
stack_top:

.section .text

.global _start
.type _start, @function

_start:
	mov $stack_top, %esp

	call kinit

	cli
1:	hlt
	jmp 1b

// Size of _start function
.size _start, . - _start