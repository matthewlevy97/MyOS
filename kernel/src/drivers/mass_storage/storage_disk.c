#include <pci.h>
#include <string.h>
#include <mass_storage/storage_disk.h>
#include <mass_storage/SATA/sata.h>

static disk_handler_init_t disk_handler_init_functions[MASS_STORAGE_SUBCLASS_NUMBER];
static struct disk_handler disk_handlers[DISK_MAX_NUMBER_HANDLERS];
static uint32_t disk_handler_counter;

static void pci_scan_callback(struct pci_device device);

/**
 * @brief      Initialize all mass storage drivers for devices found on PCI bus
 *
 * @return     Return 0 for success
 */
int disk_init()
{
	disk_handler_counter = 0;
	memset(disk_handlers, 0, sizeof(disk_handlers));

	// Add sata controller
	for(uint32_t i = 0; i < MASS_STORAGE_SUBCLASS_NUMBER; i++)
		disk_handler_init_functions[i] = NULL;

	disk_handler_init_functions[MASS_STORAGE_SATA_CONTROLLER] = sata_init;

	pci_scan_for_device_class(0x01000000, PRECISION_CLASS, pci_scan_callback);

	return 0;
}
MODULE_INIT(disk_init);

/**
 * @brief      Adds disk_handler to handler list
 *
 * @param[in]  handler  The handler to add
 *
 * @return     True if handler added successfully
 */
bool disk_add_handler(struct disk_handler handler)
{
	if(disk_handler_counter >= DISK_MAX_NUMBER_HANDLERS)
		return false;

	memcpy((void*)(disk_handlers + disk_handler_counter), (void*)&handler, sizeof(handler));
	disk_handler_counter++;
	return true;
}

int32_t disk_read(uint32_t disk_number, void *buffer, uint32_t read_len, uint32_t offset)
{
	// TODO: Set errno that disk number too large
	if(disk_number >= DISK_MAX_NUMBER_HANDLERS)
		return -1;
	
	// TODO: Mark errno that this change was done
	if((int32_t)read_len < 0)
		return -1;

	// TODO: Disk number too large
	if(disk_number >= disk_handler_counter)
		return -1;
	
	if(disk_handlers[disk_number].disk_read) {
		return disk_handlers[disk_number].disk_read(disk_handlers[disk_number].data, buffer, offset, read_len);
	}

	// TODO: Set errno that no handler found for desired disk
	return -1;
}

int32_t disk_write(uint32_t disk_number, void *buffer, uint32_t write_len, uint32_t offset)
{
	// TODO: Set errno that disk number too large
	if(disk_number >= DISK_MAX_NUMBER_HANDLERS)
		return -1;

	// TODO: Mark errno that this change was done
	if((int32_t)write_len > 0)
		return -1;

	// TODO: Disk number too large
	if(disk_number >= disk_handler_counter)
		return -1;

	if(disk_handlers[disk_number].disk_write) {
		return disk_handlers[disk_number].disk_write(disk_handlers[disk_number].data, buffer, offset, write_len);
	}

	// TODO: Set errno that no handler found for desired disk
	return -1;
}

void disk_info(uint32_t disk_number, struct disk_info *info)
{
	// TODO: Set errno that disk number too large
	if(disk_number >= DISK_MAX_NUMBER_HANDLERS)
		return;

	// TODO: How to handle when info is NULL
	if(info == NULL)
		return;
	
	// TODO: Disk number too large
	if(disk_number >= disk_handler_counter)
		return;

	if(disk_handlers[disk_number].disk_info) {
		disk_handlers[disk_number].disk_info(disk_handlers[disk_number].data, info);
	}

	// TODO: Set errno that no handler found for desired disk
	return;
}

/**
 * @brief      Callback for pci_scan_for_device_class() that handles calling the init functions for each subclass
 *
 * @param[in]  device  The PCI device data structure
 */
static void pci_scan_callback(struct pci_device device)
{
	// Map to correct init function
	if(device.device_subclass > MASS_STORAGE_SUBCLASS_NUMBER &&
		device.device_subclass == MASS_STORAGE_OTHER_ACTUAL_CONTROLLER) {
		device.device_subclass = MASS_STORAGE_OTHER_CONTROLLER;
	}

	// Call correct init function
	if(disk_handler_init_functions[device.device_subclass]) {
		disk_handler_init_functions[device.device_subclass](device);
	}
}