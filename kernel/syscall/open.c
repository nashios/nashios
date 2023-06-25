#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_open(const char *path, int oflag, mode_t mode) { return virtual_fs_open(path, oflag, mode); }
