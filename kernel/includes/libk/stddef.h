#pragma once

/*
 * Easy calculating of sizes
*/
#define KB (1 << 10)
#define MB (1 << 20)

#define NULL ((void*)0)

typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef unsigned int size_t;
typedef signed int   ssize_t;

typedef enum {
	true  = 1,
	false = 0
} bool;