#pragma once

#include <kernel/api/posix/mqueue.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>

mqd_t mq_open(const char *name, int oflag, ...);
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime);
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abstime);
int mq_close(mqd_t mqdes);
