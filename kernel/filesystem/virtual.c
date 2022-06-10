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
#include <kernel/stdio.h>
#include <kernel/string.h>

static struct dlist_head vfs_type_list;

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

void virt_fs_init()
{
    dlist_head_init(&vfs_type_list);

    printf("Virtual FS: Initialized\n");
}
