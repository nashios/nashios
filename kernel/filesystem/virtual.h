/**
 * @file kernel/filesystem/virtual.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Virtual filesystem
 * @version 0.1
 * @date 2022-06-09
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

#define VFS_MAX_FD 256
#define VFS_BYTES_P_SECTOR 512

#include <kernel/api/posix/fcntl.h>
#include <kernel/api/posix/stat.h>
#include <kernel/api/posix/types.h>
#include <kernel/dlist.h>
#include <stdint.h>

struct vfs_type
{
    const char *name;
    struct vfs_mount *(*mount)(const char *pathname, const char *devname);

    struct dlist_head list;
};

struct vfs_mount
{
    struct vfs_dentry *root;
    struct dlist_head list;
};

struct vfs_file
{
    loff_t pos;
    struct vfs_dentry *dentry;
    struct vfs_file_op *fop;
};

struct vfs_file_op
{
    int (*open)();
    ssize_t (*read)(struct vfs_file *file, void *buffer, size_t count);
};

struct vfs_inode
{
    ino_t ino;
    umode_t mode;
    uint32_t size;
    struct vfs_superblock *sb;
    struct vfs_file_op *fop;
    struct vfs_inode_op *iop;
    void *data;
};

struct vfs_inode_op
{
    struct vfs_inode *(*lookup)(struct vfs_inode *inode, struct vfs_dentry *dentry);
};

struct vfs_dentry
{
    const char *name;
    struct dlist_head subdir_list;
    struct dlist_head list;
    struct vfs_inode *inode;
};

struct vfs_nameidata
{
    struct vfs_dentry *dentry;
};

struct vfs_superblock
{
    const char *devname;
    uint32_t blocksize;
    struct vfs_superblock_op *sop;
    void *data;
};

struct vfs_superblock_op
{
    struct vfs_inode *(*create_inode)(struct vfs_superblock *sb);
    void (*read_inode)(struct vfs_inode *inode);
};

void virt_fs_init();
void virt_fs_add_type(struct vfs_type *type);
void virt_fs_remove_type(struct vfs_type *type);
struct vfs_dentry *virt_fs_create_dentry(const char *name, struct vfs_dentry *parent);
int virt_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
                  const void *data);
int virt_fs_open(const char *pathname, int flags, ...);
int virt_fs_fstat(int fd, struct stat *buf);
ssize_t virt_fs_read(int fd, void *buf, size_t count);
char *virt_fs_bread(const char *devname, sector_t sector, uint32_t size);
