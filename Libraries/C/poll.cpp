#include <poll.h>
#include <sys/syscall.h>

extern "C"
{
    _syscall3(poll, struct pollfd *, nfds_t, int);
    int poll(struct pollfd fds[], nfds_t nfds, int timeout) { SYSCALL_RETURN(syscall_poll(fds, nfds, timeout)); }
}
