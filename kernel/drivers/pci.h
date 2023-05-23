#pragma once

#include <kernel/dlist.h>
#include <stdint.h>

struct pci_device
{
    uint16_t device_id;
    uint16_t vendor_id;
    uint16_t class_code;
    uint16_t subclass_code;
    uint32_t address;
    struct dlist_head list;
};

enum pci_bar
{
    PCI_BAR0 = 0x10
};

extern struct dlist_head g_pci_device_list;

void pci_init();
bool pci_is_disabled();
uint32_t pci_read_bar(struct pci_device *device, enum pci_bar bar);
struct pci_device *pci_find_device(uint16_t device_id, uint16_t vendor_id);
