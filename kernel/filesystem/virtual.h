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

#include <kernel/dlist.h>

struct vfs_type
{
    const char *name;
    struct vfs_mount *(*mount)();

    struct dlist_head list;
};

struct vfs_mount
{
    struct dlist_head list;
};

void virt_fs_init();
void virt_fs_add_type(struct vfs_type *type);
void virt_fs_remove_type(struct vfs_type *type);
int virt_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);
