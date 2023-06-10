#include <poll.h>
#include <sys/syscall.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout) { return syscall(__NR_poll, fds, nfds, timeout); }
