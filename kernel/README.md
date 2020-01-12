# Kernel
- Kernel currently implements:
	- Paging
	- KMalloc (Buddy based allocator)
	- Priority based, Preemptive multitasking
- Checkout TODO.md for current list of features to implement

# Memory Layout
- 0x40000000 -> 0x7FFFFFFF == program (non-kernel)
- 0x80000000 -> 0xBFFFFFFF == program 2 (non-kernel) (used when creating a new process)
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFF7FFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (4MiB)
