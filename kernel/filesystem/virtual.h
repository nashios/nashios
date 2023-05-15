#pragma once

#include <kernel/dlist.h>

struct vfs_type
{
    const char *name;
    struct dlist_head list;
};

void virtual_fs_init();
int virtual_fs_set_type(struct vfs_type *type);