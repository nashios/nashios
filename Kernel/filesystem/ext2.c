#include <Kernel/api/posix/errno.h>
#include <Kernel/filesystem/ext2.h>
#include <Kernel/filesystem/virtual.h>
#include <Kernel/lib/stdio.h>
#include <Kernel/lib/stdlib.h>
#include <Kernel/lib/string.h>
#include <Kernel/math.h>
#include <st/assert.h>

#define EXT2_SUPER_MAGIC 0xEF53

#define EXT2_MIN_BLOCK_SIZE 1024

#define EXT2_STARTING_INO 1
#define EXT2_ROOT_INO 2

#define EXT2_BLOCK_SIZE(sb) (EXT2_MIN_BLOCK_SIZE << sb->s_log_block_size)
#define EXT2_INODES_P_BLOCK(sb) (EXT2_BLOCK_SIZE(sb) / sb->s_inode_size)
#define EXT2_GROUPS_P_BLOCK(sb) (EXT2_BLOCK_SIZE(sb) / sizeof(struct ext2_group_desc))

#define EXT2_GROUP_FROM_INODE(sb, ino) ((ino - EXT2_STARTING_INO) / sb->s_inodes_per_group)
#define EXT2_GROUP_FROM_BLOCK(sb, block) ((block - sb->s_first_data_block) / sb->s_blocks_per_group)
#define EXT2_RELATIVE_INODE_IN_GROUP(sb, ino) ((ino - EXT2_STARTING_INO) % sb->s_inodes_per_group)

#define EXT2_INO_UPPER_LEVEL0 12
#define EXT2_INO_UPPER_LEVEL1 268
#define EXT2_INO_UPPER_LEVEL2 65804
#define EXT2_INO_UPPER_LEVEL3 16843020

#define EXT2_DIR_PAD 4
#define EXT2_DIR_ROUND (EXT2_DIR_PAD - 1)
#define EXT2_DIR_REC_LEN(name_len) (((name_len) + 8 + EXT2_DIR_ROUND) & ~EXT2_DIR_ROUND)

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

struct ext2_dir_entry
{
    uint32_t ino;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    char name[];
};

typedef int (*ext2_fs_recursive_action_t)(struct vfs_superblock *superblock, uint32_t block, const void *arg);

void ext2_fs_read_inode(struct vfs_inode *inode);
struct ext2_group_desc *ext2_fs_get_group_desc(struct vfs_superblock *sb, uint32_t group);
struct vfs_inode *ext2_fs_create_inode(struct vfs_inode *dir, struct vfs_dentry *dentry, mode_t mode);

char *ext2_fs_read_block_size(struct vfs_superblock *superblock, uint32_t block, uint32_t size)
{
    sector_t sector = block * (superblock->block_size / VFS_BYTES_P_SECTOR);
    return virtual_fs_read_block(superblock->device, sector, size);
}

char *ext2_fs_read_block(struct vfs_superblock *superblock, uint32_t block)
{
    return ext2_fs_read_block_size(superblock, block, superblock->block_size);
}

void ext2_fs_write_block_size(struct vfs_superblock *superblock, uint32_t block, char *buffer, uint32_t size)
{
    return virtual_fs_write_block(superblock->device, block * (superblock->block_size / VFS_BYTES_P_SECTOR), buffer,
                                  size);
}

void ext2_fs_write_block(struct vfs_superblock *superblock, uint32_t block, char *buffer)
{
    return ext2_fs_write_block_size(superblock, block, buffer, superblock->block_size);
}

int ext2_fs_recursive_block_action(struct vfs_superblock *superblock, int level, uint32_t block, const void *arg,
                                   ext2_fs_recursive_action_t action)
{
    assert(level <= 3);

    if (level > 0)
    {
        int result = -ENOENT;
        uint32_t *buffer = (uint32_t *)ext2_fs_read_block(superblock, block);
        if (!buffer)
            return result;

        uint32_t block_size = superblock->block_size / 4;
        for (uint32_t i = 0; i < block_size; i++)
        {
            result = ext2_fs_recursive_block_action(superblock, level - 1, buffer[i], arg, action);
            if (result >= 0)
                break;
        }
        return result;
    }
    else
        return action(superblock, block, arg);
}

