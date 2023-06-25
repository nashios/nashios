#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_stat(const char *path, struct stat *buf) { return virtual_fs_stat(path, buf); }
