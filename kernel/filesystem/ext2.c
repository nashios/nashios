#include <kernel/filesystem/ext2.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

#define EXT2_SUPER_MAGIC 0xEF53

#define EXT2_MIN_BLOCK_SIZE 1024

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

char *ext2_read_block_size(struct vfs_superblock *vfs_sb, uint32_t block, uint32_t size)
{
    sector_t sector = block * (vfs_sb->block_size / VFS_BYTES_P_SECTOR);
    return virtual_fs_read_block(vfs_sb->device, sector, size);
}

char *ext2_read_block(struct vfs_superblock *vfs_sb, uint32_t block)
{
    return ext2_read_block_size(vfs_sb, block, vfs_sb->block_size);
}

struct vfs_mount *ext2_fs_mount(const char *source, const char *target, const char *filesystemtype,
                                unsigned long mountflags, const void *data)
{
    struct vfs_superblock *vfs_sb = (struct vfs_superblock *)calloc(1, sizeof(struct vfs_superblock));
    if (!vfs_sb)
        return NULL;
    vfs_sb->block_size = EXT2_MIN_BLOCK_SIZE;
    vfs_sb->device = strdup(source);

    struct ext2_superblock *ext2_sb = (struct ext2_superblock *)calloc(1, sizeof(struct ext2_superblock));
    if (!ext2_sb)
        return NULL;

    char *ext2_sb_buffer = ext2_read_block(vfs_sb, 1);
    if (!ext2_sb_buffer)
        return NULL;

    memcpy(ext2_sb, ext2_sb_buffer, vfs_sb->block_size);
    if (ext2_sb->s_magic != EXT2_SUPER_MAGIC)
        return NULL;

    struct vfs_mount *mount = (struct vfs_mount *)calloc(1, sizeof(struct vfs_mount));
    if (!mount)
        return NULL;

    return mount;
}

static struct vfs_type s_ext2_type = {.name = "ext2", .mount = ext2_fs_mount};

void ext2_fs_init()
{
    virtual_fs_set_type(&s_ext2_type);
    virtual_fs_mount("/dev/hda", "/", s_ext2_type.name, 0x00, NULL);

    printf("Ext2 FS: Initialized\n");
}