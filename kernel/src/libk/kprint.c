#include <kernel/kprint.h>

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief      Holds information related to the current position of the cursor
 * 				on the screen
 */
struct terminal_screen {
	uint16_t width;
	uint16_t height;

	uint16_t color;

	uint16_t x_pos;
	uint16_t y_pos;
};

static volatile uint16_t      *VGA_BUFFER;
static struct terminal_screen  screen;

static inline int  putchar(const char c);
static inline int  putint(const int i);
static inline int  puthex(const int i);
static inline int  putstring(const char *str);
static        void screen_moveup();
static        void screen_clear();

#if defined(__UNIT_TEST) && defined(__UNIT_TEST_KPRINT)
static void unit_test();
#endif

/**
 * @brief      Initialize the screen and prepare for writing
 */
void kprint_init()
{
	VGA_BUFFER = (volatile uint16_t*)0xB8000;

	screen.width  = 80;
	screen.height = 25;
	screen.color  = 0x07 << 8, // Default: Lightgrey on Black
	screen.x_pos  = 0;
	screen.y_pos  = 0;

	screen_clear();

#if defined(__UNIT_TEST) && defined(__UNIT_TEST_KPRINT)
	unit_test();
#endif
}

/**
 * @brief      Print format string to the screen
 *
 * @param[in]  format     Format string
 * @param[in]  ...        Arguments to use when populating format string
 *
 * @return     Number of bytes printed to screen
 */
int kprintf(const char *format, ...)
{
	char *fmt;
	int bytes_written;
	va_list args;

	fmt = (char*)format;
	bytes_written = 0;

	va_start(args, format);

	while(*fmt) {
		if(*fmt != '%') {
			bytes_written += putchar(*fmt);
		} else {
			++fmt;
			switch(*fmt) {
			case '\0':
				// This is an invalid format string
				break;
			case 'd':
				bytes_written += putint(va_arg(args, int));
				break;
			case 'c':
				bytes_written += putchar(va_arg(args, int));
				break;
			case 'x':
				bytes_written += puthex(va_arg(args, int));
				break;
			case 's':
				bytes_written += putstring(va_arg(args, char *));
				break;
			case '%':
				bytes_written += putchar(*fmt);
				break;
			default:
				break;
			}
		}

		fmt++;
	}

	va_end(args);

	return bytes_written;
}

int ksprintf(char *str, const char *format, ...)
{
	// TODO:
	return 0;
}

int ksnprintf(char *str, size_t size, const char *format, ...)
{
	// TODO:
	return 0;
}

static inline int putchar(const char c)
{
	uint16_t entry;
	bool new_line;

	if(c == '\n') {
		// TODO: Replace ' ' with '\r'. '\r' currently shows odd.
		entry = ((uint16_t)' ') | screen.color;
		new_line = true;
	} else {
		entry = ((uint16_t)c) | screen.color;
		new_line = false;
	}

	*(VGA_BUFFER + (screen.x_pos) + (screen.y_pos * screen.width)) = entry;

	if(++screen.x_pos >= screen.width) {
		screen.x_pos = 0;
		++screen.y_pos;

		if(screen.y_pos >= screen.height) {
			screen_moveup();
			screen.x_pos = 0;
		}
	}

	if(new_line) {
		screen.x_pos = 0;
		++screen.y_pos;

		if(screen.y_pos >= screen.height) {
			screen_moveup();
		}
	}

	return 1;
}

static inline int putint(const int i)
{
	char buf[12];
	size_t bytes_written;

	bytes_written = itoa(buf, sizeof(buf), i, 10);
	if(!bytes_written) {
		return 0;
	}

	return putstring(buf);
}

static inline int puthex(const int i)
{
	char buf[12];
	size_t bytes_written;

	bytes_written = itoa(buf, sizeof(buf), i, 16);
	if(!bytes_written) {
		return 0;
	}

	return putstring(buf);
}

static inline int putstring(const char *str)
{
	/**
	 * TODO: This cannot be the most efficent method for doing this
	 */
	char *ptr;
	int bytes_written;

	ptr = (char*)str;
	bytes_written = 0;

	while(*ptr) {
		putchar(*ptr);
		ptr++;
	}

	return bytes_written;
}

static void screen_moveup()
{
	// Copy up line by line
	uint16_t counter = screen.height;
	uint16_t *current_line = (uint16_t*)(VGA_BUFFER + screen.width);
	uint16_t *next_line    = (uint16_t*)(current_line + screen.width);

	while(counter--) {
		memcpy(current_line, next_line, screen.width * sizeof(uint16_t));
	}
}

static void screen_clear()
{
	memset((void*)VGA_BUFFER, 0, screen.width * screen.height * sizeof(uint16_t));
}

/**
 * @brief      Runs tests to ensure correctness in code
 */
#if defined(__UNIT_TEST) && defined(__UNIT_TEST_KPRINT)
static void unit_test()
{
	kprintf("Testing kprintf:\n");
	kprintf("\tDecimal: %d %d %d\n", 100, 10, -100);
	kprintf("\tCharacter: %c %c\n", '?', 'Z');
	kprintf("\tString: <%s>\n", "Hello, World!");
	kprintf("\tHex: $%x $%x $%x\n", 0xAF, 0xDE, 0xFF);
}
#endif