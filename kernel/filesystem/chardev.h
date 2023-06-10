#pragma once

#include <kernel/api/posix/sys/types.h>
#include <kernel/filesystem/virtual.h>
#include <st/dlist.h>

struct chardev
{
    dev_t dev;
    uint32_t major;
    uint32_t baseminor;
    uint32_t minorct;
    struct dlist_head list;
    struct vfs_file_op *op;
};

extern struct vfs_file_op g_chardev_fop;

void chardev_init();
int chardev_set(struct chardev *device);
