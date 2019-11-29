#pragma once

#include <stddef.h>

size_t itoa(char *buffer, size_t buffer_len, int value, size_t base);

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);