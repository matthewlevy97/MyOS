# TODO
- Create driver template
- Implement paging
	- Get an unused page (physical)
	- Implement page fault handler
- Implement palloc
	- Create bitmap for page tracking
	- Get total physical memory in system from multiboot
	- Disqualify kernel pages, multiboot page, and initial heap init pages from bitmap
	- Setup after kmalloc
- Implement heap storage
	- kmalloc
	- slab storage
- Implement filesystem
	- abstract code between low level and high level FS functionality

# Memory Layout
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFFBFFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (4MiB)
