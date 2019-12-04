#include <assert.h>
#include <kpanic.h>
#include <kprint.h>

/**
 * @brief      Handle a failed assertion.
 * 				Do NOT call this function directly. Only through helper
 * 				macro ASSERT(x)
 *
 * @param      condition    The condition as a string
 * @param      file_name    The file name the assert failed on
 * @param[in]  line_number  The line number the assert failed on
 */
void __assert(char *condition, char *file_name, size_t line_number)
{
	kprintf("Assertion Failed: %s in file %s on line %d\n\0", condition, file_name, line_number);
#ifdef __KERNEL_CODE
	kpanic();
#else
	// TODO:
#endif
}