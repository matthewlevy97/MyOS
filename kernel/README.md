# TODO
- Create driver template
- Implement paging
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