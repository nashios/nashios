#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

char **environ = NULL;

_syscall1(exit, int);
void __attribute__((noreturn)) _exit(int status)
{
    syscall_exit(status);
    __builtin_unreachable();
}

_syscall0(fork);
pid_t fork(void) { SYSCALL_RETURN(syscall_fork()); }

int execl(const char *path, const char *arg0, ...) {}

int execle(const char *path, const char *arg0, ...) {}

int execlp(const char *file, const char *arg0, ...) {}

int execv(const char *path, char *const argv[]) {}

_syscall3(execve, const char *, char *const *, char *const *);
int execve(const char *path, char *const argv[], char *const envp[])
{
    SYSCALL_RETURN(syscall_execve(path, argv, envp));
}

int execvp(const char *file, char *const argv[]) {}

int fexecve(int fd, char *const argv[], char *const envp[]) {}

_syscall1(brk, void *);
int brk(void *addr) { SYSCALL_RETURN(syscall_brk(addr)); }

void *sbrk(intptr_t incr)
{
    void *value = (void *)brk(0);
    brk(value + incr);
    return value;
}

_syscall3(read, int, void *, size_t);
ssize_t read(int fildes, void *buf, size_t nbyte) { SYSCALL_RETURN(syscall_read(fildes, buf, nbyte)); }

_syscall1(close, int);
int close(int fildes) { SYSCALL_RETURN(syscall_close(fildes)); }
