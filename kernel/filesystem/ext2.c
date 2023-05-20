#include <kernel/api/posix/errno.h>
#include <kernel/assert.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

#define EXT2_SUPER_MAGIC 0xEF53

#define EXT2_MIN_BLOCK_SIZE 1024

#define EXT2_STARTING_INO 1
#define EXT2_ROOT_INO 2

#define EXT2_BLOCK_SIZE(sb) (EXT2_MIN_BLOCK_SIZE << sb->s_log_block_size)
#define EXT2_INODES_P_BLOCK(sb) (EXT2_BLOCK_SIZE(sb) / sb->s_inode_size)
#define EXT2_GROUPS_P_BLOCK(sb) (EXT2_BLOCK_SIZE(sb) / sizeof(struct ext2_group_desc))

#define EXT2_GROUP_FROM_INODE(sb, ino) ((ino - EXT2_STARTING_INO) / sb->s_inodes_per_group)
#define EXT2_RELATIVE_INODE_IN_GROUP(sb, ino) ((ino - EXT2_STARTING_INO) % sb->s_inodes_per_group)

#define EXT2_INO_UPPER_LEVEL0 12
#define EXT2_INO_UPPER_LEVEL1 268
#define EXT2_INO_UPPER_LEVEL2 65804
#define EXT2_INO_UPPER_LEVEL3 16843020

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

char *ext2_fs_read_block_size(struct vfs_superblock *superblock, uint32_t block, uint32_t size)
{
    sector_t sector = block * (superblock->block_size / VFS_BYTES_P_SECTOR);
    return virtual_fs_read_block(superblock->device, sector, size);
}

char *ext2_fs_read_block(struct vfs_superblock *superblock, uint32_t block)
{
    return ext2_fs_read_block_size(superblock, block, superblock->block_size);
}

int ext2_fs_recursive_block_action(struct vfs_superblock *superblock, int level, uint32_t block, const void *arg,
                                   ext2_fs_recursive_action_t action)
{
    ASSERT(level <= 3);

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
    struct vfs_inode *inode = calloc(1, sizeof(struct vfs_inode));
    if (!inode)
        return NULL;
    inode->superblock = superblock;

    return inode;
}

static struct vfs_inode_op s_ext2_file_iop = {};
static struct vfs_file_op s_ext2_file_fop = {};
static struct vfs_inode_op s_ext2_dir_iop = {.lookup = ext2_fs_lookup};
static struct vfs_file_op s_ext2_dir_fop = {};
static struct vfs_superblock_op s_ext2_sop = {.allocate_inode = ext2_fs_allocate_inode,
                                              .read_inode = ext2_fs_read_inode};

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
    mount->dentry = dentry;

    return mount;
}

static struct vfs_type s_ext2_type = {.name = "ext2", .mount = ext2_fs_mount};

void ext2_fs_init()
{
    virtual_fs_set_type(&s_ext2_type);
    virtual_fs_mount("/dev/hda", "/", s_ext2_type.name, 0x00, NULL);

    printf("Ext2 FS: Initialized\n");
}