int ext2_fs_find_ino(struct vfs_superblock *superblock, uint32_t block, const void *arg)
{
    char *buffer = ext2_fs_read_block(superblock, block);
    if (!buffer)
        return -EINVAL;

    char name[256];
    uint32_t size = 0;
    struct ext2_dir_entry *entry = (struct ext2_dir_entry *)buffer;
    while (size < superblock->block_size)
    {
        memcpy(name, entry->name, entry->name_len);
        name[entry->name_len] = 0;

        if (!strcmp(name, arg))
            return entry->ino;

        size = size + entry->rec_len;
        entry = (struct ext2_dir_entry *)((char *)entry + entry->rec_len);
    }
    return -ENOENT;
}

struct vfs_inode *ext2_fs_lookup(struct vfs_inode *inode, struct vfs_dentry *dentry)
{
    struct ext2_inode *ext2_inode = inode->info;
    struct vfs_superblock *superblock = inode->superblock;

    for (uint32_t i = 0, ino = 0; i < ext2_inode->i_blocks; i++)
    {
        if (!ext2_inode->i_block[i])
            continue;

        if ((i < EXT2_INO_UPPER_LEVEL0 && (ino = ext2_fs_recursive_block_action(superblock, 0, ext2_inode->i_block[i],
                                                                                dentry->name, ext2_fs_find_ino)) > 0) ||
            ((EXT2_INO_UPPER_LEVEL0 <= i && i < EXT2_INO_UPPER_LEVEL1) &&
             (ino = ext2_fs_recursive_block_action(superblock, 1, ext2_inode->i_block[12], dentry->name,
                                                   ext2_fs_find_ino)) > 0) ||
            ((EXT2_INO_UPPER_LEVEL1 <= i && i < EXT2_INO_UPPER_LEVEL2) &&
             (ino = ext2_fs_recursive_block_action(superblock, 2, ext2_inode->i_block[13], dentry->name,
                                                   ext2_fs_find_ino)) > 0) ||
            ((EXT2_INO_UPPER_LEVEL2 <= i && i < EXT2_INO_UPPER_LEVEL3) &&
             (ino = ext2_fs_recursive_block_action(superblock, 3, ext2_inode->i_block[14], dentry->name,
                                                   ext2_fs_find_ino)) > 0))
        {
            struct vfs_inode *new_inode = superblock->op->allocate_inode(inode->superblock);
            if (!new_inode)
                break;

            new_inode->ino = ino;
            superblock->op->read_inode(new_inode);
            return new_inode;
        }
    }
    return NULL;
}

struct vfs_inode *ext2_fs_allocate_inode(struct vfs_superblock *superblock)
{
    return virtual_fs_create_inode(superblock);
}

void ext2_fs_read_n_block(struct vfs_superblock *superblock, uint32_t block, char **p_buffer, loff_t ppos,
                          loff_t *p_position, size_t count, int level)
{
    assert(level <= 3);

    if (level > 0)
    {
        uint32_t *buffer = (uint32_t *)ext2_fs_read_block(superblock, block);
        uint32_t blocks = superblock->block_size / 4;
        for (uint32_t i = 0; *p_position < ppos + (loff_t)count && i < blocks; ++i)
            ext2_fs_read_n_block(superblock, buffer[i], p_buffer, ppos, p_position, count, level - 1);
    }
    else
    {

        char *buffer = ext2_fs_read_block(superblock, block);
        uint32_t p_start = (ppos > *p_position) ? ppos - *p_position : 0;
        uint32_t p_end = ((ppos + count) < (*p_position + superblock->block_size))
                             ? (*p_position + superblock->block_size - ppos - count)
                             : 0;
        memcpy(*p_buffer, buffer + p_start, superblock->block_size - p_start - p_end);
        *p_position += superblock->block_size;
        *p_buffer += superblock->block_size - p_start - p_end;
    }
}

