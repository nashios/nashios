#include <kernel/api/posix/errno.h>
#include <kernel/api/posix/sys/syscall.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/interrupts/handler.h>
#include <kernel/ipc/mqueue.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/string.h>
#include <kernel/memory/mmap.h>
#include <kernel/system/syscall.h>
#include <kernel/task/scheduler.h>

void syscall_exit(int status) { scheduler_exit(status); }

pid_t syscall_fork(void)
{
    struct process *process = scheduler_fork(g_scheduler_process);
    scheduler_queue_thread(process->thread);
    return process->pid;
}

int syscall_execve(const char *path, char *const argv[], char *const envp[])
{
    return scheduler_execve(path, argv, envp);
}

int syscall_brk(void *addr)
{
    uint32_t p_addr = (uint32_t)addr;
    if (p_addr == 0)
        return g_scheduler_process->memory->brk_middle;
    return mmap_brk(p_addr);
}

void *syscall_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    return mmap(addr, len, prot, flags, fildes, off);
}

mqd_t syscall_mq_open(const char *name, int oflag, struct mq_attr *attr) { return mq_open(name, oflag, attr); }

int syscall_mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio,
                         const struct timespec *abstime)
{
    return mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, abstime);
}

ssize_t syscall_mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio,
                                const struct timespec *abstime)
{
    return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, abstime);
}

pid_t syscall_getpid(void) { return g_scheduler_process->pid; }

int syscall_poll(struct pollfd fds[], nfds_t nfds, int timeout) { return virtual_fs_poll(fds, nfds, timeout); }

int syscall_open(const char *path, int oflag, mode_t mode) { return virtual_fs_open(path, oflag, mode); }

int syscall_close(int fildes) { return virtual_fs_close(fildes); }

int syscall_ftruncate(int fildes, off_t length) { return virtual_fs_ftruncate(fildes, length); }

int syscall_stat(const char *path, struct stat *buf) { return virtual_fs_stat(path, buf); }

int syscall_fstat(int fildes, struct stat *buf) { return virtual_fs_fstat(fildes, buf); }

ssize_t syscall_read(int fildes, void *buf, size_t nbyte) { return virtual_fs_read(fildes, buf, nbyte); }

int syscall_ioctl(int fd, unsigned long request, void *arg) { return virtual_fs_ioctl(fd, request, arg); }

ssize_t syscall_write(int fildes, const void *buf, size_t nbyte) { return virtual_fs_write(fildes, buf, nbyte);  }

static void *s_syscall_list[] = {[__NR_exit] = syscall_exit,
                                 [__NR_fork] = syscall_fork,
                                 [__NR_read] = syscall_read,
                                 [__NR_open] = syscall_open,
                                 [__NR_close] = syscall_close,
                                 [__NR_execve] = syscall_execve,
                                 [__NR_getpid] = syscall_getpid,
                                 [__NR_brk] = syscall_brk,
                                 [__NR_ioctl] = syscall_ioctl,
                                 [__NR_mmap] = syscall_mmap,
                                 [__NR_ftruncate] = syscall_ftruncate,
                                 [__NR_stat] = syscall_stat,
                                 [__NR_fstat] = syscall_fstat,
                                 [__NR_poll] = syscall_poll,
                                 [__NR_mq_open] = syscall_mq_open,
                                 [__NR_mq_timedsend] = syscall_mq_timedsend,
                                 [__NR_mq_timedreceive] = syscall_mq_timedreceive};

bool syscall_handler(struct itr_registers *registers)
{
    uint32_t number = registers->eax;
    uint32_t size = sizeof(s_syscall_list) / sizeof(s_syscall_list[0]);
    if (number >= size || !s_syscall_list[number])
    {
        printf("Syscall: Called syscall number = %d without handler\n", number);
        registers->eax = -ENOSYS;
        return ITR_STOP;
    }

    memcpy(&g_scheduler_thread->registers, registers, sizeof(struct itr_registers));
    uint32_t (*handler)(uint32_t, ...) = s_syscall_list[number];
    uint32_t result = handler(registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi);
    registers->eax = result;
    printf("Syscall: Called syscall number = %d, result = %d\n", number, result);

    return ITR_CONTINUE;
}

void syscall_init()
{
    itr_set_handler(128, syscall_handler);
    printf("Syscall: Initialized\n");
}
