#include <kernel/drivers/char/debug.h>
#include <kernel/filesystem/chardev.h>
#include <kernel/filesystem/devfs.h>
#include <kernel/lib/stdio.h>

int debug_open(struct vfs_inode *, struct vfs_file *) { return 0; }

ssize_t debug_write(struct vfs_file *, const char *buffer, size_t count, loff_t)
{
    printf(buffer, count);
    printf("\n");

    return 0;
}

static struct vfs_file_op s_debug_fop = {.open = debug_open, .write = debug_write};

static struct chardev s_debug_chardev = {
    .dev = MKDEV(1, 0), .major = 1, .baseminor = 0, .minorct = 11, .op = &s_debug_fop};

void debug_init()
{
    chardev_set(&s_debug_chardev);
    virtual_fs_mknod("/dev/debug", S_IFCHR, s_debug_chardev.dev);
    printf("Debug: Initialized\n");
}