ssize_t ext2_fs_read_file(struct vfs_file *file, char *buffer, size_t count, loff_t ppos)
{
    struct vfs_inode *vfs_inode = file->dentry->inode;
    struct ext2_inode *ext2_inode = vfs_inode->info;
    struct vfs_superblock *superblock = vfs_inode->superblock;

    count = MIN_T(size_t, ppos + count, ext2_inode->i_size) - ppos;
    loff_t position = (ppos / superblock->block_size) * superblock->block_size;
    char *p_buffer = buffer;
    while (position < ppos + (loff_t)count)
    {
        uint32_t block = position / superblock->block_size;
        if (block < EXT2_INO_UPPER_LEVEL0)
            ext2_fs_read_n_block(superblock, ext2_inode->i_block[block], &p_buffer, ppos, &position, count, 0);
        else if (block < EXT2_INO_UPPER_LEVEL1)
            ext2_fs_read_n_block(superblock, ext2_inode->i_block[12], &p_buffer, ppos, &position, count, 1);
        else if (block < EXT2_INO_UPPER_LEVEL2)
            ext2_fs_read_n_block(superblock, ext2_inode->i_block[13], &p_buffer, ppos, &position, count, 2);
        else if (block < EXT2_INO_UPPER_LEVEL3)
            ext2_fs_read_n_block(superblock, ext2_inode->i_block[14], &p_buffer, ppos, &position, count, 3);
        else
            assert_not_reached();
    }

    file->position = ppos + count;
    return count;
}

void ext2_fs_write_inode(struct vfs_inode *inode)
{
    struct ext2_superblock *ext2_sb = inode->superblock->info;
    struct ext2_inode *ext2_inode = inode->info;

    ext2_inode->i_mode = inode->mode;
    ext2_inode->i_size = inode->size;
    ext2_inode->i_blocks = inode->blocks;

    if (S_ISCHR(inode->mode))
        ext2_inode->i_block[0] = inode->rdev;

    uint32_t group = EXT2_GROUP_FROM_INODE(ext2_sb, inode->ino);
    struct ext2_group_desc *gdp = ext2_fs_get_group_desc(inode->superblock, group);
    uint32_t block =
        gdp->bg_inode_table + EXT2_RELATIVE_INODE_IN_GROUP(ext2_sb, inode->ino) / EXT2_INODES_P_BLOCK(ext2_sb);
    uint32_t offset =
        (EXT2_RELATIVE_INODE_IN_GROUP(ext2_sb, inode->ino) % EXT2_INODES_P_BLOCK(ext2_sb)) * sizeof(struct ext2_inode);
    char *buffer = ext2_fs_read_block(inode->superblock, block);

    memcpy(buffer + offset, ext2_inode, sizeof(struct ext2_inode));
    ext2_fs_write_block(inode->superblock, block, buffer);
}

int ext2_fs_mknod(struct vfs_inode *dir, struct vfs_dentry *dentry, int mode, dev_t dev)
{
    struct vfs_inode *inode = ext2_fs_lookup(dir, dentry);
    if (inode == NULL)
        inode = ext2_fs_create_inode(dir, dentry, mode);

    inode->rdev = dev;
    virtual_fs_init_special_inode(inode, mode, dev);
    ext2_fs_write_inode(inode);

    dentry->inode = inode;
    return 0;
}

void ext2_fs_write_super(struct vfs_superblock *sb)
{
    struct ext2_superblock *ext2_sb = sb->info;
    ext2_fs_write_block(sb, ext2_sb->s_first_data_block, (char *)ext2_sb);
}

static struct vfs_inode_op s_ext2_file_iop = {};

static struct vfs_file_op s_ext2_file_fop = {.read = ext2_fs_read_file};

static struct vfs_inode_op s_ext2_dir_iop = {
    .lookup = ext2_fs_lookup, .mknod = ext2_fs_mknod, .create = ext2_fs_create_inode};

static struct vfs_file_op s_ext2_dir_fop = {};

static struct vfs_superblock_op s_ext2_sop = {.allocate_inode = ext2_fs_allocate_inode,
                                              .read_inode = ext2_fs_read_inode,
                                              .write_inode = ext2_fs_write_inode,
                                              .write_super = ext2_fs_write_super};

struct vfs_inode_op s_ext2_special_iop = {};

int ext2_fs_find_unused_inode(struct vfs_superblock *sb)
{
    struct ext2_superblock *ext2_sb = sb->info;

    uint32_t number_of_groups = DIV_ROUND_UP(ext2_sb->s_blocks_count, ext2_sb->s_blocks_per_group);
    for (uint32_t group = 0; group < number_of_groups; group += 1)
    {
        struct ext2_group_desc *gdp = ext2_fs_get_group_desc(sb, group);
        unsigned char *inode_bitmap = (unsigned char *)ext2_fs_read_block(sb, gdp->bg_inode_bitmap);

        for (uint32_t i = 0; i < sb->block_size; ++i)
        {
            if (inode_bitmap[i] == 0xff)
                continue;

            for (uint8_t j = 0; j < 8; ++j)
            {
                if (!(inode_bitmap[i] & (1 << j)))
                    return group * ext2_sb->s_inodes_per_group + i * 8 + j + EXT2_STARTING_INO;
            }
        }
    }
    return -ENOSPC;
}

