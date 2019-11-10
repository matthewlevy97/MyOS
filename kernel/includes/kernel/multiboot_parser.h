#pragma once

#include <stddef.h>
#include <kernel/multiboot2.h>

void *multiboot_get_tag(void *mb_header, uint32_t tag_number);