#pragma once

#include <kernel/api/posix/sys/stat.h>
#include <sys/cdefs.h>

___BEGIN_DECLS

int stat(const char *path, struct stat *buf);
int fstat(int fildes, struct stat *buf);
int chmod(const char *path, mode_t mode);
int mkdir(const char *path, mode_t mode);

___END_DECLS