struct ext2_group_desc *ext2_fs_get_group_desc(struct vfs_superblock *sb, uint32_t group)
{
    struct ext2_superblock *ext2_sb = sb->info;
    uint32_t block = ext2_sb->s_first_data_block + 1 + group / EXT2_GROUPS_P_BLOCK(ext2_sb);
    char *buffer = ext2_fs_read_block(sb, block);
    if (!buffer)
        return NULL;

    struct ext2_group_desc *description = (struct ext2_group_desc *)calloc(1, sizeof(struct ext2_group_desc));
    if (!description)
        return NULL;

    uint32_t offset = group % EXT2_GROUPS_P_BLOCK(ext2_sb) * sizeof(struct ext2_group_desc);
    memcpy(description, buffer + offset, sizeof(struct ext2_group_desc));
    return description;
}

void ext2_fs_write_group_desc(struct vfs_superblock *sb, struct ext2_group_desc *gdp)
{
    struct ext2_superblock *ext2_sb = sb->info;
    uint32_t group = EXT2_GROUP_FROM_BLOCK(ext2_sb, gdp->bg_block_bitmap);
    uint32_t block = ext2_sb->s_first_data_block + 1 + group / EXT2_GROUPS_P_BLOCK(ext2_sb);
    uint32_t offset = group % EXT2_GROUPS_P_BLOCK(ext2_sb) * sizeof(struct ext2_group_desc);
    char *buffer = ext2_fs_read_block(sb, block);
    memcpy(buffer + offset, gdp, sizeof(struct ext2_group_desc));
    ext2_fs_write_block(sb, block, buffer);
}

uint32_t ext2_fs_create_block(struct vfs_superblock *sb)
{
    struct ext2_superblock *ext2_sb = sb->info;
    uint32_t block = ext2_fs_find_unused_inode(sb);
    ext2_sb->s_free_blocks_count -= 1;
    sb->op->write_super(sb);

    struct ext2_group_desc *gdp = ext2_fs_get_group_desc(sb, EXT2_GROUP_FROM_BLOCK(ext2_sb, block));
    gdp->bg_free_blocks_count -= 1;
    ext2_fs_write_group_desc(sb, gdp);

    char *bitmap_buf = ext2_fs_read_block(sb, gdp->bg_block_bitmap);
    uint32_t relative_block = EXT2_RELATIVE_INODE_IN_GROUP(ext2_sb, block);
    bitmap_buf[relative_block / 8] |= 1 << (relative_block % 8);
    ext2_fs_write_block(sb, gdp->bg_block_bitmap, bitmap_buf);

    char *data_buf = calloc(sb->block_size, sizeof(char));
    ext2_fs_write_block(sb, block, data_buf);

    return block;
}

int ext2_fs_add_entry(struct vfs_superblock *sb, uint32_t block, void *arg)
{
    struct vfs_dentry *dentry = arg;
    int filename_length = strlen(dentry->name);

    char *block_buf = ext2_fs_read_block(sb, block);

    uint32_t size = 0, new_rec_len = 0;
    struct ext2_dir_entry *entry = (struct ext2_dir_entry *)block_buf;
    while (size < sb->block_size && (char *)entry < block_buf + sb->block_size)
    {
        if (!entry->ino && (!entry->rec_len || entry->rec_len >= EXT2_DIR_REC_LEN(filename_length)))
        {
            entry->ino = dentry->inode->ino;
            if (S_ISREG(dentry->inode->mode))
                entry->file_type = 1;
            else if (S_ISDIR(dentry->inode->mode))
                entry->file_type = 2;
            else
                assert_todo();
            entry->name_len = filename_length;
            memcpy(entry->name, dentry->name, entry->name_len);
            entry->rec_len = MAX_T(uint16_t, new_rec_len, entry->rec_len);

            ext2_fs_write_block(sb, block, block_buf);
            return 0;
        }
        if (EXT2_DIR_REC_LEN(filename_length) + EXT2_DIR_REC_LEN(entry->name_len) < entry->rec_len)
        {
            new_rec_len = entry->rec_len - EXT2_DIR_REC_LEN(entry->name_len);
            entry->rec_len = EXT2_DIR_REC_LEN(entry->name_len);
            size += entry->rec_len;
            entry = (struct ext2_dir_entry *)((char *)entry + entry->rec_len);
            memset(entry, 0, new_rec_len);
        }
        else
        {
            size += entry->rec_len;
            new_rec_len = sb->block_size - size;
            entry = (struct ext2_dir_entry *)((char *)entry + entry->rec_len);
        }
    }
    return -ENOENT;
}

