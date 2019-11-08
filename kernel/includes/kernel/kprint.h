#pragma once

#include <stddef.h>

#define KPRINT_DEBUG   "[*] "
#define KPRINT_ERROR   "[-] "
#define KPRINT_SUCCESS "[+] "

void kprint_init();

int kprintf(const char *format, ...);
int ksprintf(char *str, const char *format, ...);
int ksnprintf(char *str, size_t size, const char *format, ...);