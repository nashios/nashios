#include <assert.h>
#include <sys/uio.h>

extern "C"
{
    ssize_t readv(int, const struct iovec *, int) { assert(false); }

    ssize_t writev(int, const struct iovec *, int) { assert(false); }

    ssize_t preadv(int, const struct iovec *, int, off_t) { assert(false); }

    ssize_t pwritev(int, const struct iovec *, int, off_t) { assert(false); }
}
