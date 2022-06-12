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
#include <kernel/api/posix/limits.h>
#include <kernel/api/posix/errno.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

struct vfs_inode *ext2_fs_lookup(struct vfs_inode *inode, struct vfs_dentry *dentry);
ssize_t ext2_fs_read(struct vfs_file *file, void *buffer, size_t count);

static struct vfs_inode_op ext2_file_inode_op = {};
static struct vfs_file_op ext2_file_op = {
    .read = ext2_fs_read};
static struct vfs_inode_op ext2_dir_inode_op = {
    .lookup = ext2_fs_lookup};
static struct vfs_file_op ext2_dir_op = {};

char *ext2_fs_bread(struct vfs_superblock *sb, uint32_t block, uint32_t size)
{
    return virt_fs_bread(sb->devname, block * (sb->blocksize / VFS_BYTES_P_SECTOR), size);
}

char *ext2_fs_bread_block(struct vfs_superblock *sb, uint32_t block)
{
    return ext2_fs_bread(sb, block, sb->blocksize);
}

struct ext2_group_desc *ext2_get_group_desc(struct vfs_superblock *sb, uint32_t group)
{
    struct ext2_superblock *ext2_sb = sb->data;
    if (!ext2_sb)
        return NULL;

    uint32_t block = ext2_sb->s_first_data_block + 1 + group / EXT2_GROUPS_P_BLOCK(ext2_sb);
    char *buffer = ext2_fs_bread_block(sb, block);
    if (!buffer)
        return NULL;

    struct ext2_group_desc *group_desc = calloc(1, sizeof(struct ext2_group_desc));
    uint32_t offset = group % EXT2_GROUPS_P_BLOCK(ext2_sb) * sizeof(struct ext2_group_desc);
    return memcpy(group_desc, buffer + offset, sizeof(struct ext2_group_desc));
}

struct ext2_inode *ext2_fs_get_inode(struct vfs_superblock *sb, ino_t ino)
{
    struct ext2_superblock *ext2_sb = sb->data;
    if (!ext2_sb)
        return NULL;

    struct ext2_group_desc *group_desc = ext2_get_group_desc(sb, EXT2_GET_GROUP_FROM_INODE(ext2_sb, ino));
    if (!group_desc)
        return NULL;

    uint32_t block = group_desc->bg_inode_table + EXT2_GET_RELATIVE_INODE_IN_GROUP(ext2_sb, ino) / EXT2_INODES_P_BLOCK(ext2_sb);
    char *buffer = ext2_fs_bread_block(sb, block);
    if (!buffer)
        return NULL;

    uint32_t offset = (EXT2_GET_RELATIVE_INODE_IN_GROUP(ext2_sb, ino) % EXT2_INODES_P_BLOCK(ext2_sb)) * sizeof(struct ext2_inode);
    return (struct ext2_inode *)(buffer + offset);
}

int ext2_fs_find_ino(struct vfs_superblock *sb, uint32_t block, const char *name)
{
    char *buffer = ext2_fs_bread_block(sb, block);
    if (!buffer)
        return -ENOMEM;

    struct ext2_dentry *dentry = (struct ext2_dentry *)buffer;
    uint32_t size = 0;
    char tmp[NAME_MAX] = {};

    while (size < sb->blocksize)
    {
        memcpy(tmp, dentry->name, dentry->name_len);
        tmp[dentry->name_len] = 0;

        if (strcmp(tmp, name) == 0)
            return dentry->ino;

        size = size + dentry->rec_len;
        dentry = (struct ext2_dentry *)((char *)dentry + dentry->rec_len);
    }

    return -ENOENT;
}

struct vfs_inode *ext2_fs_lookup(struct vfs_inode *inode, struct vfs_dentry *dentry)
{
    struct ext2_inode *ext2_inode = inode->data;
    if (!ext2_inode)
        return NULL;

    for (uint32_t i = 0; i < ext2_inode->i_blocks; i++)
    {
        if (!ext2_inode->i_block[i])
            continue;

        struct vfs_inode *new_inode = inode->sb->sop->create_inode(inode->sb);
        if (!new_inode)
            return NULL;

        int ino = ext2_fs_find_ino(inode->sb, ext2_inode->i_block[i], dentry->name);
        if (ino < 0)
            return NULL;
        new_inode->ino = ino;

        inode->sb->sop->read_inode(new_inode);
        return new_inode;
    }