int ext2_fs_create_entry(struct vfs_superblock *sb, struct vfs_inode *dir, struct vfs_dentry *dentry)
{
    struct ext2_inode *ei = dir->info;
    for (uint32_t i = 0; i < dir->blocks; i++)
    {
        if (i >= EXT2_INO_UPPER_LEVEL0)
            assert_not_reached();

        int block = ei->i_block[i];
        if (!block)
        {
            block = ext2_fs_create_block(dir->superblock);
            ei->i_block[i] = block;
            dir->blocks += 1;
            dir->size += sb->block_size;
            ext2_fs_write_inode(dir);
        }
        if (ext2_fs_add_entry(sb, block, dentry) >= 0)
            return 0;
    }
    return -ENOENT;
}

struct vfs_inode *ext2_fs_create_inode(struct vfs_inode *dir, struct vfs_dentry *dentry, mode_t mode)
{
    struct vfs_superblock *superblock = dir->superblock;
    struct ext2_superblock *ext2_sb = superblock->info;
    uint32_t ino = ext2_fs_find_unused_inode(superblock);
    struct ext2_group_desc *gdp = ext2_fs_get_group_desc(superblock, EXT2_GROUP_FROM_INODE(ext2_sb, ino));

    ext2_sb->s_free_inodes_count -= 1;
    superblock->op->write_super(superblock);

    gdp->bg_free_inodes_count -= 1;
    if (S_ISDIR(mode))
        gdp->bg_used_dirs_count += 1;

    ext2_fs_write_group_desc(superblock, gdp);

    char *inode_bitmap_buf = ext2_fs_read_block(superblock, gdp->bg_inode_bitmap);
    uint32_t relative_inode = EXT2_RELATIVE_INODE_IN_GROUP(ext2_sb, ino);
    inode_bitmap_buf[relative_inode / 8] |= 1 << (relative_inode % 8);
    ext2_fs_write_block(superblock, gdp->bg_inode_bitmap, inode_bitmap_buf);

    struct ext2_inode *ei_new = calloc(1, sizeof(struct ext2_inode));
    ei_new->i_links_count = 1;

    struct vfs_inode *inode = superblock->op->allocate_inode(superblock);
    inode->ino = ino;
    inode->mode = mode;
    inode->size = 0;
    inode->info = ei_new;
    inode->superblock = superblock;

    if (S_ISREG(mode))
    {
        inode->iop = &s_ext2_file_iop;
        inode->fop = &s_ext2_file_fop;
    }
    else if (S_ISDIR(mode))
    {
        inode->iop = &s_ext2_dir_iop;
        inode->fop = &s_ext2_dir_fop;

        struct ext2_inode *ei = inode->info;
        uint32_t block = ext2_fs_create_block(inode->superblock);
        ei->i_block[0] = block;

        inode->blocks += 1;
        inode->size += superblock->block_size;

        ext2_fs_write_inode(inode);

        char *block_buf = ext2_fs_read_block(inode->superblock, block);

        struct ext2_dir_entry *c_entry = (struct ext2_dir_entry *)block_buf;
        c_entry->ino = inode->ino;
        memcpy(c_entry->name, ".", 1);
        c_entry->name_len = 1;
        c_entry->rec_len = EXT2_DIR_REC_LEN(1);
        c_entry->file_type = 2;

        struct ext2_dir_entry *p_entry = (struct ext2_dir_entry *)(block_buf + c_entry->rec_len);
        p_entry->ino = dir->ino;
        memcpy(p_entry->name, "..", 2);
        p_entry->name_len = 2;
        p_entry->rec_len = superblock->block_size - c_entry->rec_len;
        p_entry->file_type = 2;

        ext2_fs_write_block(inode->superblock, block, block_buf);
    }
    else
        assert_not_reached();

