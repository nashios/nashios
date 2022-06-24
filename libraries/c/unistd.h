#pragma once

#include <stdint.h>
#include <sys/types.h>

extern char **environ;

int execl(const char *path, const char *arg, ...);
int execle(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execve(const char *filename, char *const argv[], char *const envp[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t fork(void);
void _exit(int status);
int brk(void *addr);
void *sbrk(intptr_t increment);
