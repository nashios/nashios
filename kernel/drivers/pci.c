/**
 * @file kernel/drivers/pci.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Peripheral Component Interconnect
 * @version 0.1
 * @date 2022-06-06
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/drivers/pci.h>
#include <kernel/io.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <stdint.h>

static struct dlist_head pci_list;

struct pci_device *pci_get_device(uint16_t vendor_id, uint16_t device_id)
{
    struct pci_device *device;
    dlist_foreach_entry(device, &pci_list, list)
    {
        if (device->vendor_id == vendor_id && device->device_id == device_id)
            return device;
    }
    return NULL;
}

uint32_t pci_read(uint32_t address, uint8_t offset)
{
    outl(PCI_ADDR_PORT, address | (offset & 0xFC));
    return inl(PCI_VALUE_PORT);
}

uint32_t pci_get_address(uint8_t bus, uint8_t slot, uint8_t function)
{
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunction = (uint32_t)function;

    return (lbus << 16) | (lslot << 11) | (lfunction << 8) | 0x80000000;
}

uint16_t pci_get_header_type(uint32_t address)
{
    uint32_t value = pci_read(address, 0xC);
    return (value >> 16) & 0xFF;
}

uint16_t pci_get_vendor_id(uint32_t address)
{
    uint32_t value = pci_read(address, 0x0);
    return value & 0xFFFF;
}

uint16_t pci_get_device_id(uint32_t address)
{
    uint32_t value = pci_read(address, 0x0);
    return (value >> 16) & 0xFFFF;
}

uint16_t pci_get_class_code(uint32_t address)
{
    uint32_t value = pci_read(address, 0x8);
    return (value >> 24) & 0xFF;
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t address = pci_get_address(bus, device, function);
    uint16_t vendor_id = pci_get_vendor_id(address);

    if (PCI_VENDOR_IS_INVALID(vendor_id))
        return;

    uint32_t class_code = pci_get_class_code(address);
    if (class_code == PCI_BRIDGE_CLASS_CODE)
        return;

    struct pci_device *pci_device = calloc(1, sizeof(struct pci_device));
    pci_device->address = address;
    pci_device->vendor_id = vendor_id;
    pci_device->device_id = pci_get_device_id(address);
    pci_device->bar0 = pci_read(address, PCI_BAR0);
    pci_device->bar1 = pci_read(address, PCI_BAR1);
    pci_device->bar2 = pci_read(address, PCI_BAR2);
    pci_device->bar3 = pci_read(address, PCI_BAR3);
    pci_device->bar4 = pci_read(address, PCI_BAR4);
    pci_device->bar5 = pci_read(address, PCI_BAR5);

    dlist_add_tail(&pci_device->list, &pci_list);

    printf("PCI: Device address = 0x%x, id = 0x%x, vendor = 0x%x\n", pci_device->address, pci_device->device_id,
           pci_device->vendor_id);
}

void pci_scan_bus(uint8_t bus)
{
    for (uint8_t device = 0; device < 32; device++)
    {
        uint32_t address = pci_get_address(bus, device, 0);
        uint16_t type = pci_get_header_type(address);
        if (PCI_IS_MULTI_FUNC(type))
        {
            for (uint8_t function = 0; function < 7; function++)
                pci_check_function(bus, device, function);
        }
        else
            pci_check_function(bus, device, 0);
    }
}

void pci_scan()
{
    uint32_t address = pci_get_address(0, 0, 0);
    uint16_t type = pci_get_header_type(address);
    if (PCI_IS_MULTI_FUNC(type))
    {
        for (uint8_t function = 0; function < 8; function++)
        {
            uint16_t vendor_id = pci_get_vendor_id(pci_get_address(0, 0, function));
            if (PCI_VENDOR_IS_INVALID(vendor_id))
                break;

            pci_scan_bus(function);
        }
    }
    else
        pci_scan_bus(0);
}

void pci_init()
{
    dlist_head_init(&pci_list);

    pci_scan();

    printf("PCI: Initialized\n");
}
