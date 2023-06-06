#ifndef _SYS_POLL_H
#define _SYS_POLL_H

#include <bits/poll.h>
#include <bits/signal.h>
#include <bits/timespec.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef __libc_size nfds_t;

    struct pollfd
    {
        int fd;
        short events;
        short revents;
    };

    int poll(struct pollfd *, nfds_t, int);
    int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask);

#ifdef __cplusplus
}
#endif

#endif
