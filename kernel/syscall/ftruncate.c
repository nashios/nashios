#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_ftruncate(int fildes, off_t length) { return virtual_fs_ftruncate(fildes, length); }
