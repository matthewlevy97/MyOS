.section .text

.global gdt_flush
.type gdt_flush, @function

gdt_flush:
	mov 4(%esp), %eax  // Get the pointer to the GDT, passed as a parameter.
	lgdt (%eax)        // Load the new GDT pointer

	mov $0x10, %ax     // 0x10 is the offset in the GDT to our data segment
	mov %ax, %ds       // Load all data segment selectors
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	jmp $0x08,$.flush  // 0x08 is the offset to our code segment: Far jump!
.flush:
	ret
.size gdt_flush, . - gdt_flush

.global idt_flush
.type idt_flush, @function

idt_flush:
	mov 4(%esp), %eax  // Get the pointer to the IDT, passed as a parameter.
	lidt (%eax)        // Load the IDT pointer.
	ret
.size idt_flush, . - idt_flush

.global tss_flush
.type tss_flush, @function

tss_flush:
	/**
	Load the index of our TSS structure - The index is
	0x28, as it is the 5th selector and each is 8 bytes
	long, but we set the bottom two bits (making 0x2B)
	so that it has an RPL of 3, not zero.
	*/
	mov $0x2B, %ax
   	ltr %ax
	ret