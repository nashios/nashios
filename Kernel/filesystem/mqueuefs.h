#pragma once

#include <stdint.h>

struct mqueuefs_inode
{
    uint32_t key;
};

void mqueuefs_init();
