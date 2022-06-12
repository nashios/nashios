/**
 * @file kernel/filesystem/virtual.c
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
#include <kernel/filesystem/virtual.h>
#include <kernel/drivers/ata.h>
#include <kernel/task/scheduler.h>
#include <kernel/api/posix/errno.h>
#include <kernel/api/posix/limits.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/stdlib.h>
#include <kernel/math.h>

static struct dlist_head vfs_type_list;
static struct dlist_head vfs_mount_list;

char *virt_fs_bread(const char *devname, sector_t sector, uint32_t size)
{
    struct ata_device *device = ata_get_device(devname);
    if (!device)
        return NULL;

    uint8_t sectors = DIV_ROUND_UP(size, VFS_BYTES_P_SECTOR);
    char *buffer = calloc(sectors * VFS_BYTES_P_SECTOR, sizeof(char));
    if (!buffer)
        return NULL;

    if (ata_read(device, (uint16_t *)buffer, sector, sectors) < 0)
        return NULL;

    return buffer;
}

struct vfs_dentry *virt_fs_create_dentry(const char *name, struct vfs_dentry *parent)
{
    struct vfs_dentry *dentry = calloc(1, sizeof(struct vfs_dentry));
    if (!dentry)
        return NULL;

    dentry->name = strdup(name);
    dlist_head_init(&dentry->subdir_list);

    return dentry;
}

struct vfs_type *virt_fs_get_type(const char *name)
{
    struct vfs_type *type;
    dlist_foreach_entry(type, &vfs_type_list, list)
    {
        if (strcmp(type->name, name) == 0)
            return type;
    }
    return NULL;
}

void virt_fs_add_type(struct vfs_type *type)
{
    if (virt_fs_get_type(type->name))
        return;

    dlist_add_tail(&type->list, &vfs_type_list);
}

void virt_fs_remove_type(struct vfs_type *type)
{
    if (!virt_fs_get_type(type->name))
        return;

    dlist_remove_entry(&type->list);
}

ssize_t virt_fs_read(int fd, void *buf, size_t count)
{
    if (fd < 0)
        return -EBADF;

    struct vfs_file *file = sched_current_process()->files[fd];
    if (!file)
        return -EBADF;

    if (file->fop && file->fop->read)
        return file->fop->read(buf, count);

    return -EBADF;
}

int virt_fs_getattr(struct vfs_dentry *dentry, struct stat *stat)
{
    struct vfs_inode *inode = dentry->inode;
    if (!inode)
        return -EFAULT;

    stat->st_size = inode->size;

    return 0;
}

int virt_fs_fstat(int fd, struct stat *buf)
{
    if (fd < 0)
        return -EBADF;

    struct vfs_file *file = sched_current_process()->files[fd];
    if (!file)
        return -EBADF;

    return virt_fs_getattr(file->dentry, buf);
}

int virt_fs_path_lookup(const char *pathname, struct vfs_nameidata *nameidata)
{
    struct process *process = sched_current_process();
    size_t i = 0;
    if (pathname[i] == '/')
    {
        nameidata->dentry = process->mount->root;
        while (pathname[i] == '/')
            i++;
    }
    else
        nameidata->dentry = process->root;

    char name[NAME_MAX] = {};
    size_t length = strlen(pathname);
    while (i < length)
    {
        memset(name, 0x0, sizeof(name));

        for (size_t j = 0; pathname[i] != '/' && i < length; i++, j++)
            name[j] = pathname[i];

        while (pathname[i] == '/' && i < length)
            i++;

        struct vfs_dentry *dentry = NULL;
        struct vfs_dentry *i_dentry = NULL;
        dlist_foreach_entry(i_dentry, &nameidata->dentry->subdir_list, list)
        {
            if (strcmp(name, i_dentry->name) == 0)
            {
                dentry = i_dentry;
                break;
            }
        }

        if (!dentry)
        {
            dentry = virt_fs_create_dentry(name, nameidata->dentry);

            struct vfs_inode *inode = NULL;
            if (nameidata->dentry->inode->iop && nameidata->dentry->inode->iop->lookup)
                inode = nameidata->dentry->inode->iop->lookup();

            if (!inode)
                return -ENOENT;

            dentry->inode = inode;
            dlist_add_tail(&dentry->list, &nameidata->dentry->subdir_list);
            nameidata->dentry = dentry;
        }
        else
            nameidata->dentry = dentry;
    }

    return 0;
}

int virt_fs_new_fd()
{
    for (int i = 0; i < VFS_MAX_FD; i++)
    {
        if (!sched_current_process()->files[i])
            return i;
    }
    return -ENFILE;
}

int virt_fs_open(const char *pathname, int flags, ...)
{
    int fd = virt_fs_new_fd();
    if (fd < 0)
        return fd;

    struct vfs_nameidata nameidata = {};
    int result = virt_fs_path_lookup(pathname, &nameidata);
    if (result < 0)
        return result;

    struct vfs_file *file = calloc(1, sizeof(struct vfs_file));
    if (!file)
        return -ENOMEM;
    file->dentry = nameidata.dentry;
    file->fop = nameidata.dentry->inode->fop;

    if (file->fop && file->fop->open)
    {
        result = file->fop->open();
        if (result)
        {
            free(file);
            return result;
        }
    }

    sched_current_process()->files[fd] = file;

    return fd;
}

int virt_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *)
{
    struct ata_device *device = ata_get_device(source);
    if (!device)
        return -ENOTBLK;

    struct vfs_type *type = virt_fs_get_type(filesystemtype);
    if (!type)
        return -ENODEV;

    struct vfs_mount *mount = type->mount(target);
    if (!mount)
        return -ENOMEM;

    dlist_add_tail(&mount->list, &vfs_mount_list);

    if (strcmp(target, "/") == 0)
    {
        struct process *process = sched_current_process();
        process->mount = mount;
        process->root = mount->root;
    }

    printf("Virtual FS: Mounted source = %s, target = %s, type = %s, flags = 0x%x\n", source, target, filesystemtype, mountflags);
    return 0;
}

void virt_fs_init()
{
    dlist_head_init(&vfs_type_list);
    dlist_head_init(&vfs_mount_list);

    printf("Virtual FS: Initialized\n");
}
