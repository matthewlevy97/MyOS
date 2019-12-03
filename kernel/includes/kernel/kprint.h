#pragma once

#include <stddef.h>

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

int kprintf(const char *format, ...);
int ksprintf(char *str, const char *format, ...);
int ksnprintf(char *str, size_t size, const char *format, ...);