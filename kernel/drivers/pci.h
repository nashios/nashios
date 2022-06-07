/**
 * @file kernel/drivers/pci.h
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
#pragma once

#define PCI_ADDR_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC

#define PCI_MULTI_FUNC_DEVICE 0x80
#define PCI_INVALID_VENDOR_ID 0xFFFF
#define PCI_BRIDGE_CLASS_CODE 0x06

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24

#define PCI_IS_MULTI_FUNC(type) (type & PCI_MULTI_FUNC_DEVICE)
#define PCI_VENDOR_IS_INVALID(vendor) (vendor == PCI_INVALID_VENDOR_ID)

#include <kernel/dlist.h>
#include <stdint.h>

struct pci_device
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint32_t address;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    struct dlist_head list;
};

void pci_init();
