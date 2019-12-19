#pragma once

#include <stddef.h>

#define VGA_BUFFER_ADDRESS 0xC03FF000

#define CONSOLE_COLOR_CYAN  "\x1b[35m"
#define CONSOLE_COLOR_RED   "\x1b[31m"
#define CONSOLE_COLOR_GREEN "\x1b[32m"
#define CONSOLE_COLOR_RESET "\x1b[0m"

#define KPRINT_DEBUG   "[*] "
#define KPRINT_ERROR   "[-] "
#define KPRINT_SUCCESS "[+] "

void kprint_init();

void enable_serial_output();
void disable_serial_output();

void enable_vga_output();
void disable_vga_output();

void dump_hex(int32_t *hex_bytes, uint32_t number_of_bytes);

int kprintf(const char *format, ...);
int ksprintf(char *str, const char *format, ...);
int ksnprintf(char *str, size_t size, const char *format, ...);