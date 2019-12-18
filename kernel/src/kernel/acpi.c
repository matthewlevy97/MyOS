#include <acpi.h>
#include <kpanic.h>
#include <string.h>

static RSDP_VERSION rsdp_is_valid(struct root_system_descriptor_pointer *rsdp);

void acpi_init(struct multiboot_tag_new_acpi *acpi_tag)
{
	if(rsdp_is_valid((struct root_system_descriptor_pointer*)acpi_tag->rsdp) == RSDP_INVALID)
		kpanic("Invalid RSDP structure");
}

/**
 * @brief      Validate RSDP version 1 or 2
 *
 * @param      rsdp  The rsdp
 */
static RSDP_VERSION rsdp_is_valid(struct root_system_descriptor_pointer *rsdp)
{
	uint32_t checksum, length;
	uint8_t *ptr;

	if(strncmp((char*)(rsdp->signature), RSDP_SIGNATURE, 8))
		return RSDP_INVALID;

	if(rsdp->revision == 2)
		length = sizeof(struct root_system_descriptor_pointer_2);
	else
		length = sizeof(struct root_system_descriptor_pointer);

	checksum = 0;
	ptr = (uint8_t*)rsdp;
	while(length--) {
		checksum += *ptr++;
	}

	return (checksum & 0xFF) == 0x00 ? RSDP_1 : RSDP_INVALID;
}