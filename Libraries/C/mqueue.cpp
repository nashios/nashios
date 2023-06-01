#include <mqueue.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

extern "C"
{

    _syscall3(mq_open, const char *, int, struct mq_attr *);
    mqd_t mq_open(const char *name, int oflag, ...)
    {
        va_list ap;
        va_start(ap, oflag);
        struct mq_attr *attr = va_arg(ap, struct mq_attr *);
        va_end(ap);

        static const char default_dir[] = "/dev/mqueue/";

        char *p_name;
        if (name[0] == '/')
            p_name = (char *)name;
        else
        {
            p_name = (char *)calloc(strlen(name) + sizeof(default_dir), sizeof(char));
            strcpy(p_name, default_dir);
            strcat(p_name, name);
        }

        SYSCALL_RETURN(syscall_mq_open(p_name, oflag, attr));
    }

    _syscall5(mq_timedsend, mqd_t, const char *, size_t, unsigned, const struct timespec *);
    int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio,
                     const struct timespec *abstime)
    {
        SYSCALL_RETURN(syscall_mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, abstime));
    }

    int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio)
    {
        struct timespec time = {};
        return mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, &time);
    }

    _syscall5(mq_timedreceive, mqd_t, char *, size_t, unsigned *, const struct timespec *);
    ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio,
                            const struct timespec *abstime)
    {
        SYSCALL_RETURN(syscall_mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, abstime));
    }

    ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio)
    {
        struct timespec time = {};
        return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, &time);
    }

    int mq_close(mqd_t mqdes) { return close(mqdes); }
}
