#include <kernel/api/posix/errno.h>
#include <kernel/api/posix/mount.h>
#include <kernel/arch/i686/memory/virtual.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/filesystem/tmpfs.h>
#include <kernel/filesystem/virtual.h>
#include <st/debug.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>

struct page
{
    uint32_t physical;
    struct dlist_head list;
};

struct vfs_inode *tmpfs_get_inode(struct vfs_superblock *superblock, mode_t mode);

struct vfs_inode *tmpfs_create_inode(struct vfs_inode *inode, struct vfs_dentry *, mode_t mode)
{
    return tmpfs_get_inode(inode->superblock, mode | S_IFREG);
}

int tmpfs_mmap(struct vfs_file *file, struct process_vm *memory)
{
    struct vfs_inode *inode = file->dentry->inode;
    struct vfs_superblock *superblock = inode->superblock;

    struct page *page;
    uint32_t virtual_address = memory->start;
    dlist_for_each_entry(page, &inode->data.list, list)
    {
        if (virtual_address >= memory->end)
            break;

        virtual_mm_map(g_scheduler_process->directory, page->physical, virtual_address,
                       PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
        virtual_address += superblock->block_size;
    }
    return 0;
}

int tmpfs_setattr(struct vfs_dentry *dentry, struct vfs_iattr *iattr)
{
    struct vfs_inode *inode = dentry->inode;
    if (!inode)
        return -EINVAL;

    if (iattr->valid & ATTR_SIZE && iattr->size != inode->size)
    {
        uint32_t new_size = PAGE_ALIGN(iattr->size);
        uint32_t size = PAGE_ALIGN(inode->size);
        if (size < new_size)
        {
            uint32_t frames = (new_size - size) / PAGE_SIZE;
            for (uint32_t i = 0; i < frames; i++)
            {
                struct page *page = (struct page *)calloc(1, sizeof(struct page));
                if (!page)
                    return -ENOMEM;

                page->physical = (uint32_t)physical_mm_allocate();
                if (!page->physical)
                    return -ENOMEM;

                dlist_add_tail(&page->list, &inode->data.list);
            }
        }
        else if (size > new_size)
        {
            uint32_t frames = (size - new_size) / PAGE_SIZE;
            for (uint32_t i = 0; i < frames; i++)
                dlist_remove(inode->data.list.previous);
        }

        inode->data.count = new_size / PAGE_SIZE;
        inode->size = iattr->size;
    }

    if (iattr->valid & ATTR_MODE)
        inode->mode = iattr->mode;

    return 0;
}

struct vfs_inode_op s_tmpfs_file_iop = {.setattr = tmpfs_setattr};
struct vfs_file_op s_tmpfs_file_fop = {.mmap = tmpfs_mmap};
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
    struct vfs_inode *inode = virtual_fs_create_inode(superblock);
    if (!inode)
        return NULL;

    dlist_head_init(&inode->data.list);
    return inode;
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
    dbgln("TmpFS: Initialized");
}
