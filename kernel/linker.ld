ENTRY(_start)

SECTIONS
{
	/*
		Setup kernel in high memory
	*/
	_kernel_offset = 0xC0000000;
	. = 0xC0100000;

	_kernel_start = .;

	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}

	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
		
		_driver_initcall_start = .;
		*(.initcall)
		_driver_initcall_end = .;

		_driver_exitcall_start = .;
		*(.exitcall)
		_driver_exitcall_end = .;
	}

	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	_kernel_data_end = .;

	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	_kernel_end = .;
}