#pragma once

#include <driver_interface.h>
#include <pci.h>

#define DISK_MAX_NUMBER_HANDLERS 16

/**
 * @brief      Subclasses for mass storage PCI devices
 */
enum mass_storage_subclass {
	MASS_STORAGE_SCSI_BUS_CONTROLLER             = 0,
	MASS_STORAGE_IDE_CONTROLLER                  = 1,
	MASS_STORAGE_FLOPPY_DISK_CONTROLLER          = 2,
	MASS_STORAGE_IPI_BUS_CONTROLLER              = 3,
	MASS_STORAGE_RAID_CONTROLLER                 = 4,
	MASS_STORAGE_ATA_CONTROLLER                  = 5,
	MASS_STORAGE_SATA_CONTROLLER                 = 6,
	MASS_STORAGE_SERIAL_ATTACHED_SCSI_CONTROLLER = 7,
	MASS_STORAGE_NON_VOLATILE_MEMORY_CONTROLLER  = 8,
	MASS_STORAGE_OTHER_CONTROLLER                = 9, // Actually 0x80
	MASS_STORAGE_SUBCLASS_NUMBER                 = 10,
	MASS_STORAGE_OTHER_ACTUAL_CONTROLLER         = 0x80,
};

/**
 * Function type for disk handler init functions
 */
typedef int (*disk_handler_init_t)(struct pci_device);

/**
 * @brief      Holds information about a certain disk
 */
struct disk_info {
	char handler_name[256];
};

/**
 * NOTE: read/write return the number of bytes read/written from the disk
 *  	if size of bytes to read/write > MAX_SIZE_INT32_T:
 *  		throw error
 */

/**
 * @brief      V-Table for disk handler functions
 */
struct disk_handler {
	/**
	 * Read from a disk
	 * 
	 * int32_t bread(void *data, void *buffer, uint32_t offset, uint32_t size)
	 * 
	 * Returns bytes read
	 */
	int32_t (*disk_read)(void*, void*, uint32_t, uint32_t);
	
	/**
	 * Write to the disk
	 * 
	 * int32_t bwrite(void *data, void *buffer, uint32_t offset, uint32_t size)
	 * 
	 * Returns bytes written
	 */
	int32_t (*disk_write)(void*, void*, uint32_t, uint32_t);
	
	/**
	 * Return information about the disk
	 * 
	 * struct disk_info disk_info(void *data, struct disk_info *info)
	 */
	void (*disk_info)(void*, struct disk_info*);
	
	/**
	 * Pointer to data to be used by disk handler for storing data
	 */
	void *data;
};

int disk_init();

bool disk_add_handler(struct disk_handler handler);

int32_t disk_read(uint32_t disk_number, void *buffer, uint32_t read_len, uint32_t offset);
int32_t disk_write(uint32_t disk_number, void *buffer, uint32_t write_len, uint32_t offset);
void disk_info(uint32_t disk_number, struct disk_info *info);