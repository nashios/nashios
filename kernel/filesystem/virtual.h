#pragma once

#include <kernel/api/posix/fcntl.h>
#include <kernel/api/posix/poll.h>
#include <kernel/api/posix/sys/stat.h>
#include <kernel/task/scheduler.h>
#include <st/dlist.h>
#include <stddef.h>
#include <stdint.h>

#define VFS_BYTES_P_SECTOR 512

#define FMODE_CAN_READ ((fmode_t)0x20000)
#define FMODE_CAN_WRITE ((fmode_t)0x40000)

#define ATTR_MODE (1 << 0)
#define ATTR_SIZE (1 << 3)

struct vfs_iattr
{
    unsigned int valid;
    uint32_t size;
    umode_t mode;
};

struct vfs_inode;
struct vfs_file;
struct vfs_poll;
struct vfs_file_op
{
    int (*open)(struct vfs_inode *inode, struct vfs_file *file);
    ssize_t (*read)(struct vfs_file *file, char *buf, size_t count, loff_t ppos);
    int (*mmap)(struct vfs_file *file, struct process_vm *memory);
    int (*release)(struct vfs_inode *inode, struct vfs_file *file);
    int (*poll)(struct vfs_file *file, struct vfs_poll *poll);
};

struct vfs_file
{
    fmode_t mode;
    loff_t position;
    struct vfs_file_op *op;
    struct vfs_dentry *dentry;
    struct vfs_mount *mount;
    void *data;
};

struct vfs_inode_op
{
    struct vfs_inode *(*create)(struct vfs_inode *inode, struct vfs_dentry *dentry, mode_t mode);
    struct vfs_inode *(*lookup)(struct vfs_inode *inode, struct vfs_dentry *dentry);
    int (*getattr)(struct vfs_mount *mount, struct vfs_dentry *dentry, struct stat *stat);
    int (*setattr)(struct vfs_dentry *dentry, struct vfs_iattr *iattr);
    int (*mknod)(struct vfs_inode *inode, struct vfs_dentry *dentry, int mode, dev_t dev);
};

struct vfs_inode_data
{
    uint32_t count;
    struct dlist_head list;
};

struct vfs_inode
{
    ino_t ino;
    dev_t rdev;
    umode_t mode;
    uint32_t size;
    uint32_t blocks;
    uint32_t block_size;
    struct vfs_inode_data data;
    struct vfs_inode_op *iop;
    struct vfs_file_op *fop;
    struct vfs_superblock *superblock;
    void *info;
};

struct vfs_dentry
{
    const char *name;
    struct vfs_inode *inode;
    struct dlist_head subdir;
    struct dlist_head sibling;
};

struct vfs_superblock_op
{
    struct vfs_inode *(*allocate_inode)(struct vfs_superblock *superblock);
    void (*read_inode)(struct vfs_inode *inode);
    void (*write_inode)(struct vfs_inode *inode);
    void (*write_super)(struct vfs_superblock *superblock);
};

struct vfs_superblock
{
    const char *device;
    uint32_t block_size;
    uint32_t block_size_bits;
    struct vfs_superblock_op *op;
    void *info;
};

struct vfs_mount
{
    struct vfs_dentry *root;
    struct vfs_dentry *mount;
    struct dlist_head list;
};

struct vfs_type
{
    const char *name;
    struct vfs_mount *(*mount)(const char *source, const char *target, const char *filesystemtype,
                               unsigned long mountflags, const void *data);
    struct dlist_head list;
};

struct vfs_poll
{
    struct dlist_head list;
};

void virtual_fs_init();
int virtual_fs_set_type(struct vfs_type *type);
char *virtual_fs_read_block(const char *source, sector_t sector, uint32_t size);
void virtual_fs_write_block(const char *source, sector_t sector, char *buffer, uint32_t size);
int virtual_fs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
                     const void *data);
struct vfs_inode *virtual_fs_create_inode(struct vfs_superblock *superblock);
struct vfs_dentry *virtual_fs_create_dentry(const char *name);
void virtual_fs_init_special_inode(struct vfs_inode *inode, umode_t mode, dev_t dev);
int virtual_fs_open(const char *pathname, int flags, mode_t mode);
int virtual_fs_fstat(int fd, struct stat *buf);
int virtual_fs_stat(const char *path, struct stat *buf);
ssize_t virtual_fs_read(int fd, void *buf, size_t count);
int virtual_fs_mknod(const char *pathname, mode_t mode, dev_t dev);
int virtual_fs_poll(struct pollfd fds[], nfds_t nfds, int timeout);
void virtual_fs_poll_wait(struct vfs_file *file, struct wait_queue *wait, struct vfs_poll *poll);
int virtual_fs_close(int fildes);
int virtual_fs_ftruncate(int fildes, off_t length);
