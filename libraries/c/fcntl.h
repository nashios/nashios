#pragma once

#include <kernel/api/posix/fcntl.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <unistd.h>

___BEGIN_DECLS

int open(const char *path, int oflag, ...);
int openat(int dirfd, const char *pathname, int flags, ...);
int mkdirat(int fd, const char *path, mode_t mode);
int creat(const char *pathname, mode_t mode);

___END_DECLS
