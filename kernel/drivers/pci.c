#include <kernel/drivers/pci.h>
#include <kernel/io.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <stdint.h>

#define PCI_INVALID_VENDOR 0xFFFF

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC

#define PCI_MULTI_FUNC_DEVICE 0x80

#define PCI_CLASS_CODE_BRIDGE_DEVICE 0x06

#define PCI_SUBCLASS_PCI_PCI_BRIDGE 0x04

struct dlist_head g_pci_device_list = {};

void pci_scan_bus(uint8_t bus);

uint32_t pci_read_field(uint32_t address, uint8_t offset)
{
    io_outl(PCI_ADDRESS_PORT, address | (offset & 0xFC));
    return io_inl(PCI_VALUE_PORT);
}

uint32_t pci_get_address(uint8_t bus, uint8_t slot, uint8_t function)
{
    uint32_t p_bus = (uint32_t)bus;
    uint32_t p_slot = (uint32_t)slot;
    uint32_t p_function = (uint32_t)function;
    return (p_bus << 16) | (p_slot << 11) | (p_function << 8) | 0x80000000;
}

uint16_t pci_get_vendor_id(uint32_t address)
{
    uint32_t value = pci_read_field(address, 0x00);
    return value & 0xFFFF;
}

uint16_t pci_get_device_id(uint32_t address)
{
    uint32_t value = pci_read_field(address, 0x00);
    return (value >> 16) & 0xFFFF;
}

uint16_t pci_get_secondary_bus(uint32_t address)
{
    uint32_t value = pci_read_field(address, 0x18);
    return (value >> 8) & 0xFF;
}

uint16_t pci_get_class_code(uint32_t address)
{
    uint32_t value = pci_read_field(address, 0x08);
    return (value >> 24) & 0xFF;
}

uint16_t pci_get_subclass_code(uint32_t address)
{
    uint32_t value = pci_read_field(address, 0x08);
    return (value >> 16) & 0xFF;
}

uint16_t pci_get_header_type(uint32_t address)
{
    uint32_t value = pci_read_field(address, 0x0C);
    return (value >> 16) & 0xFF;
}

uint32_t pci_read_bar(struct pci_device *device, enum pci_bar bar)
{
    uint32_t address = pci_read_field(device->address, PCI_BAR0);
    return address & 0xFFFFFFF0;
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t address = pci_get_address(bus, device, function);
    uint16_t vendor_id = pci_get_vendor_id(address);
    if (vendor_id == PCI_INVALID_VENDOR)
        return;

    uint16_t class_code = pci_get_class_code(address);
    if (class_code == PCI_CLASS_CODE_BRIDGE_DEVICE)
        return;

    struct pci_device *pci_device = (struct pci_device *)calloc(1, sizeof(struct pci_device));
    pci_device->device_id = pci_get_device_id(address);
    pci_device->vendor_id = vendor_id;
    pci_device->class_code = class_code;
    pci_device->subclass_code = pci_get_subclass_code(address);
    pci_device->address = address;

    dlist_add_tail(&pci_device->list, &g_pci_device_list);
    printf("PCI: Added device id = 0x%x, vendor id = 0x%x, bus = 0x%x, device = 0x%x, function = 0x%x\n",
           pci_device->device_id, pci_device->vendor_id, bus, device, function);
}

void pci_scan_bus(uint8_t bus)
{
    for (uint8_t device = 0; device < 32; device++)
    {
        uint32_t address = pci_get_address(bus, device, 0x00);
        uint16_t header_type = pci_get_header_type(address);
        if ((header_type & PCI_MULTI_FUNC_DEVICE) != 0)
        {
            for (uint8_t function = 0; function < 7; function++)
                pci_check_function(bus, device, function);
        }
        else
            pci_check_function(bus, device, 0);
    }
}

struct pci_device *pci_find_device(uint16_t device_id, uint16_t vendor_id)
{
    struct pci_device *device;
    dlist_for_each_entry(device, &g_pci_device_list, list)
    {
        if (device->device_id == device_id && device->vendor_id == vendor_id)
            return device;
    }
    return NULL;
}

bool pci_is_disabled() { return dlist_empty(&g_pci_device_list); }

void pci_init()
{
    dlist_head_init(&g_pci_device_list);

    uint16_t header_type = pci_get_header_type(pci_get_address(0, 0, 0));
    if ((header_type & PCI_MULTI_FUNC_DEVICE) != 0)
    {
        printf("PCI: Detected multiple pci host controllers\n");
        for (uint8_t function = 0; function < 8; function++)
        {
            uint32_t address = pci_get_address(0, 0, function);
            if (pci_get_vendor_id(address) == PCI_INVALID_VENDOR)
                break;

            pci_scan_bus(function);
        }
    }
    else
    {
        printf("PCI: Detected single pci host controller\n");
        pci_scan_bus(0);
    }

    printf("PCI: Initialized\n");
}
