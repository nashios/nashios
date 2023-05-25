#include <mqueue.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <unistd.h>

_syscall3(mq_open, const char *, int, struct mq_attr *);
mqd_t mq_open(const char *name, int oflag, ...)
{
    struct mq_attr *attr;

    va_list ap;
    va_start(ap, oflag);
    attr = va_arg(ap, struct mq_attr *);
    va_end(ap);

    SYSCALL_RETURN(syscall_mq_open(name, oflag, attr));
}

_syscall5(mq_timedsend, mqd_t, const char *, size_t, unsigned, const struct timespec *);
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime)
{
    SYSCALL_RETURN(syscall_mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, abstime));
}

int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio)
{
    return mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, NULL);
}

_syscall5(mq_timedreceive, mqd_t, char *, size_t, unsigned *, const struct timespec *);
ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abstime)
{
    SYSCALL_RETURN(syscall_mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, abstime));
}

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio)
{
    return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, NULL);
}

int mq_close(mqd_t mqdes) { return close(mqdes); }
