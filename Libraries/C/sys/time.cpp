#include <assert.h>
#include <sys/time.h>

extern "C"
{
    int gettimeofday(struct timeval *__restrict, void *__restrict) { assert(false); }

    int settimeofday(const struct timeval *, const struct timezone *) { assert(false); }

    void timeradd(const struct timeval *, const struct timeval *, struct timeval *) { assert(false); }

    void timersub(const struct timeval *, const struct timeval *, struct timeval *) { assert(false); }

    void timerclear(struct timeval *) { assert(false); }

    int timerisset(struct timeval *) { assert(false); }

    int getitimer(int, struct itimerval *) { assert(false); }

    int setitimer(int, const struct itimerval *, struct itimerval *) { assert(false); }

    int timer_create(clockid_t, struct sigevent *__restrict, timer_t *__restrict) { assert(false); }

    int timer_settime(timer_t, int, const struct itimerspec *__restrict, struct itimerspec *__restrict)
    {
        assert(false);
    }

    int timer_gettime(timer_t, struct itimerspec *) { assert(false); }

    int timer_delete(timer_t) { assert(false); }
}
