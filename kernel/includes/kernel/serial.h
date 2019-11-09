#pragma once

#define COM1 0x3F8
#define COM2 0x2F8

void serial_init();

void serial_write(char c);