#include <kernel/system/syscall.h>

int syscall_execve(const char *path, char *const argv[], char *const envp[])
{
    return scheduler_execve(path, argv, envp);
}