    return NULL;
}

ssize_t ext2_fs_read(struct vfs_file *file, void *buffer, size_t count)
{
    struct vfs_inode *inode = file->dentry->inode;
    struct vfs_superblock *sb = inode->sb;
    struct ext2_inode *ext2_inode = inode->data;

    loff_t current_pos = file->pos;
    while (current_pos < file->pos + count)
    {
        uint32_t block = current_pos / sb->blocksize;
        char *ext2_buffer = ext2_fs_bread_block(sb, ext2_inode->i_block[block]);
        if (!ext2_buffer)
            return -ENOMEM;

        loff_t pos_start = (file->pos > current_pos) ? file->pos - current_pos : 0;
        loff_t pos_end = ((file->pos + count) < (current_pos + sb->blocksize)) ? (current_pos + sb->blocksize - file->pos - count) : 0;
        memcpy(buffer, ext2_buffer + pos_start, sb->blocksize - pos_start - pos_end);

        current_pos += sb->blocksize;
        buffer += sb->blocksize - pos_start - pos_end;
    }
    file->pos = file->pos + count;

    return count;
}

struct vfs_inode *ext2_fs_create_inode(struct vfs_superblock *sb)
{
    struct vfs_inode *inode = calloc(1, sizeof(struct vfs_inode));
    if (!inode)
        return NULL;
    inode->sb = sb;
    return inode;
}

void ext2_fs_read_inode(struct vfs_inode *inode)
{
    struct ext2_inode *ext2_inode = ext2_fs_get_inode(inode->sb, inode->ino);
    if (!ext2_inode)
        return;

    inode->size = ext2_inode->i_size;
    inode->mode = ext2_inode->i_mode;
    inode->data = ext2_inode;

    if (S_ISREG(inode->mode))
    {
        inode->iop = &ext2_file_inode_op;
        inode->fop = &ext2_file_op;
    }
    else if (S_ISDIR(inode->mode))
    {
        inode->iop = &ext2_dir_inode_op;
        inode->fop = &ext2_dir_op;
    }
}

static struct vfs_superblock_op ext2_superblock_op = {
    .create_inode = ext2_fs_create_inode,
    .read_inode = ext2_fs_read_inode};

struct vfs_mount *ext2_fs_mount(const char *pathname, const char *devname)
{
    struct vfs_superblock *sb = calloc(1, sizeof(struct vfs_superblock));
    if (!sb)
        return NULL;
    sb->blocksize = EXT2_BLOCK_SIZE;
    sb->devname = strdup(devname);
    sb->sop = &ext2_superblock_op;

    struct ext2_superblock *ext2_sb = calloc(1, sizeof(struct ext2_superblock));
    if (!ext2_sb)
        return NULL;

    char *buffer = ext2_fs_bread_block(sb, 1);
    if (!buffer)
        return NULL;
    memcpy(ext2_sb, buffer, sb->blocksize);

    if (ext2_sb->s_magic != EXT2_SUPER_MAGIC)
        return NULL;

    sb->data = ext2_sb;
    sb->blocksize = EXT2_GET_BLOCK_SIZE(ext2_sb);

    struct vfs_inode *inode = ext2_fs_create_inode(sb);
    if (!inode)
        return NULL;
    inode->ino = EXT2_ROOT_INO;
    ext2_fs_read_inode(inode);

    struct vfs_dentry *dentry = virt_fs_create_dentry(pathname, NULL);
    if (!dentry)
        return NULL;
    dentry->inode = inode;

    struct vfs_mount *mount = calloc(1, sizeof(struct vfs_mount));
    if (!mount)
        return NULL;
    mount->root = dentry;

    return mount;
}

static struct vfs_type ext2_type = {
    .name = "ext2",
    .mount = ext2_fs_mount};

void ext2_fs_init()
{
    virt_fs_add_type(&ext2_type);
    printf("Ext2: Initialized\n");
}