    superblock->op->write_inode(inode);
    dentry->inode = inode;

    if (ext2_fs_create_entry(superblock, dir, dentry) >= 0)
        return inode;
    return NULL;
}

struct ext2_inode *ext2_fs_get_inode(struct vfs_superblock *sb, ino_t ino)
{
    struct ext2_superblock *ext2_sb = sb->info;
    uint32_t group = EXT2_GROUP_FROM_INODE(ext2_sb, ino);
    struct ext2_group_desc *description = ext2_fs_get_group_desc(sb, group);
    if (!description)
        return NULL;

    uint32_t block =
        description->bg_inode_table + EXT2_RELATIVE_INODE_IN_GROUP(ext2_sb, ino) / EXT2_INODES_P_BLOCK(ext2_sb);
    char *buffer = ext2_fs_read_block(sb, block);
    if (!buffer)
        return NULL;

    uint32_t offset =
        (EXT2_RELATIVE_INODE_IN_GROUP(ext2_sb, ino) % EXT2_INODES_P_BLOCK(ext2_sb)) * sizeof(struct ext2_inode);
    return (struct ext2_inode *)(buffer + offset);
}

void ext2_fs_read_inode(struct vfs_inode *inode)
{
    struct ext2_inode *ext2_inode = ext2_fs_get_inode(inode->superblock, inode->ino);
    if (!ext2_inode)
        return;

    inode->mode = ext2_inode->i_mode;
    inode->size = ext2_inode->i_size;
    inode->blocks = ext2_inode->i_blocks;
    inode->block_size = PAGE_SIZE;
    inode->info = ext2_inode;

    if (S_ISREG(inode->mode))
    {
        inode->iop = &s_ext2_file_iop;
        inode->fop = &s_ext2_file_fop;
    }
    else if (S_ISDIR(inode->mode))
    {
        inode->iop = &s_ext2_dir_iop;
        inode->fop = &s_ext2_dir_fop;
    }
    else
    {
        inode->iop = &s_ext2_special_iop;
        virtual_fs_init_special_inode(inode, inode->mode, ext2_inode->i_block[0]);
    }
}

struct vfs_mount *ext2_fs_mount(const char *source, const char *target, const char *, unsigned long, const void *)
{
    struct vfs_superblock *sb = (struct vfs_superblock *)calloc(1, sizeof(struct vfs_superblock));
    if (!sb)
        return NULL;
    sb->block_size = EXT2_MIN_BLOCK_SIZE;
    sb->device = strdup(source);

    struct ext2_superblock *ext2_sb = (struct ext2_superblock *)calloc(1, sizeof(struct ext2_superblock));
    if (!ext2_sb)
        return NULL;

    char *ext2_sb_buffer = ext2_fs_read_block(sb, 1);
    if (!ext2_sb_buffer)
        return NULL;

    memcpy(ext2_sb, ext2_sb_buffer, sb->block_size);
    if (ext2_sb->s_magic != EXT2_SUPER_MAGIC)
        return NULL;

    sb->info = ext2_sb;
    sb->op = &s_ext2_sop;
    sb->block_size = EXT2_BLOCK_SIZE(ext2_sb);

    struct vfs_inode *inode = sb->op->allocate_inode(sb);
    if (!inode)
        return NULL;
    inode->ino = EXT2_ROOT_INO;
    sb->op->read_inode(inode);

    struct vfs_dentry *dentry = virtual_fs_create_dentry(target);
    if (!dentry)
        return NULL;
    dentry->inode = inode;

    struct vfs_mount *mount = (struct vfs_mount *)calloc(1, sizeof(struct vfs_mount));
    if (!mount)
        return NULL;
    mount->root = dentry;
    mount->mount = dentry;

    return mount;
}

static struct vfs_type s_ext2_type = {.name = "ext2", .mount = ext2_fs_mount};

void ext2_fs_init()
{
    virtual_fs_set_type(&s_ext2_type);
    virtual_fs_mount("/dev/hda", "/", s_ext2_type.name, 0x00, NULL);

    printf("Ext2 FS: Initialized\n");
}
