#include <multiboot/multiboot_parser.h>
#include <kpanic.h>
#include <kprint.h>

/**
 * @brief      Finds a tag in the multiboot header
 *
 * @param      mb_header   The multiboot header start address
 * @param[in]  tag_number  The tag number to find in the header
 *
 * @return     Returns a pointer to the tag structure if found, otherwise NULL
 */
void *multiboot_get_tag(void *mb_header, uint32_t tag_number)
{
	struct multiboot_tag *tag;
	char *ptr;
	uint32_t total_size;

	ptr = mb_header;

	total_size = *(uint32_t*)ptr;
	if(((uint32_t*)ptr)[1] != 0x00 || total_size < sizeof(struct multiboot_tag)) {
		kpanic("Invalid multiboot header");
	}

	total_size -= sizeof(uint32_t) * 2;
	ptr += sizeof(uint32_t) * 2;

	tag = (struct multiboot_tag*)ptr;
	while(total_size && tag->type) {
		if(tag->type == tag_number) break;

		// Need to pad to alignment
		ptr += tag->size;
		if(tag->size % MULTIBOOT_HEADER_ALIGN) {
			ptr += 8 - (tag->size % MULTIBOOT_HEADER_ALIGN);
		}
		tag = (struct multiboot_tag*)ptr;
	}

	return (tag->type == tag_number ? tag : NULL);
}