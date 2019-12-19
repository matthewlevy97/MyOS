#pragma once

#include <stddef.h>
#include <multiboot/multiboot2.h>

#define RSDP_SIGNATURE "RSD PTR "

typedef enum {
	RSDP_INVALID,
	RSDP_1,
	RSDP_2	
} RSDP_VERSION;

struct SDT_Header {
	char     signature[4];
	uint32_t length;
	uint8_t  revision;
	uint8_t  checksum;
	char     oemid[6];
	char     oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
};

typedef struct root_system_descriptor_table {
	struct SDT_Header header;
	uintptr_t         sdt_ptrs[0];
} RSDT;

typedef struct root_system_descriptor_pointer {
	char     signature[8];
	uint8_t  checksum;
	char     oemid[6];
	uint8_t  revision;
	RSDT*    rsdt_address;
} RSDPv1;

typedef struct root_system_descriptor_pointer_v2 {
	struct root_system_descriptor_pointer version_1;
	
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t  extended_checksum;
	uint8_t  reserved[3];
} RSDPv2;

void acpi_init(struct multiboot_tag_new_acpi *acpi_tag);