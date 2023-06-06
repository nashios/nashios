#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <bits/iovec.h>
#include <bits/off_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <limits.h>

#define UIO_MAXIOV IOV_MAX

#ifdef __cplusplus
extern "C"
{
#endif

    ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
    ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
    ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);
    ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);

#ifdef __cplusplus
}
#endif

#endif
