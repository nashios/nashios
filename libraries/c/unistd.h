#pragma once

#include <stdint.h>
#include <sys/types.h>

extern char **environ;

int execl(const char *path, const char *arg0, ...);
int execle(const char *path, const char *arg0, ...);
int execlp(const char *file, const char *arg0, ...);
int execv(const char *path, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
int execvp(const char *file, char *const argv[]);
int fexecve(int fd, char *const argv[], char *const envp[]);
pid_t fork(void);
void _exit(int status);
int brk(void *addr);
void *sbrk(intptr_t incr);
ssize_t read(int fildes, void *buf, size_t nbyte);
int close(int fildes);
