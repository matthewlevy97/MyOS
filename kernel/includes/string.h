#pragma once

#include <stddef.h>

//TODO: Add memmove(), memcmp()

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

char *reverse(char * restrict str, size_t n);