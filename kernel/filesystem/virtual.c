#include <kernel/api/posix/errno.h>
#include <kernel/drivers/ata.h>
#include <kernel/filesystem/chardev.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>
#include <kernel/lib/string.h>
#include <st/math.h>

#define FMODE_READ ((fmode_t)0x1)
#define FMODE_WRITE ((fmode_t)0x2)
#define FMODE_NONOTIFY ((fmode_t)0x4000000)

#define OPEN_FMODE(flag) ((fmode_t)(((flag + 1) & O_ACCMODE) | (flag & FMODE_NONOTIFY)))

struct vfs_nameidata
{
    struct vfs_mount *mount;
    struct vfs_dentry *dentry;
};

struct vfs_poll_entry
{
    struct vfs_file *file;
    struct wait_queue_entry wait;
    struct dlist_head list;
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

void virtual_fs_write_block(const char *source, sector_t sector, char *buffer, uint32_t size)
{
    struct ata_device *device = ata_find_device(source);
    if (!device)
        return;

    if (ata_write(device, sector, DIV_ROUND_UP(size, VFS_BYTES_P_SECTOR), (uint16_t *)buffer) < 0)
        return;
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
        if (mount->mount == dentry)
            return mount;
    }
    return NULL;
}

struct vfs_inode *virtual_fs_create_inode(struct vfs_superblock *superblock)
{
    struct vfs_inode *inode = (struct vfs_inode *)calloc(1, sizeof(struct vfs_inode));
    if (!inode)
        return NULL;
    inode->superblock = superblock;

    return inode;
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

void virtual_fs_init_special_inode(struct vfs_inode *inode, umode_t mode, dev_t dev)
{
    inode->mode = mode;

    if (S_ISCHR(mode))
    {
        inode->fop = &g_chardev_fop;
        inode->rdev = dev;
    }
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
        nameidata->dentry = g_scheduler_process->fs->mount->root;
        while (path[i] == '/')
            i++;
    }
    else
        nameidata->dentry = g_scheduler_process->fs->root;

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
                if ((i == length) && (flags & O_CREAT))
                {
                    if (!nameidata->dentry->inode->iop->create)
                        return -EINVAL;

                    inode = nameidata->dentry->inode->iop->create(nameidata->dentry->inode, child,
                                                                  i == length ? mode : S_IFDIR);
                }

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

    file->dentry = nameidata.dentry;
    file->mount = nameidata.mount;
    file->mode = OPEN_FMODE(flags);
    file->op = file->dentry->inode->fop;

