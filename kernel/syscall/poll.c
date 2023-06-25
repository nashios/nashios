#include <kernel/filesystem/virtual.h>
#include <kernel/system/syscall.h>

int syscall_poll(struct pollfd fds[], nfds_t nfds, int timeout) { return virtual_fs_poll(fds, nfds, timeout); }
