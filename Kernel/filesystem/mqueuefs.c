#include <Kernel/api/posix/errno.h>
#include <Kernel/api/posix/mount.h>
#include <Kernel/filesystem/devfs.h>
#include <Kernel/filesystem/mqueuefs.h>
#include <Kernel/filesystem/virtual.h>
#include <Kernel/ipc/mqueue.h>
#include <Kernel/lib/stdio.h>
#include <Kernel/lib/stdlib.h>

static uint32_t s_mqueuefs_index = 0;

struct vfs_inode *mqueuefs_get_inode(struct vfs_superblock *superblock, mode_t mode);

struct vfs_inode *mqueuefs_allocate_inode(struct vfs_superblock *superblock)
{
    return virtual_fs_create_inode(superblock);
}

struct vfs_inode *mqueuefs_create_inode(struct vfs_inode *inode, struct vfs_dentry *, mode_t mode)
{
    return mqueuefs_get_inode(inode->superblock, mode);
}

int mqueuefs_open(struct vfs_inode *inode, struct vfs_file *file)
{
    struct mqueuefs_inode *mqueuefs_inode = inode->info;
    if (!mqueuefs_inode)
        return -EINVAL;

    struct mq_queue *queue = hashmap_get(&g_mq_hashmap, &mqueuefs_inode->key);
    if (!queue)
    {
        queue = (struct mq_queue *)calloc(1, sizeof(struct mq_queue));
        if (!queue)
            return -EINVAL;

        dlist_head_init(&queue->wait.list);
        dlist_head_init(&queue->messages);
        dlist_head_init(&queue->receivers);
        dlist_head_init(&queue->senders);

        if (!hashmap_put(&g_mq_hashmap, &mqueuefs_inode->key, queue))
            return -EINVAL;
    }

    file->data = queue;

    return 0;
}

int mqueuefs_poll(struct vfs_file *file, struct vfs_poll *poll)
{
    struct mq_queue *queue = (struct mq_queue *)file->data;
    if (!queue)
        return -EINVAL;

    virtual_fs_poll_wait(file, &queue->wait, poll);
    return (!dlist_empty(&queue->messages) ? POLLIN : 0) | POLLOUT;
}

static struct vfs_superblock_op s_mqueuefs_sop = {.allocate_inode = mqueuefs_allocate_inode};

static struct vfs_inode_op s_mqueuefs_file_iop = {};

static struct vfs_inode_op s_mqueuefs_dir_iop = {.create = mqueuefs_create_inode};

static struct vfs_file_op s_mqueuefs_file_op = {.open = mqueuefs_open, .poll = mqueuefs_poll};

static struct vfs_file_op s_mqueuefs_dir_op = {};

struct vfs_inode *mqueuefs_get_inode(struct vfs_superblock *superblock, mode_t mode)
{
    struct mqueuefs_inode *mqueuefs_inode = (struct mqueuefs_inode *)calloc(1, sizeof(struct mqueuefs_inode));
    if (!mqueuefs_inode)
        return NULL;

    mqueuefs_inode->key = s_mqueuefs_index++;

    struct vfs_inode *inode = superblock->op->allocate_inode(superblock);
    if (!inode)
        return NULL;

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
    mount->mount = dentry;
    mount->root = dentry;

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
