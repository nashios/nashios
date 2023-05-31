#include <kernel/api/posix/fcntl.h>
#include <kernel/filesystem/mqueuefs.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/ipc/mqueue.h>
#include <kernel/lib/stdio.h>
#include <kernel/task/scheduler.h>

#define MAX_MQ_MESSAGES 32
#define MAX_MESSAGE_SIZE 512

struct mq_message
{
    char *buffer;
    uint32_t priority;
    size_t size;
    struct dlist_head list;
};

struct mq_receiver
{
    struct thread *thread;
    struct dlist_head list;
};

struct mq_sender
{
    struct thread *thread;
    uint32_t priority;
    struct dlist_head list;
};

struct hashmap g_mq_hashmap = {};

mqd_t mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *)
{
    struct vfs_file *file = g_scheduler_thread->process->files->fd[mqdes];
    if (!file)
        return -EBADF;

    struct mq_queue *queue = (struct mq_queue *)file->data;
    if (!queue)
        return -EBADF;

    if (!(file->mode & FMODE_CAN_WRITE))
        return -EBADF;

    if (msg_len > (size_t)queue->attr->mq_msgsize)
        return -EMSGSIZE;

    assert(queue->attr->mq_curmsgs <= queue->attr->mq_maxmsg);
    if (queue->attr->mq_curmsgs == queue->attr->mq_maxmsg)
    {
        if ((queue->attr->mq_flags & O_NONBLOCK) == 0)
        {
            struct mq_sender *sender;
            bool condition = dlist_is_poison(&sender->list) && queue->attr->mq_curmsgs < queue->attr->mq_maxmsg;
            WAIT_BEFORE_AFTER(condition, ({
                                  sender = (struct mq_sender *)calloc(1, sizeof(struct mq_sender));
                                  sender->thread = g_scheduler_thread;
                                  sender->priority = msg_prio;

                                  struct mq_sender *iter;
                                  dlist_for_each_entry(iter, &queue->senders, list)
                                  {
                                      if (sender->priority > iter->priority)
                                          break;
                                  }

                                  if (&iter->list == &queue->senders)
                                      dlist_add_tail(&sender->list, &queue->senders);
                                  else
                                      dlist_add(&sender->list, iter->list.previous);
                              }),
                              free(sender));
        }
        else
            return -EAGAIN;
    }
    assert(queue->attr->mq_curmsgs < queue->attr->mq_maxmsg);

    char *buffer = (char *)calloc(msg_len, sizeof(char));
    if (!buffer)
        return -ENOMEM;

    memcpy(buffer, msg_ptr, msg_len);

    struct mq_message *message = (struct mq_message *)calloc(1, sizeof(struct mq_message));
    if (!message)
        return -ENOMEM;

    message->buffer = buffer;
    message->size = msg_len;
    message->priority = msg_prio;

    struct mq_message *iter;
    dlist_for_each_entry(iter, &queue->messages, list)
    {
        if (message->priority > iter->priority)
            break;
    }

    if (&iter->list == &queue->messages)
        dlist_add_tail(&message->list, &queue->messages);
    else
        dlist_add(&message->list, iter->list.previous);
    queue->attr->mq_curmsgs += 1;

    struct mq_receiver *receiver = dlist_first_entry_or_null(&queue->receivers, struct mq_receiver, list);
    if (receiver)
    {
        assert(queue->attr->mq_curmsgs == 1);
        dlist_remove(&receiver->list);
        scheduler_update_thread(receiver->thread, THREAD_READY_STATE);
    }

    scheduler_wake_up(&queue->wait);

    return 0;
}

mqd_t mq_open(const char *name, int oflag, struct mq_attr *attr)
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
        struct mq_attr *new_attr = (struct mq_attr *)calloc(1, sizeof(struct mq_attr));
        if (!new_attr)
            return -ENOMEM;

        new_attr->mq_flags = oflag;
        new_attr->mq_maxmsg = attr ? attr->mq_maxmsg : MAX_MQ_MESSAGES;
        new_attr->mq_msgsize = attr ? attr->mq_msgsize : MAX_MESSAGE_SIZE;
        queue->attr = new_attr;
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
