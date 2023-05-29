#include <kernel/api/posix/mount.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>

struct vfs_inode *devfs_allocate_inode(struct vfs_superblock *superblock)
{
    return virtual_fs_create_inode(superblock);
}

static struct vfs_inode_op s_devfs_file_iop = {};
static struct vfs_file_op s_devfs_file_fop = {};
static struct vfs_inode_op s_devfs_dir_iop = {};
static struct vfs_file_op s_devfs_dir_fop = {};
static struct vfs_inode_op s_devfs_special_iop = {};

static struct vfs_superblock_op s_devfs_sop = {.allocate_inode = devfs_allocate_inode};

struct vfs_inode *devfs_create_inode(struct vfs_superblock *superblock, mode_t mode)
{
    struct vfs_inode *inode = superblock->op->allocate_inode(superblock);
    if (!inode)
        return NULL;
    inode->mode = mode;

    if (S_ISDIR(inode->mode))
    {
        inode->iop = &s_devfs_dir_iop;
        inode->fop = &s_devfs_dir_fop;
    }
    else if (S_ISREG(inode->mode))
    {
        inode->iop = &s_devfs_file_iop;
        inode->fop = &s_devfs_file_fop;
    }
    else
    {
        inode->iop = &s_devfs_special_iop;
        virtual_fs_init_special_inode(inode, inode->mode, inode->rdev);
    }

    return inode;
}

struct vfs_mount *devfs_mount(const char *source, const char *target, const char *, unsigned long, const void *)
{
    struct vfs_superblock *superblock = (struct vfs_superblock *)calloc(1, sizeof(struct vfs_superblock));
    if (!superblock)
        return NULL;
    superblock->op = &s_devfs_sop;
    superblock->block_size = PAGE_SIZE;
    superblock->device = strdup(source);

    struct vfs_dentry *dentry = virtual_fs_create_dentry(target);
    if (!dentry)
        return NULL;

    dentry->inode = devfs_create_inode(superblock, S_IFDIR);
    if (!dentry->inode)
        return NULL;

    struct vfs_mount *mount = (struct vfs_mount *)calloc(1, sizeof(struct vfs_mount));
    mount->dentry = dentry;

    return mount;
}

static struct vfs_type s_devfs_type = {.name = "devfs", .mount = devfs_mount};

void devfs_init()
{
    virtual_fs_set_type(&s_devfs_type);
    virtual_fs_mount("devfs", "/dev", "devfs", MS_NOUSER, NULL);

    printf("DevFS: Initialized\n");
}
