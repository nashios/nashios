/**
 * @file kernel/filesystem/ext2.h
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
#pragma once

#define EXT2_SUPER_MAGIC 0xEF53

#define EXT2_BLOCK_SIZE 1024

#define EXT2_GET_BLOCK_SIZE(sb) (EXT2_BLOCK_SIZE << sb->s_log_block_size)
#define EXT2_INODES_P_BLOCK(sb) (EXT2_GET_BLOCK_SIZE(sb) / sb->s_inode_size)
#define EXT2_GROUPS_P_BLOCK(sb) (EXT2_GET_BLOCK_SIZE(sb) / sizeof(struct ext2_group_desc))

#define EXT2_STARTING_INO 1
#define EXT2_ROOT_INO 2

#define EXT2_GET_GROUP_FROM_INODE(sb, ino) ((ino - EXT2_STARTING_INO) / sb->s_inodes_per_group)
#define EXT2_GET_RELATIVE_INODE_IN_GROUP(sb, ino) ((ino - EXT2_STARTING_INO) % sb->s_inodes_per_group)

#include <stdint.h>

struct ext2_inode
{
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    union {
        struct
        {
            uint32_t l_i_reserved1;
        } linux1;
        struct
        {
            uint32_t h_i_translator;
        } hurd1;
        struct
        {
            uint32_t m_i_reserved1;
        } masix1;
    } osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    union {
        struct linux2
        {
            uint8_t l_i_frag;
            uint8_t l_i_fsize;
            uint16_t i_pad1;
            uint16_t l_i_uid_high;
            uint16_t l_i_gid_high;
            uint32_t l_i_reserved2;
        } linux2;
        struct
        {
            uint8_t h_i_frag;
            uint8_t h_i_fsize;
            uint16_t h_i_mode_high;
            uint16_t h_i_uid_high;
            uint16_t h_i_gid_high;
            uint32_t h_i_author;
        } hurd2;
        struct
        {
            uint8_t m_i_frag;
            uint8_t m_i_fsize;
            uint16_t m_pad1;
            uint32_t m_i_reserved2[2];
        } masix2;
    } osd2;
};

struct ext2_group_desc
{
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
};

struct ext2_superblock
{
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;

    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t s_uuid[16];
    char s_volume_name[16];
    char s_last_mounted[64];
    uint32_t s_algorithm_usage_bitmap;

    uint8_t s_prealloc_blocks;
    uint8_t s_prealloc_dir_blocks;
    uint16_t s_padding1;

    uint8_t s_journal_uuid[16];
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;
    uint32_t s_hash_seed[4];
    uint8_t s_def_hash_version;
    uint8_t s_reserved_char_pad;
    uint16_t s_reserved_word_pad;
    uint32_t s_default_mount_opts;
    uint32_t s_first_meta_bg;
    uint32_t s_reserved[190];
};

struct ext2_dentry
{
    uint32_t ino;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    char name[];
};

void ext2_fs_init();
