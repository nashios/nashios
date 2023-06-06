#include <assert.h>
#include <sys/poll.h>

extern "C"
{
    int poll(struct pollfd *, nfds_t, int) { assert(false); }

    int ppoll(struct pollfd *, nfds_t, const struct timespec *, const sigset_t *) { assert(false); }
}
