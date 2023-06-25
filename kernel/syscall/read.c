#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

ssize_t syscall_read(int fildes, void *buf, size_t nbyte) { return virtual_fs_read(fildes, buf, nbyte); }
