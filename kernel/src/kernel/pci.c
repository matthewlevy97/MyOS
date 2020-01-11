#include <kpanic.h>
#include <kprint.h>
#include <pci.h>
#include <portio.h>

/**
 * @brief      Initialize the PCI bus
 */
void pci_init()
{
}

/**
 * @brief      Scans for PCI devices matching the device class with a given precision level.
 *
 * @param[in]  device_class   The device class to match
 * @param[in]  precision      The precision level to match the device_class with
 * @param[in]  scan_callback  The function to call for each match (arguments: )
 */
void pci_scan_for_device_class(uint32_t device_class, pci_scan_precision_t precision,
	void (*scan_callback)(struct pci_device))
{
	struct pci_device pci_device;
	uint32_t dev_class;

	for(uint32_t bus = 0; bus < 256; bus++) {
		for(uint32_t device = 0; device < 32; device++) {
			if(pci_get_vendor_id(bus, device) != 0xFFFF) {

				switch(precision)
				{
				case PRECISION_REVISION_ID:
					// Do not clear anything
					dev_class = pci_get_class(bus, device);
					break;
				case PRECISION_PROGRAMMING_INTERFACE:
					// Clear out revision ID
					dev_class = pci_get_class(bus, device) & ~((1 << 8) - 1);
					break;
				case PRECISION_SUBCLASS:
					// Clear out the programming interface
					dev_class = pci_get_class(bus, device) & ~((1 << 16) - 1);
					break;
				case PRECISION_CLASS:
					// Clear out the subclass
					dev_class = pci_get_class(bus, device) & ~((1 << 24) - 1);
					break;
				default:
					// PRECISION_NONE lands here
					dev_class = device_class;
				}

				if(dev_class == device_class) {
					pci_get_device_header(bus, device, &pci_device);
					scan_callback(pci_device);
				}
			}
		}
	}
}

/**
 * @brief      Gets the general header for a PCI device
 *
 * @param[in]  bus         The bus number to read from
 * @param[in]  device      The device ID to read into
 * @param      pci_device  Pointer to the pci_device structure to save header to
 */
void pci_get_device_header(uint8_t bus, uint8_t device, struct pci_device *pci_device)
{
	uint32_t tmp;

	// Set location information
	pci_device->bus = bus;
	pci_device->device = device;

	// Set device id and vendor id
	pci_device->device_id = pci_get_device_id(bus, device);
	pci_device->vendor_id = pci_get_vendor_id(bus, device);

	// Set status and command
	tmp = pci_read32(bus, device, 0, PCI_STATUS);
	pci_device->status = tmp >> 16;
	pci_device->command = tmp & 0xFFFF;

	// Set class, subclass, programming_interface, and revision ID
	pci_device->device_class_line = pci_read32(bus, device, 0, PCI_CLASS);

	// Set bist, header type, latency timer, cache line size
	pci_device->_header_type_line = pci_read32(bus, device, 0, PCI_HEADER);
}

/**
 * @brief      Gets the device ID for a PCI device
 *
 * @param[in]  bus       The bus number to read from
 * @param[in]  device    The device ID to read into
 *
 * @return     The device ID
 */
inline uint16_t pci_get_device_id(uint8_t bus, uint8_t device)
{
	return pci_read32(bus, device, 0, PCI_DEVICE_ID) >> 16;
}

/**
 * @brief      Gets the vendor ID for a PCI device
 *
 * @param[in]  bus       The bus number to read from
 * @param[in]  device    The device ID to read into
 *
 * @return     The vendor ID
 */
inline uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device)
{
	return pci_read32(bus, device, 0, PCI_VENDOR_ID) & 0xFFFF;
}

/**
 * @brief      Gets the class line for a PCI device
 *
 * @param[in]  bus       The bus number to read from
 * @param[in]  device    The device ID to read into
 *
 * @return     The class line (32-bit value containing Class, Subclass, Programming Interface, and Revision ID)
 */
inline uint32_t pci_get_class(uint8_t bus, uint8_t device)
{
	return pci_read32(bus, device, 0, PCI_CLASS);
}

/**
 * @brief      Gets the header type for a PCI device
 *
 * @param[in]  bus       The bus number to read from
 * @param[in]  device    The device ID to read into
 *
 * @return     The header type
 */
inline uint8_t pci_get_header_type(uint8_t bus, uint8_t device)
{
	return pci_read32(bus, device, 0, PCI_HEADER) & 0xFF;
}

/**
 * @brief      Reads a 32bit value from the PCI bus
 *
 * @param[in]  bus       The bus number to read from
 * @param[in]  device    The device ID to read into
 * @param[in]  function  The function number for the device
 * @param[in]  offset    The offset into the device header
 *
 * @return     { description_of_the_return_value }
 */
uint32_t pci_read32(uint8_t bus, uint8_t device, uint8_t function, enum pci_header_offsets offset)
{
	uint32_t address;

	address = PCI_ADDRESS_ENABLE_BIT;
	address |= (((uint32_t)bus) << 16) | (((uint32_t)device) << 11);
	address |= (((uint32_t)function) << 8) | (((uint8_t)offset) & 0xFC);

	out32(PCI_CONFIG_ADDRESS, address);

	// (offset & 2) * 8 gets the upper or lower 16bits of the 32bit value returned
	return in32(PCI_CONFIG_DATA);
}