#include <drivers.h>
#include <driver_interface.h>
#include <stddef.h>

extern uint32_t _driver_initcall_start, _driver_initcall_end, _driver_exitcall_start, _driver_exitcall_end;

/**
 * @brief      Load statically linked drivers
 */
void drivers_init()
{
	initcall_t func;

	for(uint32_t *ptr = &_driver_initcall_start; ptr < &_driver_initcall_end; ptr++) {
		func = (initcall_t)*ptr;
		func();
	}
}

/**
 * @brief      Unload statically linked drivers
 */
void drivers_terminate()
{
	initcall_t func;

	for(uint32_t *ptr = &_driver_exitcall_start; ptr < &_driver_exitcall_end; ptr++) {
		func = (initcall_t)*ptr;
		func();
	}
}