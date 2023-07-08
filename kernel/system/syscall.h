#pragma once

#include <kernel/api/posix/poll.h>
#include <kernel/api/posix/sys/stat.h>
#include <kernel/api/posix/sys/types.h>
#include <kernel/ipc/mqueue.h>
#include <stddef.h>

void syscall_exit(int status);
pid_t syscall_fork(void);
int syscall_execve(const char *path, char *const argv[], char *const envp[]);
int syscall_brk(void *addr);
void *syscall_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
mqd_t syscall_mq_open(const char *name, int oflag, struct mq_attr *attr);
int syscall_mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio,
                         const struct timespec *abstime);
ssize_t syscall_mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio,
                                const struct timespec *abstime);
pid_t syscall_getpid(void);
int syscall_poll(struct pollfd fds[], nfds_t nfds, int timeout);
int syscall_open(const char *path, int oflag, mode_t mode);
int syscall_close(int fildes);
int syscall_ftruncate(int fildes, off_t length);
int syscall_stat(const char *path, struct stat *buf);
int syscall_fstat(int fildes, struct stat *buf);
ssize_t syscall_read(int fildes, void *buf, size_t nbyte);
int syscall_ioctl(int fd, unsigned long request, void *arg);
ssize_t syscall_write(int fildes, const void *buf, size_t nbyte);
int syscall_fcntl(int fildes, int cmd, void *arg);

void syscall_init();
