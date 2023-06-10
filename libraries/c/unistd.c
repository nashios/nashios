#include <sys/syscall.h>
#include <unistd.h>

int brk(void *addr) { return syscall(__NR_brk, addr); }

void *sbrk(intptr_t increment)
{
    void *value = (void *)brk(0);
    brk(value + increment);
    return value;
}

pid_t fork(void) { return syscall(__NR_fork); }

int execve(const char *path, char *const argv[], char *const envp[]) { return syscall(__NR_execve, path, argv, envp); }

int close(int fildes) {return syscall(__NR_close, fildes);}
