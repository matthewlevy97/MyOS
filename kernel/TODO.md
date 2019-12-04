# TODO
- Create drivers
	- Loadable through GRUB
	- Loadable through FS
- Implement paging
	- Implement page fault handler
		- Create poison pages that will cause panic if hit (i.e. 0xCAFEBABE, 0xDEADBEEF, 0x00000)
		- Lazy loading of non-present pages
- Multiboot
	- Move information to heap, clear page
	- Add function to multiboot_parser() to change the address of the multiboot information
- Implement heap storage
	- kmalloc
		- combine neighboring free'd blocks
	- slab storage
		- look at Linux SLUB allocator
	- expand heap size
- Implement filesystem
	- abstract code between low level and high level FS functionality (VFS)
- Errno for libk functions
- Improve kpanic()
	- Redo definition to kpanic(const char fmt[], ...)
	- Have kpanic print file + line number of where it was called
	- Improve debug output of kpanic ()
- Implement kernel threads
- Fix page permissions for kernel segments
	- rodata is read-only
	- text is executable
	- etc
- Remove hardcoded values in boot.S
	- Use _kernel_offset from linker.ld instead of 0xC0000000