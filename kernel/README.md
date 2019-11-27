# TODO
- Create driver template
- Implement paging
	- Get a page (physical)
	- Map Physical page -> Virtual page
	- Get Physical page from Virtual page
- Implement heap storage
	- kmalloc
	- slab storage
	- eternal storage (Stuff that will NEVER be free'd)
- Implement filesystem
	- abstract code between low level and high level FS functionality

# Idea
- When setting up ISR, push 3 things:
	- error code
	- ISR number
	- ISR function handler address

# Memory Layout
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFFBFFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (1MiB)