# Kernel
- Kernel currently implements:
	- Paging... wow, feels like more should exist for this amount of code
- Checkout TODO.md for current list of features to implement

# Memory Layout
- 0xC0000000 -> 0xC07FFFFF == kernel memory (8MiB)
- 0xC0800000 -> 0xFFBFFFFF == heap memory
- 0xFFC00000 -> 0xFFFFFFFF == recursive paging entries (4MiB)
