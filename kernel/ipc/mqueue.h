#pragma once

#include <kernel/api/posix/mqueue.h>
#include <kernel/api/posix/sys/types.h>
#include <kernel/api/posix/time.h>
#include <st/hashmap.h>
#include <stddef.h>

struct mq_queue
{
    struct mq_attr *attr;
    struct dlist_head wait;
    struct dlist_head messages;
};

extern struct hashmap g_mq_hashmap;

void mq_init();
mqd_t mq_open(const char *name, int oflag, struct mq_attr *attr);
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime);
