#include <assert.h>
#include <sched.h>

extern "C"
{
    int sched_yield(void) { assert(false); }

    int sched_get_priority_max(int) { assert(false); }

    int sched_get_priority_min(int) { assert(false); }

    int sched_setscheduler(pid_t, int, const struct sched_param *) { assert(false); }

    int sched_getparam(pid_t, struct sched_param *) { assert(false); }

    int sched_getscheduler(pid_t) { assert(false); }

    int sched_setaffinity(pid_t, size_t, const cpu_set_t *) { assert(false); }

    int sched_getaffinity(pid_t, size_t, cpu_set_t *) { assert(false); }

    int unshare(int) { assert(false); }

    int clone(int (*)(void *), void *, int, void *, ...) { assert(false); }

    int sched_getcpu(void) { assert(false); }

    int setns(int, int) { assert(false); }
}
