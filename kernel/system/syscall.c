#include <kernel/api/posix/errno.h>
#include <kernel/api/posix/sys/syscall.h>
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

void *syscall_mmap(void *addr, size_t len, int prot, int flags, int fildes)
{
    return mmap(addr, len, prot, flags, fildes, 0);
}

mqd_t syscall_mq_open(const char *name, int oflag, struct mq_attr *attr) { return mq_open(name, oflag, attr); }

pid_t syscall_getpid(void) { return g_scheduler_process->pid; }

static void *s_syscall_list[] = {
    [__NR_exit] = syscall_exit,      [__NR_fork] = syscall_fork, [__NR_execve] = syscall_execve,
    [__NR_getpid] = syscall_getpid,  [__NR_brk] = syscall_brk,   [__NR_mmap] = syscall_mmap,
    [__NR_mq_open] = syscall_mq_open};

bool syscall_handler(struct itr_registers *registers)
{
    uint32_t number = registers->eax;
    uint32_t (*handler)(uint32_t, ...) = s_syscall_list[number];
    if (!handler)
    {
        printf("Syscall: Called syscall number = %d without handler\n", number);
        registers->eax = -EINVAL;
        return ITR_STOP;
    }

    memcpy(&g_scheduler_thread->registers, registers, sizeof(struct itr_registers));
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
