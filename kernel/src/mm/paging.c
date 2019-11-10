#include <kernel/mm/paging.h>
#include <kernel/kprint.h>

void paging_init(uint32_t paging_directory)
{
	uint32_t * ptr = paging_directory;
	kprintf(KPRINT_DEBUG "Paging Directory: %d\n", paging_directory);

	for(int i = 0; i < 1024; i++)
		if(ptr[i]) kprintf(KPRINT_DEBUG "Paging Directory: %d %d\n", ptr[i] & 0xFFFFF000, i);

	// I'm cheating here and know 768 is populated with kernel data
	ptr = ptr[768] + 0xC0000000;
	for(int i = 0; i < 1024; i++)
		if(ptr[i]) kprintf(KPRINT_DEBUG "Page Table: %x %d\n", ptr[i] & 0xFFFFF000, i);
}