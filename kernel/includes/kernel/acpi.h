#pragma once

#include <stddef.h>
#include <multiboot/multiboot2.h>

typedef enum {
	RSDP_INVALID,
	RSDP_1,
	RSDP_2	
} RSDP_VERSION;

struct root_system_descriptor_pointer {
	uint8_t signature[8];
	uint8_t checksum;
	uint8_t OEMID[6];
	uint8_t revision;
	uint32_t rsdt_address;
} __attribute__ ((packed));

struct root_system_descriptor_pointer_2 {
	struct root_system_descriptor_pointer version1;
	
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __attribute__ ((packed));

void acpi_init(struct multiboot_tag_new_acpi *acpi_tag);