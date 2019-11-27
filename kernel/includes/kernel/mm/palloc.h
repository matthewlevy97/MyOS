#pragma once

#include <stddef.h>

void palloc_init(uint32_t low_address, uint32_t high_address);
uint32_t palloc_physical();
void palloc_release(uint32_t address);