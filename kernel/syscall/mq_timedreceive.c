#include <kernel/system/syscall.h>

ssize_t syscall_mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio,
                                const struct timespec *abstime)
{
    return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, abstime);
}
