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