#pragma once

#include <macros.h>

#define kpanic(msg) kpanic_implementation("On Line: " CODE_AT "\n\tMessage: " #msg "\n")

void kpanic_implementation(const char *message);