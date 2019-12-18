#pragma once

#include <stddef.h>

//TODO: Add memmove(), memcmp()

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

char *reverse(char * restrict str, size_t n);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);