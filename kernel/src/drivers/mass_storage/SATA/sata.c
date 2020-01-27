#include <kprint.h>
#include <string.h>

#include <mass_storage/storage_disk.h>
#include <mass_storage/SATA/sata.h>

int sata_init(struct pci_device device)
{
	void * data;

	data = NULL;
	struct disk_handler handler = {
		.disk_read  = sata_read,
		.disk_write = sata_write,
		.disk_info  = sata_info,
		.data       = data,
	};

	disk_add_handler(handler);
	
	kprintf("Found SATA Device (BUS: %d, DEVICE: %d)\n", device.bus, device.device);
	return 0;
}

int32_t sata_read(void *data, void *buffer, uint32_t offset, uint32_t size)
{
	// XXX: Testing write
	memcpy(buffer, "TESTING!", 8);
	return 8;
}

int32_t sata_write(void *data, void *buffer, uint32_t offset, uint32_t size)
{
	return 0;
}

void sata_info(void *data, struct disk_info *info)
{
}