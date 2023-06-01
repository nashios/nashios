#pragma once

#include <kernel/api/posix/sys/types.h>
#include <stddef.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
int mmap_brk(uint32_t addr);
uint32_t mmap_sbrk(intptr_t increment);
struct process_vm *mmap_find_unmapped_area(uint32_t addr, uint32_t len);
