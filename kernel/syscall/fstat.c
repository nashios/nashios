#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_fstat(int fildes, struct stat *buf) { return virtual_fs_fstat(fildes, buf); }
