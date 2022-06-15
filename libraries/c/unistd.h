#pragma once

#include <sys/types.h>
#include <stdint.h>

int execv(const char *, char *const[]);
int execve(const char *, char *const[], char *const[]);
int execvp(const char *, char *const[]);
pid_t fork(void);
void _exit(int status);
int brk(void *addr);
void *sbrk(intptr_t increment);
