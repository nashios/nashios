#include <kernel/system/syscall.h>

mqd_t syscall_mq_open(const char *name, int oflag, struct mq_attr *attr) { return mq_open(name, oflag, attr); }
