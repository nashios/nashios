/**
 * @file kernel/filesystem/ext2.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Second extended file system
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
#include <kernel/filesystem/ext2.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/stdio.h>

static struct vfs_type ext2_type = {
    .name = "ext2"};

void ext2_fs_init()
{
    virt_fs_add_type(&ext2_type);
    printf("Ext2: Initialized\n");
}
