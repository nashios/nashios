#pragma once

#include <kernel/api/posix/mman.h>
#include <stddef.h>
#include <sys/cdefs.h>
#include <sys/types.h>

___BEGIN_DECLS

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
int shm_open(const char *name, int oflag, mode_t mode);

___END_DECLS
