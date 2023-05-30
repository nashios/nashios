#include <kernel/api/posix/fcntl.h>
#include <kernel/filesystem/mqueuefs.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/ipc/mqueue.h>
#include <kernel/lib/stdio.h>
#include <kernel/task/scheduler.h>

#define MAX_MQ_MESSAGES 32
#define MAX_MESSAGE_SIZE 512

struct hashmap g_mq_hashmap = {};

int mq_open(const char *name, int oflag, struct mq_attr *attr)
{
    int fd = virtual_fs_open(name, oflag, S_IFREG);
    if (fd < 0)
        return fd;

    struct vfs_file *file = g_scheduler_process->files->fd[fd];
    if (!file)
        return -EINVAL;

    struct mqueuefs_inode *inode = file->dentry->inode->info;
    if (!inode)
        return -EINVAL;

    struct mq_queue *queue = hashmap_get(&g_mq_hashmap, &inode->key);
    assert(queue);

    if (oflag & O_CREAT && !queue->attr)
    {
        struct mq_attr *attr = (struct mq_attr *)calloc(1, sizeof(struct mq_attr));
        attr->mq_flags = oflag;
        attr->mq_maxmsg = attr ? attr->mq_maxmsg : MAX_MQ_MESSAGES;
        attr->mq_msgsize = attr ? attr->mq_msgsize : MAX_MESSAGE_SIZE;
        queue->attr = attr;
    }
    else if (attr && (queue->attr->mq_maxmsg != attr->mq_maxmsg || queue->attr->mq_msgsize != attr->mq_msgsize))
        return -EINVAL;

    return fd;
}

void mq_init()
{
    hashmap_init(&g_mq_hashmap, hashmap_hash_uint32, hashmap_compare_uint32, 0);
    printf("MQ: Initialized\n");
}
