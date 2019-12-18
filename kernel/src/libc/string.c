#include <string.h>
#include <stddef.h>
#include <macros.h>

/**
 * TODO: Alot of the functions in here could be improved by juggling
 * 	types. ie. if memcpy length is divisible by uint16_t, use that instead
 * 	of uint8_t.
 */

/**
 * @brief      Calculate the length of a string excluding terminating NULL ('\0') character
 *
 * @param[in]  s     String to determine the length of
 *
 * @return     Length of the string not counting NULL byte
 */
size_t strlen(const char *s)
{
	size_t length;
	length = 0;

	while(*s++) length++;

	return length;
}

/**
 * @brief      Copies n-bytes from src into dst buffer
 *
 * @param      dest  The destination buffer
 * @param[in]  src   The source buffer
 * @param[in]  n     Number of bytes to copy
 *
 * @return     Pointer to destination buffer
 */
void *memcpy(void *dest, const void *src, size_t n)
{
	char *d, *s;
	d = (char*)dest;
	s = (char*)src;

	while(n--) {
		*d++ = *s++;
	}

	return dest;
}

/**
 * @brief      Sets n-bytes in buffer s to byte c
 *             
 * @param      s     Buffer to set bytes
 * @param[in]  c     Byte
 * @param[in]  n     Number of bytes to write
 *
 * @return     { description_of_the_return_value }
 */
void *memset(void *s, int c, size_t n)
{
	char *ptr;

	ptr = s;
	while(n--) {
		*ptr++ = c;
	}

	return s;
}

/**
 * @brief      Reverses a string
 *
 * @param      str   The string to reverse
 * @param[in]  n     The length of the string
 *
 * @return     Pointer to the string (str)
 */
char *reverse(char * restrict str, size_t n)
{
	size_t start, end;
	char tmp;

	start = 0;
	end   = MIN(strlen(str) - 1, n);

	while(start < end) {
		tmp        = str[start];
		str[start] = str[end];
		str[end]   = tmp;

		start++;
		end--;
	}

	return str;
}

int strcmp(const char *s1, const char *s2)
{
	char *a, *b;

	a = (char*)s1;
	b = (char*)s2;

	while(*a) {
		if(*a == *b) {
			a++;
			b++;
		} else if(*a < *b) {
			return -1;
		} else {
			return 1;
		}
	}

	return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	char *a, *b;

	// XXX: If n == 0, return 0. Is this correct behavior?
	if(!n) return 0;

	a = (char*)s1;
	b = (char*)s2;

	while(*a && n--) {
		if(*a == *b) {
			a++;
			b++;
		} else if(*a < *b) {
			return -1;
		} else {
			return 1;
		}
	}

	return 0;
}