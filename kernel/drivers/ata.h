#pragma once

#include <kernel/dlist.h>
#include <stdbool.h>
#include <stdint.h>

struct ata_device
{
    const char *name;
    bool master;
    uint16_t io_base;
    struct dlist_head list;
};

void ata_init();
struct ata_device *ata_find_device(const char *name);
int ata_read(struct ata_device *device, uint32_t lba, uint8_t sectors, uint16_t *buffer);