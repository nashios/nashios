#include <kernel/api/posix/mount.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/filesystem/mqueuefs.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>

static uint32_t s_mqueuefs_index = 0;

struct vfs_inode *mqueuefs_allocate_inode(struct vfs_superblock *superblock)
{
    return virtual_fs_create_inode(superblock);
}

static struct vfs_superblock_op s_mqueuefs_sop = {.allocate_inode = mqueuefs_allocate_inode};
static struct vfs_inode_op s_mqueuefs_file_iop = {};
static struct vfs_inode_op s_mqueuefs_dir_iop = {};
static struct vfs_file_op s_mqueuefs_file_op = {};
static struct vfs_file_op s_mqueuefs_dir_op = {};

struct vfs_inode *mqueuefs_get_inode(struct vfs_superblock *superblock, mode_t mode)
{
    struct mqueuefs_inode *mqueuefs_inode = (struct mqueuefs_inode *)calloc(1, sizeof(struct mqueuefs_inode));
    mqueuefs_inode->key = s_mqueuefs_index++;

    struct vfs_inode *inode = superblock->op->allocate_inode(superblock);
    inode->block_size = PAGE_SIZE;
    inode->mode = mode;
    inode->info = mqueuefs_inode;

    if (S_ISREG(inode->mode))
    {
        inode->iop = &s_mqueuefs_file_iop;
        inode->fop = &s_mqueuefs_file_op;
    }
    else if (S_ISDIR(inode->mode))
    {
        inode->iop = &s_mqueuefs_dir_iop;
        inode->fop = &s_mqueuefs_dir_op;
    }

    return inode;
}

struct vfs_mount *mqueuefs_mount(const char *, const char *target, const char *, unsigned long, const void *)
{
    struct vfs_superblock *superblock = (struct vfs_superblock *)calloc(1, sizeof(struct vfs_superblock));
    superblock->op = &s_mqueuefs_sop;
    superblock->block_size = PAGE_SIZE;

    struct vfs_dentry *dentry = virtual_fs_create_dentry(target);
    dentry->inode = mqueuefs_get_inode(superblock, S_IFDIR);

    struct vfs_mount *mount = (struct vfs_mount *)calloc(1, sizeof(struct vfs_mount));
    mount->dentry = dentry;

    return mount;
}

static struct vfs_type s_mqueuefs_type = {.name = "mqueuefs", .mount = mqueuefs_mount};

void mqueuefs_init()
{
    virtual_fs_set_type(&s_mqueuefs_type);
    virtual_fs_mknod("/dev/mqueue", O_RDWR, MKDEV(0, 19));
    virtual_fs_mount("mqueuefs", "/dev/mqueue", s_mqueuefs_type.name, MS_NOUSER, NULL);

    printf("Mqueue FS: Initialized\n");
}
