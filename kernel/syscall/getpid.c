#include <kernel/system/syscall.h>

pid_t syscall_getpid(void) { return g_scheduler_process->pid; }
