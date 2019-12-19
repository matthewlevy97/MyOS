#include <acpi.h>
#include <kpanic.h>
#include <kprint.h>
#include <string.h>
#include <mm/paging.h>

static RSDP_VERSION is_rsdp_valid(RSDPv1 *rsdp);
static void parse_rsdt(RSDPv1 *rsdp, const char *identifier);

void acpi_init(struct multiboot_tag_new_acpi *acpi_tag)
{
	RSDPv1 *rsdp;
	RSDP_VERSION version;

	rsdp    = (RSDPv1 *)acpi_tag->rsdp;
	version = is_rsdp_valid(rsdp);
	if(version == RSDP_INVALID)
		kpanic("Invalid RSDP structure");

	kprintf(KPRINT_DEBUG "ACPI - Version: %d - OEMID: '%s'\n", version, rsdp->oemid);

	parse_rsdt(rsdp, "TEST");
}

/**
 * @brief      Validate RSDP version 1 or 2
 *
 * @param      rsdp  The rsdp
 */
static RSDP_VERSION is_rsdp_valid(RSDPv1 *rsdp)
{
	RSDP_VERSION version;
	uint32_t checksum, length;
	uint8_t *ptr;

	// Verify Signature
	if(strncmp(rsdp->signature, RSDP_SIGNATURE, 8))
		return RSDP_INVALID;

	// Check which version this is
	if(rsdp->revision == 2) {
		length = sizeof(RSDPv2);
		version = RSDP_2;
	} else {
		length = sizeof(RSDPv1);
		version = RSDP_1;
	}

	// Checksum should have 0x00 in the lowest byte to be correct
	checksum = 0;
	ptr = (uint8_t*)rsdp;
	while(length--) {
		checksum += *ptr++;
	}

	return (checksum & 0xFF) == 0x00 ? version : RSDP_INVALID;
}

/**
 * @brief      Parses an RSDT to find an entry. Only for ACPI v1.
 *
 * @param      rsdp  The rsdp
 */
static void parse_rsdt(RSDPv1 *rsdp, const char *identifier)
{
	RSDT *rsdt, *tmp;
	uint32_t entries;

	rsdt = (RSDT*)rsdp->rsdt_address;

	// TODO: Map to an actual address
	// TODO: Need code to dish out temporary virtual addresses
	paging_map2(rsdt, (void*)PAGE_ALIGN((uintptr_t)rsdp->rsdt_address), PAGE_PRESENT | PAGE_READ_WRITE, 0);

	kprintf("<%s>\n", rsdt->header.signature);
	entries = (rsdt->header.length - sizeof(struct SDT_Header)) / sizeof(uintptr_t);
	kprintf("%d\n", entries);

	int i = 0;
	while(rsdt->sdt_ptrs[i]) {
		tmp = rsdt->sdt_ptrs[i];
		paging_map2(tmp, (void*)PAGE_ALIGN((uintptr_t)tmp), PAGE_PRESENT | PAGE_READ_WRITE, 0);
		kprintf("%x\n", tmp);
		kprintf("%s\n", tmp->header.signature);
		i++;
		
		paging_unmap(tmp);
	}
}