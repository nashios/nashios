#pragma once

#include <kernel/api/posix/mqueue.h>
#include <kernel/api/posix/sys/types.h>
#include <kernel/api/posix/time.h>
#include <kernel/task/scheduler.h>
#include <st/hashmap.h>
#include <stddef.h>

struct mq_queue
{
    struct mq_attr *attr;
    struct wait_queue wait;
    struct dlist_head messages;
    struct dlist_head receivers;
    struct dlist_head senders;
};

extern struct hashmap g_mq_hashmap;

void mq_init();
mqd_t mq_open(const char *name, int oflag, struct mq_attr *attr);
mqd_t mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime);
ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abstime);
