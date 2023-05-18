#pragma once

#include <kernel/api/posix/sys/types.h>
#include <stddef.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);