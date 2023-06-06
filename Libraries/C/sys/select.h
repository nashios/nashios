#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <bits/fd_set.h>
#include <bits/signal.h>
#include <bits/suseconds_t.h>
#include <bits/time_t.h>
#include <bits/timespec.h>
#include <bits/timeval.h>

#define FD_SETSIZE 1024

#ifdef __cplusplus
extern "C"
{
#endif

    typedef long int __fd_mask;
#define __NFDBITS (8 * (int)sizeof(__fd_mask))

    typedef __fd_mask fd_mask;
#define NFDBITS __NFDBITS

    void __FD_CLR(int fd, fd_set *);
    int __FD_ISSET(int fd, fd_set *);
    void __FD_SET(int fd, fd_set *);
    void __FD_ZERO(fd_set *);

#define FD_CLR(fd, set) __FD_CLR(fd, set)
#define FD_ISSET(fd, set) __FD_ISSET(fd, set)
#define FD_SET(fd, set) __FD_SET(fd, set)
#define FD_ZERO(set) __FD_ZERO(set)

    int select(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict, struct timeval *__restrict);
    int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *, const sigset_t *);

#ifdef __cplusplus
}
#endif

#endif
