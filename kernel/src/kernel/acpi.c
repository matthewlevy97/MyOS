#include <acpi.h>
#include <kpanic.h>

static RSDP_VERSION rsdp_2_is_valid(struct root_system_descriptor_pointer_2 *rsdp);
static RSDP_VERSION rsdp_is_valid(struct root_system_descriptor_pointer *rsdp);

void acpi_init(struct multiboot_tag_new_acpi *acpi_tag)
{
	switch(rsdp_is_valid((struct root_system_descriptor_pointer*)acpi_tag->rsdp)) {
	case RSDP_1:
	case RSDP_2:
		break;
	case RSDP_INVALID:
		kpanic("Invalid RSDP structure");
		break;
	}
}

/**
 * @brief      Validate RSDP version 1 or 2
 *
 * @param      rsdp  The rsdp
 */
static RSDP_VERSION rsdp_is_valid(struct root_system_descriptor_pointer *rsdp)
{
	uint32_t checksum;
	uint8_t *ptr;

	if(rsdp->revision)
		return rsdp_2_is_valid((struct root_system_descriptor_pointer_2*)rsdp);

	checksum = 0;
	ptr = (uint8_t*)rsdp;
	for(uint32_t i = 0; i < sizeof(struct root_system_descriptor_pointer); i++) {
		checksum += ptr[i];
	}

	return (checksum & 0xFF) == 0x00 ? RSDP_1 : RSDP_INVALID;
}

/**
 * @brief      Validate RSDP version 2
 *
 * @param      rsdp  Pointer to the Root System Descriptor Pointer
 */
static RSDP_VERSION rsdp_2_is_valid(struct root_system_descriptor_pointer_2 *rsdp)
{
	uint32_t checksum;
	uint8_t *ptr;

	checksum = 0;
	ptr = (uint8_t*)rsdp;
	for(uint32_t i = 0; i < sizeof(struct root_system_descriptor_pointer_2); i++) {
		checksum += ptr[i];
	}

	return (checksum & 0xFF) == 0x00 ? RSDP_2 : RSDP_INVALID;
}