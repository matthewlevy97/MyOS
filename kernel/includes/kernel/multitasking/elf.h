#pragma once

#include <multitasking/process.h>

// From 0x40000000 -> 0x7FFFFFFF
#define ELF_USER_CODE_BASE_ADDRESS     0x40000000
#define ELF_USER_STACK_BASE_ADDRESS    0x80000000

#define ELF_KERNEL_STACK_BASE_ADDRESS  0xFF800000

// The number of page directory entries a process takes up. -1 because stack really starts at page below 0x80000000
#define ELF_PAGES (GET_PAGE_DIR_INDEX(ELF_USER_STACK_BASE_ADDRESS - ELF_USER_CODE_BASE_ADDRESS - 1))

void elf_load();
void elf_setup_stack(process_t process);