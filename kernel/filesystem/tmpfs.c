#include <kernel/api/posix/mount.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/filesystem/tmpfs.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>

struct vfs_inode *tmpfs_get_inode(struct vfs_superblock *superblock, mode_t mode);

struct vfs_inode *tmpfs_create_inode(struct vfs_inode *inode, struct vfs_dentry *, mode_t mode)
{
    return tmpfs_get_inode(inode->superblock, mode | S_IFREG);
}

struct vfs_inode_op s_tmpfs_file_iop = {};
struct vfs_file_op s_tmpfs_file_fop = {};
struct vfs_inode_op s_tmpfs_dir_iop = {.create = tmpfs_create_inode};
struct vfs_file_op s_tmpfs_dir_fop = {};
struct vfs_inode_op s_tmpfs_special_iop = {};

struct vfs_inode *tmpfs_get_inode(struct vfs_superblock *superblock, mode_t mode)
{
    struct vfs_inode *inode = superblock->op->allocate_inode(superblock);
    if (!inode)
        return NULL;

    inode->mode = mode;
    inode->block_size = PAGE_SIZE;

    if (S_ISREG(inode->mode))
    {
        inode->iop = &s_tmpfs_file_iop;
        inode->fop = &s_tmpfs_file_fop;
    }
    else if (S_ISDIR(inode->mode))
    {
        inode->iop = &s_tmpfs_dir_iop;
        inode->fop = &s_tmpfs_dir_fop;
    }
    else
    {
        inode->iop = &s_tmpfs_special_iop;
        virtual_fs_init_special_inode(inode, inode->mode, inode->rdev);
    }

    return inode;
}

struct vfs_inode *tmpfs_allocate_inode(struct vfs_superblock *superblock)
{
    return virtual_fs_create_inode(superblock);
}

static struct vfs_superblock_op s_tmpfs_super_operations = {
    .allocate_inode = tmpfs_allocate_inode,
};

struct vfs_mount *tmpfs_mount(const char *source, const char *target, const char *, unsigned long, const void *)
{
    struct vfs_dentry *dentry = virtual_fs_create_dentry(target);
    if (!dentry)
        return NULL;

    struct vfs_superblock *superblock = (struct vfs_superblock *)calloc(1, sizeof(struct vfs_superblock));
    if (!superblock)
        return NULL;
    superblock->device = strdup(source);
    superblock->block_size = PAGE_SIZE;
    superblock->op = &s_tmpfs_super_operations;

    dentry->inode = tmpfs_get_inode(superblock, S_IFDIR);

    struct vfs_mount *mount = (struct vfs_mount *)calloc(1, sizeof(struct vfs_mount));
    if (!mount)
        return NULL;

    mount->mount = dentry;
    mount->root = dentry;

    return mount;
}

static struct vfs_type s_tmpfs_type = {.name = "tmpfs", .mount = tmpfs_mount};

void tmpfs_init()
{
    virtual_fs_set_type(&s_tmpfs_type);
    virtual_fs_mknod("/dev/shm", O_RDWR, MKDEV(0, 24));
    virtual_fs_mount("tmpfs", "/dev/shm", s_tmpfs_type.name, MS_NOUSER, NULL);
    printf("TmpFS: Initialized\n");
}
