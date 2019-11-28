# TODO
- Create driver template
- Implement paging
	- Get an unused page (physical)
	- Implement page fault handler
- Implement heap storage
	- kmalloc
	- slab storage
	- eternal storage (Stuff that will NEVER be free'd)
- Implement filesystem
	- abstract code between low level and high level FS functionality

# Memory Layout
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFFBFFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (4MiB)
