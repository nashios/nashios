#pragma once

#include <sys/types.h>

int execv(const char *, char *const[]);
int execve(const char *, char *const[], char *const[]);
int execvp(const char *, char *const[]);
pid_t fork(void);
