#pragma once

#include <driver_interface.h>
#include <pci.h>

int sata_init(struct pci_device device);

int32_t sata_read(void *data, void *buffer, uint32_t offset, uint32_t size);
int32_t sata_write(void *data, void *buffer, uint32_t offset, uint32_t size);
void sata_info(void *data, struct disk_info *info);