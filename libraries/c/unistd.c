#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <unistd.h>

char **environ;

int execve_get_argc(va_list list, const char *arg)
{
    int argc = arg ? 1 : 0;
    while (arg && va_arg(list, const char *))
        argc++;
    return argc;
}

char **execve_get_argv(va_list list, int argc, const char *arg)
{
    char **argv = calloc(argc + 1, sizeof(char *));
    argv[0] = (char *)arg;

    for (int i = 1; i < argc; ++i)
        argv[i] = (char *)va_arg(list, const char *);

    argv[argc] = NULL;
    return argv;
}

int execl(const char *path, const char *arg, ...)
{
    va_list list;
    va_start(list, arg);
    int argc = execve_get_argc(list, arg);
    va_end(list);

    va_start(list, arg);
    char **argv = execve_get_argv(list, argc, arg);
    va_end(list);

    int result = execve(path, argv, environ);

    free(argv);

    return result;
}

int execle(const char *path, const char *arg, ...)
{
    va_list list;
    va_start(list, arg);
    int argc = execve_get_argc(list, arg);
    va_end(list);

    va_start(list, arg);
    char **argv = execve_get_argv(list, argc, arg);
    char **envp = va_arg(list, char **);
    va_end(list);

    int result = execve(path, argv, envp);

    free(argv);
    free(envp);

    return result;
}

int execlp(const char *file, const char *arg, ...)
{
    va_list list;
    va_start(list, arg);
    int argc = execve_get_argc(list, arg);
    va_end(list);

    va_start(list, arg);
    char **argv = execve_get_argv(list, argc, arg);
    va_end(list);

    int result = execvpe(file, argv, environ);

    free(argv);

    return result;
}

int execv(const char *path, char *const argv[]) { return execve(path, argv, environ); }

_syscall3(execve, const char *, char *const *, char *const *);
int execve(const char *filename, char *const argv[], char *const envp[])
{
    SYSCALL_RETURN(syscall_execve(filename, argv, envp));
}

int execvp(const char *file, char *const argv[]) { return execvpe(file, argv, environ); }

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    if (strchr(file, '/'))
        return execve(file, argv, envp);

    // FIXME Read PATH and check if *file* exist
    return -1;
}

_syscall0(fork);
pid_t fork(void) { SYSCALL_RETURN(syscall_fork()); }

_syscall1(brk, void *);
int brk(void *addr) { SYSCALL_RETURN(syscall_brk(addr)); }

void *sbrk(intptr_t increment)
{
    void *address = (void *)brk(0x0);
    if (increment > 0)
        brk(address + increment);
    return address;
}

_syscall1(exit, int);
void _exit(int status)
{
    syscall_exit(status);
    __builtin_unreachable();
}
