#define ASM_FILE 1
#include <multiboot/multiboot2.h>

.extern _start
.type _start, @function

/* 
	Multiboot Header
*/
.section .multiboot
.align 4

multiboot_header:

/*
	Multiboot Header Magic
*/
.long MULTIBOOT2_HEADER_MAGIC
.long MULTIBOOT_ARCHITECTURE_I386
/* Header Length */
.long multiboot_header_end - multiboot_header
/* Checksum */
.long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))

/*
	End Of Multiboot
*/
.short MULTIBOOT_HEADER_TAG_END
.short 0
.long 8
multiboot_header_end:

#undef ASM_FILE