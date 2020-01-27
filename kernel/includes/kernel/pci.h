#pragma once

#include <stddef.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_ADDRESS_ENABLE_BIT 0x80000000

/**
 * @brief      General header information about a device located on the PCI bus
 */
struct pci_device {
	uint8_t bus;
	uint8_t device;
	
	uint16_t device_id;
	uint16_t vendor_id;
	uint16_t status;
	uint16_t command;
	
	union {
		uint32_t device_class_line;
		struct {
			uint8_t  revision_id;
			uint8_t  programming_interface;
			uint8_t  device_subclass;
			uint8_t  device_class;
		};
	};
	
	union {
		uint32_t _header_type_line;
		struct {
			uint8_t  cache_line_size;
			uint8_t  latency_timer;
			uint8_t  header_type;
			uint8_t  bist;
		};
	};
};

/**
 * @brief      PCI Header offsets
 */
enum pci_header_offsets {
	// General Header Fields
	PCI_DEVICE_ID   = 0x0,
	PCI_VENDOR_ID   = 0x2,
	PCI_STATUS      = 0x4,
	PCI_COMMAND     = 0x6,
	PCI_CLASS       = 0x8,
	PCI_HEADER      = 0xC,
	
	// Header Type 0 Fields
	PCI_TYPE_0_BASE_ADDRESS_REGISTERS_0 = 0x10,
	PCI_TYPE_0_BASE_ADDRESS_REGISTERS_1 = 0x14,
	PCI_TYPE_0_BASE_ADDRESS_REGISTERS_2 = 0x18,
	PCI_TYPE_0_BASE_ADDRESS_REGISTERS_3 = 0x1C,
	PCI_TYPE_0_BASE_ADDRESS_REGISTERS_4 = 0x20,
	PCI_TYPE_0_BASE_ADDRESS_REGISTERS_5 = 0x24,
};

/**
 * Set the precision level when searching for PCI devices
 */
typedef enum {
	PRECISION_REVISION_ID,
	PRECISION_PROGRAMMING_INTERFACE,
	PRECISION_SUBCLASS,
	PRECISION_CLASS,
	PRECISION_NONE
} pci_scan_precision_t;

void pci_init();

void pci_scan_for_device_class(uint32_t device_class, pci_scan_precision_t precision,
	void (*scan_callback)(struct pci_device));

void pci_get_device_header(uint8_t bus, uint8_t device, struct pci_device *pci_device);

uint16_t pci_get_device_id(uint8_t bus, uint8_t device);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device);
uint32_t pci_get_class(uint8_t bus, uint8_t device);
uint8_t  pci_get_header_type(uint8_t bus, uint8_t device);

uint32_t pci_read32(uint8_t bus, uint8_t device, uint8_t function, enum pci_header_offsets offset);