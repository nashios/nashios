#include <stdbool.h>
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

int close(int fildes) { return syscall(__NR_close, fildes); }

int ftruncate(int fildes, off_t length) { return syscall(__NR_ftruncate, fildes, length); }

pid_t getpid(void) { return syscall(__NR_getpid); }

ssize_t read(int fildes, void *buf, size_t nbyte) { return syscall(__NR_read, fildes, buf, nbyte); }

ssize_t write(int fildes, const void *buf, size_t nbyte) { return syscall(__NR_write, fildes, buf, nbyte); }

int dup(int fildes) { return syscall(__NR_dup, fildes); }

int dup2(int fildes, int fildes2) { return syscall(__NR_dup2, fildes, fildes2); }

void _exit(int status)
{
    while (true)
        syscall(__NR_exit, status);
}