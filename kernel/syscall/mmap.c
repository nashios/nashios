#include <kernel/memory/mmap.h>
#include <kernel/system/syscall.h>

void *syscall_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    return mmap(addr, len, prot, flags, fildes, off);
}
