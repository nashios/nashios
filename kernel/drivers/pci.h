#pragma once

#include <kernel/dlist.h>
#include <stdint.h>

struct pci_device
{
    uint16_t device_id;
    uint16_t vendor_id;
    struct dlist_head list;
};

void pci_init();
struct pci_device *pci_find_device(uint16_t device_id, uint16_t vendor_id);