/*
	General Constants
*/
.set KB, 1024             /* 1kb */
.set STACK_SIZE, 32*KB    /* 32 kb */

/*
	Setup Stack
*/
.section .bootstrap_stack
.align 16

stack_bottom:
	.skip STACK_SIZE
stack_top:

/*
	Setup Page Directory and Page Table 1 (For Kernel)
*/
.section .bss
.align 4096

.global kernel_page_directory
.type kernel_page_directory, @object
kernel_page_directory:
	.skip 4096

/**
*	Page table entries:
*		0->2046 == kernel.bin
*		2047    == VGA buffer
*/
_kernel_page_table:
	.skip 4096
_kernel_page_table_2:
	.skip 4096

_kernel_heap_page_table:
	.skip 4096

.section .text

.extern kinit
.type kinit, @function

.global _start
.type _start, @function

/*
	TODO: Code assumes kernel is less than 3MB. Need to update should it become larger
*/
_start:
	movl $(_kernel_page_table - 0xC0000000), %edi
	movl $0, %esi

	/*
		Map kernel, multiboot header, then VGA buffer
	*/
	movl $1024, %ecx

1:
	/* Only map the kernel here */
	cmpl $(_kernel_start - 0xC0000000), %esi
	jl 2f
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge 3f

	/*
		Map physical address as "present, writable". Note that this maps
		.text and .rodata as writable. Mind security and map them as non-writable.

		TODO: Improve this security wise
	*/
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	/*
		Move to next page
	*/
	addl $4096, %esi
	addl $4, %edi
	loop 1b /* Loop until ecx-- == 0 */

	/*
		If we are here, map into second table
	*/
	movl $(_kernel_page_table_2 - 0xC0000000), %edi
	movl $1023, %ecx
	jmp 1b

3:
	/* Multiboot should be mapped when mapping in entire kernel */
	movl %ebx, %esi
	// Update ebx to new location
	addl $0xC0000000, %ebx

	/* Map VGA buffer to 0xC07FF000 as "present, writable" */
	movl $(0x000B8000 | 0x003), _kernel_page_table_2 - 0xC0000000 + 1023 * 4

	// Map kernel (identity and not)
	movl $(_kernel_page_table - 0xC0000000 + 0x003), kernel_page_directory - 0xC0000000 + 0 * 4
	movl $(_kernel_page_table_2 - 0xC0000000 + 0x003), kernel_page_directory - 0xC0000000 + 1 * 4
	movl $(_kernel_page_table - 0xC0000000 + 0x003), kernel_page_directory - 0xC0000000 + 768 * 4
	movl $(_kernel_page_table_2 - 0xC0000000 + 0x003), kernel_page_directory - 0xC0000000 + 769 * 4

	/*
		Setup initial heap page table
	*/
	movl $(_kernel_heap_page_table - 0xC0000000 + 0x003), kernel_page_directory - 0xC0000000 + 770 * 4

	/*
		Setup recursive mappings
		Kernel Page Directory loaded at address: 0xFFC00000
	*/
	movl $(kernel_page_directory - 0xC0000000 + 0x003), kernel_page_directory - 0xC0000000 + 1023 * 4

	/* Enable paging */
	movl $(kernel_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	/* Jump to higher half */
	lea 4f, %ecx
	jmp *%ecx

4:
	/* Paging setup here (in higher half). Unmap identity mapping */
	movl $0, kernel_page_directory
	movl $0, kernel_page_directory + 4

	/* TLB flush */
	movl %cr3, %ecx
	movl %ecx, %cr3

	/* Setup stack */
	mov $stack_top, %esp
	xor %ebp, %ebp

	/* Enter C land */
	push %eax        /* magic value */
	push %ebx        /* multiboot struct addr */
	call kinit

	cli
5:	hlt
	jmp 5b
.size _start, . - _start