#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

___BEGIN_DECLS

extern char **environ;

int execl(const char *path, const char *arg0, ...);
int execle(const char *path, const char *arg0, ...);
int execlp(const char *file, const char *arg0, ...);
int execv(const char *path, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
int execvp(const char *file, char *const argv[]);
int fexecve(int fd, char *const argv[], char *const envp[]);
int ftruncate(int fildes, off_t length);
int truncate(const char *path, off_t length);
pid_t fork(void);
void _exit(int status);
int brk(void *addr);
void *sbrk(intptr_t incr);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);
ssize_t write(int fildes, const void *buf, size_t nbyte);
ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
int close(int fildes);
off_t lseek(int fildes, off_t offset, int whence);
char *getcwd(char *buf, size_t size);
int chdir(const char *path);
pid_t getpgid(pid_t pid);
pid_t getpid(void);

___END_DECLS
