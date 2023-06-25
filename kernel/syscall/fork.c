#include <kernel/system/syscall.h>

pid_t syscall_fork(void)
{
    struct process *process = scheduler_fork(g_scheduler_process);
    scheduler_queue_thread(process->thread);
    return process->pid;
}
