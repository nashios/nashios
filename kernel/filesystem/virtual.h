#pragma once

#include <kernel/api/posix/sys/types.h>
#include <kernel/dlist.h>
#include <kernel/task/scheduler.h>
#include <stdint.h>

#define VFS_BYTES_P_SECTOR 512

struct vfs_file;
struct vfs_file_op
{
    int (*mmap)(struct vfs_file *file, struct process_vm *memory);
};

struct vfs_file
{
    struct vfs_file_op *op;
};

struct vfs_superblock
{
    const char *device;
    uint32_t block_size;
};

struct vfs_mount
{
    struct dlist_head list;
};

struct vfs_type
{
    const char *name;
    struct vfs_mount *(*mount)(const char *source, const char *target, const char *filesystemtype,
                               unsigned long mountflags, const void *data);
    struct dlist_head list;
};

void virtual_fs_init();
int virtual_fs_set_type(struct vfs_type *type);
char *virtual_fs_read_block(const char *source, sector_t sector, uint32_t size);
int virtual_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
                     const void *data);