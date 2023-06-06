#include <assert.h>
#include <sys/select.h>

extern "C"
{
    void __FD_CLR(int, fd_set *) { assert(false); }

    int __FD_ISSET(int, fd_set *) { assert(false); }

    void __FD_SET(int, fd_set *) { assert(false); }

    void __FD_ZERO(fd_set *) { assert(false); }

    int select(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict, struct timeval *__restrict)
    {
        assert(false);
    }

    int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *, const sigset_t *) { assert(false); }
}
