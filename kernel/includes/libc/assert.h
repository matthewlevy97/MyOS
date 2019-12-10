#pragma once

#include <stddef.h>

void __assert(char *condition, char *file_name, size_t line_number);

#define ASSERT(condition) (void)((condition) || (__assert(#condition, __FILE__, __LINE__),0))