    if (file->mode & FMODE_READ)
        file->mode |= FMODE_CAN_READ;
    if (file->mode & FMODE_WRITE)
        file->mode |= FMODE_CAN_WRITE;

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

int virtual_fs_stat(const char *path, struct stat *buf)
{
    struct vfs_nameidata nameidata = {};
    int result = virtual_fs_path_walk(&nameidata, path, 0, S_IFREG);
    if (result < 0)
        return result;

    return virtual_fs_getattr(nameidata.mount, nameidata.dentry, buf);
}

ssize_t virtual_fs_read(int fd, void *buf, size_t count)
{
    if (fd < 0)
        return -EBADF;

    struct vfs_file *file = g_scheduler_process->files->fd[fd];
    if (!file)
        return -EBADF;

    if ((file->mode & FMODE_CAN_READ) && file->op->read)
        return file->op->read(file, buf, count, file->position);

    return -EINVAL;
}

int virtual_fs_mknod(const char *pathname, mode_t mode, dev_t dev)
{
    const char *found = strrstr(pathname, "/");
    if (!found)
        return -EINVAL;

    size_t position = found - pathname;
    const char *dir = NULL;
    if (position)
    {
        dir = (const char *)calloc(position + 1, sizeof(char));
        memcpy((void *)dir, pathname, position);
    }

    struct vfs_nameidata nameidata = {};
    int result = virtual_fs_path_walk(&nameidata, dir, 0, S_IFDIR);
    if (result < 0)
        return result;

    size_t length = strlen(pathname);
    const char *name = NULL;
    if (position < length)
    {
        name = (const char *)calloc(length - position, sizeof(char));
        memcpy((void *)name, pathname + position + 1, length - 1 - position);
    }

    if (!nameidata.dentry->inode->iop->mknod)
        return -EINVAL;

    struct vfs_dentry *dentry = virtual_fs_create_dentry(name);
    result = nameidata.dentry->inode->iop->mknod(nameidata.dentry->inode, dentry, mode, dev);
    if (result < 0)
        return result;

    dlist_add_tail(&dentry->sibling, &nameidata.dentry->subdir);
    return result;
}

void virtual_fs_poll_free(struct vfs_poll *poll)
{
    struct vfs_poll_entry *iter;
    struct vfs_poll_entry *next;

    dlist_for_each_entry_safe(iter, next, &poll->list, list)
    {
        dlist_remove(&iter->wait.list);
        dlist_remove(&iter->list);
        free(iter);
    }
    free(poll);
}

void virtual_fs_poll_wakeup(struct thread *thread) { scheduler_update_thread(thread, THREAD_READY_STATE); }

void virtual_fs_poll_wait(struct vfs_file *file, struct wait_queue *wait, struct vfs_poll *poll)
{
    struct vfs_poll_entry *entry = calloc(1, sizeof(struct vfs_poll_entry));
    if (!entry)
        return;

    entry->file = file;
    entry->wait.handler = virtual_fs_poll_wakeup;
    entry->wait.thread = g_scheduler_thread;

    dlist_add_tail(&entry->wait.list, &wait->list);
    dlist_add_tail(&entry->list, &poll->list);
}

int virtual_fs_poll(struct pollfd fds[], nfds_t nfds, int)
{
    int result;
    while (true)
    {
        struct vfs_poll *poll = (struct vfs_poll *)calloc(1, sizeof(struct vfs_poll));
        if (!poll)
            return -ENOMEM;

        result = 0;
        dlist_head_init(&poll->list);
        for (uint32_t i = 0; i < nfds; i++)
        {
            struct pollfd *p_fd = &fds[i];

            if (p_fd->fd < 0)
            {
                p_fd->revents = 0;
                continue;
            }

            struct vfs_file *file = g_scheduler_thread->process->files->fd[p_fd->fd];
            if (!file || !file->op->poll)
                continue;

            p_fd->revents = file->op->poll(file, poll);
            if (p_fd->events & p_fd->revents)
                result++;
        }

        if (result > 0)
        {
            virtual_fs_poll_free(poll);
            break;
        }

        scheduler_update_thread(g_scheduler_thread, THREAD_WAITING_STATE);
        scheduler_schedule();
        virtual_fs_poll_free(poll);
    }
    return result;
}

int virtual_fs_close(int fildes)
{
    struct vfs_file *file = g_scheduler_process->files->fd[fildes];
    if (!file)
        return -EBADF;

    int result = 0;
    if (file->op && file->op->release)
        result = file->op->release(file->dentry->inode, file);
    free(file);

    g_scheduler_process->files->fd[fildes] = NULL;
    return result;
}

int virtual_fs_setattr(struct vfs_dentry *dentry, struct vfs_iattr *iattr)
{
    struct vfs_inode *inode = dentry->inode;
    if (!inode)
        return -EINVAL;

    int result = 0;
    if (inode->iop->setattr)
        result = inode->iop->setattr(dentry, iattr);
    else
    {
        if (iattr->valid & ATTR_SIZE)
            inode->size = iattr->size;
        if (iattr->valid & ATTR_MODE)
            inode->mode = iattr->mode;
    }

    return result;
}

int virtual_fs_truncate(struct vfs_dentry *dentry, int32_t length)
{
    struct vfs_iattr *iattr = (struct vfs_iattr *)calloc(1, sizeof(struct vfs_iattr));
    if (!iattr)
        return -ENOMEM;

    iattr->valid = ATTR_SIZE;
    iattr->size = length;

    return virtual_fs_setattr(dentry, iattr);
}

int virtual_fs_ftruncate(int fildes, off_t length)
{
    struct vfs_file *file = g_scheduler_process->files->fd[fildes];
    if (!file)
        return -EBADFD;

    return virtual_fs_truncate(file->dentry, length);
}

int virtual_fs_ioctl(int fd, unsigned long request, void *arg)
{
    struct vfs_file *file = g_scheduler_process->files->fd[fd];
    if (!file)
        return -EINVAL;

    if (file->op->ioctl)
        return file->op->ioctl(file->dentry->inode, file, request, arg);

    return -EINVAL;
}

ssize_t virtual_fs_write(int fildes, const void *buffer, size_t nbyte)
{
    if (fildes < 0)
        return -EBADF;

    struct vfs_file *file = g_scheduler_process->files->fd[fildes];
    if (!file)
        return -EBADF;

    loff_t ppos = file->position;
    if (file->flags & O_APPEND)
        ppos = file->dentry->inode->size;

    if (file->mode & FMODE_CAN_WRITE)
        return file->op->write(file, buffer, nbyte, ppos);

    return -EINVAL;
}

int virtual_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
                     const void *data)
{
    struct vfs_type *type = virtual_fs_find_type(filesystemtype);
    if (!type || !type->mount)
        return -EEXIST;

    bool mounting_root = strcmp(target, "/") == 0;
    struct vfs_mount *mount = NULL;
    if (mounting_root)
    {
        struct ata_device *device = ata_find_device(source);
        if (!device)
            return -EINVAL;

        mount = type->mount(source, target, filesystemtype, mountflags, data);
        if (!mount)
            return -EINVAL;

        g_scheduler_process->fs->mount = mount;
        g_scheduler_process->fs->root = mount->root;
    }
    else
    {
        const char *found = strrstr(target, "/");
        if (!found)
            return -EINVAL;

        size_t position = found - target;
        const char *dir = NULL;
        if (position)
        {
            dir = (const char *)calloc(position + 1, sizeof(char));
            memcpy((void *)dir, target, position);
        }
        if (!dir)
            dir = "/";

        size_t length = strlen(target);
        const char *name = NULL;
        if (position < length)
        {
            name = (const char *)calloc(length - position, sizeof(char));
            memcpy((void *)name, target + position + 1, length - 1 - position);
        }

        mount = type->mount(source, name, filesystemtype, mountflags, data);
        if (!mount)
            return -EINVAL;

        struct vfs_nameidata nameidata = {};
        int result = virtual_fs_path_walk(&nameidata, dir, O_RDONLY, S_IFDIR);
        if (result < 0)
            return result;

        struct vfs_dentry *iter, *next;
        dlist_for_each_entry_safe(iter, next, &nameidata.dentry->subdir, sibling)
        {
            if (!strcmp(iter->name, name))
            {
                dlist_remove(&iter->sibling);
                free(iter);
            }
        }

        dlist_add_tail(&mount->mount->sibling, &nameidata.dentry->subdir);
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
