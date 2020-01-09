#pragma once

#include <i686/isr.h>
#include <stddef.h>

#define PIT_CHAN0 0x40 // Channel 0 data port (read/write)
#define PIT_CHAN1 0x41 // Channel 1 data port (read/write)
#define PIT_CHAN2 0x42 // Channel 2 data port (read/write)
#define PIT_CMD   0x43 // Mode/Command register (write only, a read is ignored)

void timer_init(uint32_t frequency);
void timer_interrupt_handler(struct isr_arguments *args);