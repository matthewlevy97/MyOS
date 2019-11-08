#include <stdlib.h>
#include <assert.h>
#include <string.h>

static uint8_t const hex_table[] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};

/**
 * @brief      Write an integer to a buffer as a string.
 * 				String is NULL terminiated. NULL byte is
 * 				NOT included in length written.
 *
 * @param      buffer      The buffer to write the integer to
 * @param[in]  buffer_len  Max length of string buffer
 * @param[in]  value       Integer value to convert to a string
 *
 * @return     Number of bytes written to buffer
 */
size_t itoa(char *buffer, size_t buffer_len, int value, size_t base)
{
	char *ptr;
	uint8_t bottom_digit;
	size_t digits;
	bool negative;

	ptr    = buffer;
	digits = 0;

	if(base > sizeof(hex_table)) {
		return 0;
	}

	// Is this a negative number?
	if(value < 0) {
		negative = true;
		value *= -1;
	} else {
		negative = false;
	}

	if(!value && digits < buffer_len) {
		// This is a special case where intial value == 0
		*ptr++ = hex_table[value]; // Will evaluate to '/0'
		digits++;
	}

	// Convert to string
	for(; digits < buffer_len && value; digits++) {
		bottom_digit = value % base;
		*ptr++ = hex_table[bottom_digit];

		value /= base;
	}

	// Ensure we actually did something
	if(digits) {
		digits--;

		if(negative && digits + 1 < buffer_len) {
			*ptr++ =  '-';
			digits++;
		}

		// Values get inserted into buffer backwords
		buffer = reverse(buffer, digits);

		// Append NULL byte
		if(digits + 1 < buffer_len)
			*ptr = '\0';
		else
			*(--ptr) = '\0';
	}

	return digits;
}