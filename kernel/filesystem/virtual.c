#include <kernel/api/posix/errno.h>
#include <kernel/drivers/ata.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/math.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>

struct vfs_nameidata
{
    struct vfs_mount *mount;
    struct vfs_dentry *dentry;
};

static struct dlist_head s_virtual_type_list;
static struct dlist_head s_virtual_mount_list;

char *virtual_fs_read_block(const char *source, sector_t sector, uint32_t size)
{
    struct ata_device *device = ata_find_device(source);
    if (!device)
        return NULL;

    uint8_t sectors = DIV_ROUND_UP(size, VFS_BYTES_P_SECTOR);
    char *buffer = (char *)calloc(sectors * VFS_BYTES_P_SECTOR, sizeof(char));
    if (!buffer)
        return NULL;

    if (ata_read(device, sector, sectors, (uint16_t *)buffer) < 0)
        return NULL;

    return buffer;
}

struct vfs_type *virtual_fs_find_type(const char *name)
{
    struct vfs_type *type;
    dlist_for_each_entry(type, &s_virtual_type_list, list)
    {
        if (strcmp(type->name, name) == 0)
            return type;
    }
    return NULL;
}

int virtual_fs_set_type(struct vfs_type *type)
{
    struct vfs_type *found = virtual_fs_find_type(type->name);
    if (found)
        return -EBUSY;

    dlist_add_tail(&type->list, &s_virtual_type_list);
    printf("Virtual FS: Registered filesystem type name = %s\n", type->name);

    return 0;
}

struct vfs_mount *virtual_fs_find_mount(struct vfs_dentry *dentry)
{
    struct vfs_mount *mount;
    dlist_for_each_entry(mount, &s_virtual_mount_list, list)
    {
        if (mount->dentry == dentry)
            return mount;
    }
    return NULL;
}

struct vfs_dentry *virtual_fs_create_dentry(const char *name)
{
    struct vfs_dentry *dentry = (struct vfs_dentry *)calloc(1, sizeof(struct vfs_dentry));
    if (!dentry)
        return NULL;
    dentry->name = strdup(name);

    dlist_head_init(&dentry->subdir);

    return dentry;
}

int virtual_fs_find_free_fd()
{
    for (int i = 0; i < 256; i++)
    {
        if (!g_scheduler_process->files->fd[i])
            return i;
    }
    return -EINVAL;
}

int virtual_fs_path_walk(struct vfs_nameidata *nameidata, const char *path, int flags, mode_t mode)
{
    nameidata->mount = g_scheduler_process->fs->mount;
    int i = 0;
    if (path[i] == '/')
    {
        nameidata->dentry = g_scheduler_process->fs->mount->dentry;
        while (path[i] == '/')
            i++;
    }
    else
        nameidata->dentry = g_scheduler_process->fs->dentry;

    char name[256] = {};
    int length = strlen(path);
    while (i < length)
    {
        memset(name, 0x00, sizeof(name));

        for (int j = 0; path[i] != '/' && i < length; i++, j++)
            name[j] = path[i];

        while (path[i] == '/' && i < length)
            i++;

        struct vfs_dentry *iter = NULL;
        struct vfs_dentry *child = NULL;
        dlist_for_each_entry(iter, &nameidata->dentry->subdir, sibling)
        {
            if (!strcmp(name, iter->name))
            {
                child = iter;
                break;
            }
        }

        if (child)
        {
            nameidata->dentry = child;
            if ((i == length) && (flags & O_CREAT) && (flags & O_EXCL))
                return -EEXIST;
        }
        else
        {
            if (!nameidata->dentry->inode->iop)
                return -EINVAL;

            child = virtual_fs_create_dentry(name);
            struct vfs_inode *inode = NULL;
            if (nameidata->dentry->inode->iop->lookup)
                inode = nameidata->dentry->inode->iop->lookup(nameidata->dentry->inode, child);

            if (inode == NULL)
            {
                if ((i == length) && (flags & O_CREAT) && !nameidata->dentry->inode->iop->create)
                    inode = nameidata->dentry->inode->iop->create(nameidata->dentry->inode, child,
                                                                  i == length ? mode : S_IFDIR);
                else
                    return -ENOENT;
            }
            else if ((i == length) && (flags & O_CREAT) && (flags & O_EXCL))
                return -EEXIST;

            child->inode = inode;
            dlist_add_tail(&child->sibling, &nameidata->dentry->subdir);
            nameidata->dentry = child;
        }

        struct vfs_mount *mount = virtual_fs_find_mount(nameidata->dentry);
        if (mount)
            nameidata->mount = mount;
    };

    return 0;
}

int virtual_fs_open(const char *pathname, int flags, mode_t mode)
{
    int fd = virtual_fs_find_free_fd();
    if (fd < 0)
        return fd;

    struct vfs_nameidata nameidata = {};
    int result = virtual_fs_path_walk(&nameidata, pathname, flags, mode);
    if (result < 0)
        return result;

    struct vfs_file *file = (struct vfs_file *)calloc(1, sizeof(struct vfs_file));
    if (!file)
        return -EINVAL;

    file->op = nameidata.dentry->inode->fop;
    if (file->op && file->op->open)
    {
        result = file->op->open(nameidata.dentry->inode, file);
        if (result < 0)
        {
            free(file);
            return result;
        }
    }

    g_scheduler_process->files->fd[fd] = file;

    return fd;
}

int virtual_fs_getattr(struct vfs_mount *mount, struct vfs_dentry *dentry, struct stat *stat)
{
    struct vfs_inode *inode = dentry->inode;
    if (inode->iop->getattr)
        return inode->iop->getattr(mount, dentry, stat);

    stat->st_size = inode->size;
    stat->st_blocks = inode->blocks;
    stat->st_blksize = inode->block_size;

    if (!stat->st_blksize)
    {
        struct vfs_superblock *superblock = inode->superblock;
        uint32_t blocks = (stat->st_size + superblock->block_size - 1) >> superblock->block_size_bits;
        stat->st_blocks = (superblock->block_size / VFS_BYTES_P_SECTOR) * blocks;
        stat->st_blksize = superblock->block_size;
    }

    return 0;
}

int virtual_fs_fstat(int fd, struct stat *buf)
{
    if (fd < 0)
        return -EBADF;

    struct vfs_file *file = g_scheduler_process->files->fd[fd];
    if (!file)
        return -EBADF;

    return virtual_fs_getattr(file->mount, file->dentry, buf);
}

int virtual_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
                     const void *data)
{
    struct ata_device *device = ata_find_device(source);
    if (!device)
        return -EINVAL;

    struct vfs_type *type = virtual_fs_find_type(filesystemtype);
    if (!type || !type->mount)
        return -EEXIST;

    struct vfs_mount *mount = type->mount(source, target, filesystemtype, mountflags, data);
    if (!mount)
        return -EINVAL;

    if (!strcmp(target, "/"))
    {
        g_scheduler_process->fs->mount = mount;
        g_scheduler_process->fs->dentry = mount->dentry;
    }

    dlist_add_tail(&mount->list, &s_virtual_mount_list);
    printf("Virtual FS: Mounted filesystem name = %s, source = %s, target = %s, flags = 0x%x\n", type->name, source,
           target, mountflags);
    return 0;
}

void virtual_fs_init()
{
    dlist_head_init(&s_virtual_type_list);
    dlist_head_init(&s_virtual_mount_list);

    printf("Virtual FS: Initialized\n");
}