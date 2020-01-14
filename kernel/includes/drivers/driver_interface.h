#pragma once

#include <macros.h>

typedef int (*initcall_t)(void);

#define __define_initcall(fn) \
	static initcall_t __used __attribute__((__section__(".initcall"))) __initcall_##fn = fn;
	
#define __define_exitcall(fn) \
	static initcall_t __used __attribute__((__section__(".exitcall"))) __initcall_##fn = fn;

#define MODULE_INIT(fn) __define_initcall(fn)
#define MODULE_EXIT(fn) __define_exitcall(fn)