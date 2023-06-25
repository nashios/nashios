#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

ssize_t syscall_write(int fildes, const void *buf, size_t nbyte) { return virtual_fs_write(fildes, buf, nbyte); }
