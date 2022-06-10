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
#include <kernel/api/posix/errno.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/stdlib.h>

static struct dlist_head vfs_type_list;
static struct dlist_head vfs_mount_list;

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

int virt_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *)
{
    struct ata_device *device = ata_get_device(source);
    if (!device)
        return -ENOTBLK;

    struct vfs_type *type = virt_fs_get_type(filesystemtype);
    if (!type)
        return -ENODEV;

    struct vfs_mount *mount = type->mount();
    if (!mount)
        return -ENOMEM;

    dlist_add_tail(&mount->list, &vfs_mount_list);

    printf("Virtual FS: Mounted source = %s, target = %s, type = %s, flags = 0x%x\n", source, target, filesystemtype, mountflags);
    return 0;
}

void virt_fs_init()
{
    dlist_head_init(&vfs_type_list);
    dlist_head_init(&vfs_mount_list);

    printf("Virtual FS: Initialized\n");
}
