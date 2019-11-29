# TODO
- Create driver template
- Implement paging
	- Get an unused page (physical)
	- Implement page fault handler
- Implement palloc
	- Get total physical memory in system from multiboot
- Implement heap storage
	- kmalloc
	- slab storage
- Implement filesystem
	- abstract code between low level and high level FS functionality
- Errno for libk functions
- Improve kpanic()
	- Redo definition to kpanic(const char fmt[], ...)
	- Have kpanic print file + line number of where it was called
- Implement multitasking
- Fix page permissions for kernel segments
	- rodata is read-only
	- text is executable
	- etc

# Memory Layout
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFFBFFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (4MiB)
