# Kernel
- Kernel currently implements:
	- Paging
	- KMalloc
- Checkout TODO.md for current list of features to implement

# Memory Layout
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFF7FFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (4MiB)
