#pragma once

#include <stddef.h>

void kprint_init();

int kprintf(const char *format, ...);
int ksprintf(char *str, const char *format, ...);
int ksnprintf(char *str, size_t size, const char *format, ...);