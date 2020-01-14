#include <mass_storage/SATA/sata.h>
#include <kprint.h>

int sata_init()
{
	kprintf("LOADED SATA\n");
	return 0;
}

int sata_exit()
{
	kprintf("UNLOADED SATA\n");
	return 0;
}