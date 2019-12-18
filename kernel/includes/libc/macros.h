#pragma once

#define FUNCTION_NO_RETURN __attribute__((noreturn))

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CODE_AT __FILE__ ":" TOSTRING(__LINE__)