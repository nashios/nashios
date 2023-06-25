#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_close(int fildes) { return virtual_fs_close(fildes); }
