#include <kernel/api/posix/errno.h>
#include <kernel/filesystem/chardev.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/lib/stdio.h>

static struct dlist_head s_chardev_list = {};

struct chardev *chardev_find(dev_t dev);

int chardev_open(struct vfs_inode *inode, struct vfs_file *file)
{
    struct chardev *device = chardev_find(file->dentry->inode->rdev);
    if (device == NULL)
        return -ENODEV;

    if (device->op->open)
        return device->op->open(inode, file);

    return -EINVAL;
}

ssize_t chardev_read(struct vfs_file *file, char *buf, size_t count, loff_t ppos)
{
    struct chardev *device = chardev_find(file->dentry->inode->rdev);
    if (device == NULL)
        return -ENODEV;

    if (device->op->read)
        return device->op->read(file, buf, count, ppos);

    return -EINVAL;
}

int chardev_ioctl(struct vfs_inode *inode, struct vfs_file *file, unsigned int request, void *arg)
{
    struct chardev *device = chardev_find(file->dentry->inode->rdev);
    if (device == NULL)
        return -ENODEV;

    if (device->op->ioctl)
        return device->op->ioctl(inode, file, request, arg);

    return -EINVAL;
}

int chardev_mmap(struct vfs_file *file, struct process_vm *memory)
{
    struct chardev *device = chardev_find(file->dentry->inode->rdev);
    if (device == NULL)
        return -ENODEV;

    if (device->op->mmap)
        return device->op->mmap(file, memory);

    return -EINVAL;
}

int chardev_poll(struct vfs_file *file, struct vfs_poll *poll)
{
    struct chardev *device = chardev_find(file->dentry->inode->rdev);
    if (device == NULL)
        return -ENODEV;

    if (device->op->poll)
        return device->op->poll(file, poll);

    return -EINVAL;
}

struct vfs_file_op g_chardev_fop = {
    .open = chardev_open, .read = chardev_read, .ioctl = chardev_ioctl, .mmap = chardev_mmap, .poll = chardev_poll};

struct chardev *chardev_find(dev_t dev)
{
    struct chardev *iter = NULL;
    dlist_for_each_entry(iter, &s_chardev_list, list)
    {
        if (iter->dev == dev ||
            (MKDEV(iter->major, iter->baseminor) <= dev && dev < MKDEV(iter->major, iter->baseminor + iter->minorct)))
            return iter;
    };
    return NULL;
}

int chardev_set(struct chardev *device)
{
    struct chardev *found = chardev_find(device->dev);
    if (found)
        return -EEXIST;

    dlist_add_tail(&device->list, &s_chardev_list);
    return 0;
}

void chardev_init()
{
    dlist_head_init(&s_chardev_list);
    printf("Chardev FS: Initialized\n");
}
