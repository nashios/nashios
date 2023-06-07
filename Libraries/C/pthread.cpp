#include <assert.h>
#include <pthread.h>

extern "C"
{
    int pthread_attr_init(pthread_attr_t *) { assert(false); }

    int pthread_attr_destroy(pthread_attr_t *) { assert(false); }

    int pthread_attr_getdetachstate(const pthread_attr_t *, int *) { assert(false); }

    int pthread_attr_setdetachstate(pthread_attr_t *, int) { assert(false); }

    int pthread_attr_getstacksize(const pthread_attr_t *__restrict, size_t *__restrict) { assert(false); }

    int pthread_attr_setstacksize(pthread_attr_t *, size_t) { assert(false); }

    int pthread_attr_getstackaddr(const pthread_attr_t *, void **) { assert(false); }

    int pthread_attr_setstackaddr(pthread_attr_t *, void *) { assert(false); }

    int pthread_attr_getguardsize(const pthread_attr_t *__restrict, size_t *__restrict) { assert(false); }

    int pthread_attr_setguardsize(pthread_attr_t *, size_t) { assert(false); }

    int pthread_attr_getscope(const pthread_attr_t *, int *) { assert(false); }

    int pthread_attr_setscope(pthread_attr_t *, int) { assert(false); }

    int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_attr_setschedpolicy(pthread_attr_t *__restrict, int) { assert(false); }

    int pthread_attr_getinheritsched(const pthread_attr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_attr_setinheritsched(pthread_attr_t *__restrict, int) { assert(false); }

    int pthread_attr_getschedparam(const pthread_attr_t *__restrict, struct sched_param *__restrict) { assert(false); }

    int pthread_attr_setschedparam(pthread_attr_t *__restrict, const struct sched_param *__restrict) { assert(false); }

    int pthread_attr_getaffinity_np(const pthread_attr_t *__restrict, size_t, cpu_set_t *__restrict) { assert(false); }

    int pthread_attr_setaffinity_np(pthread_attr_t *__restrict, size_t, const cpu_set_t *__restrict) { assert(false); }

    int pthread_attr_getsigmask_np(const pthread_attr_t *__restrict, sigset_t *__restrict) { assert(false); }

    int pthread_attr_setsigmask_np(pthread_attr_t *__restrict, const sigset_t *__restrict) { assert(false); }

    int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset) { assert(false); }

    int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset) { assert(false); }

    int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict, void *(*)(void *), void *__restrict)
    {
        assert(false);
    }

    pthread_t pthread_self(void) { assert(false); }

    int pthread_equal(pthread_t, pthread_t) { assert(false); }

    int pthread_exit(void *) { assert(false); }

    int pthread_join(pthread_t, void **) { assert(false); }

    int pthread_detach(pthread_t) { assert(false); }

    void pthread_cleanup_push(void (*)(void *), void *) { assert(false); }

    void pthread_cleanup_pop(int) { assert(false); }

    int pthread_setname_np(pthread_t, const char *) { assert(false); }

    int pthread_getname_np(pthread_t, char *, size_t) { assert(false); }

    int pthread_attr_setstack(pthread_attr_t *, void *, size_t) { assert(false); }

    int pthread_attr_getstack(const pthread_attr_t *, void **, size_t *) { assert(false); }

    int pthread_getattr_np(pthread_t, pthread_attr_t *) { assert(false); }

    int pthread_setschedparam(pthread_t, int, const struct sched_param *) { assert(false); }

    int pthread_getschedparam(pthread_t, int *, struct sched_param *) { assert(false); }

    int pthread_setcanceltype(int, int *) { assert(false); }

    int pthread_setcancelstate(int, int *) { assert(false); }

    void pthread_testcancel(void) { assert(false); }

    int pthread_cancel(pthread_t) { assert(false); }

    int pthread_atfork(void (*)(void), void (*)(void), void (*)(void)) { assert(false); }

    int pthread_key_create(pthread_key_t *, void (*)(void *)) { assert(false); }

    int pthread_key_delete(pthread_key_t) { assert(false); }

    void *pthread_getspecific(pthread_key_t) { assert(false); }

    int pthread_setspecific(pthread_key_t, const void *) { assert(false); }

    int pthread_once(pthread_once_t *, void (*)(void)) { assert(false); }

    int pthread_mutexattr_init(pthread_mutexattr_t *) { assert(false); }

    int pthread_mutexattr_destroy(pthread_mutexattr_t *) { assert(false); }

    int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_mutexattr_settype(pthread_mutexattr_t *, int) { assert(false); }

    int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_mutexattr_setrobust(pthread_mutexattr_t *, int) { assert(false); }

    int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *) { assert(false); }

    int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int) { assert(false); }

    int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int) { assert(false); }

    int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *, int *) { assert(false); }

    int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int) { assert(false); }

    int pthread_mutex_init(pthread_mutex_t *__restrict, const pthread_mutexattr_t *__restrict) { assert(false); }

    int pthread_mutex_destroy(pthread_mutex_t *) { assert(false); }

    int pthread_mutex_lock(pthread_mutex_t *) { assert(false); }

    int pthread_mutex_trylock(pthread_mutex_t *) { assert(false); }

    int pthread_mutex_timedlock(pthread_mutex_t *__restrict, const struct timespec *__restrict) { assert(false); }

    int pthread_mutex_unlock(pthread_mutex_t *) { assert(false); }

    int pthread_mutex_consistent(pthread_mutex_t *) { assert(false); }

    int pthread_condattr_init(pthread_condattr_t *) { assert(false); }

    int pthread_condattr_destroy(pthread_condattr_t *) { assert(false); }

    int pthread_condattr_getclock(const pthread_condattr_t *__restrict, clockid_t *__restrict) { assert(false); }

    int pthread_condattr_setclock(pthread_condattr_t *, clockid_t) { assert(false); }

    int pthread_condattr_getpshared(const pthread_condattr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_condattr_setpshared(pthread_condattr_t *, int) { assert(false); }

    int pthread_cond_init(pthread_cond_t *__restrict, const pthread_condattr_t *__restrict) { assert(false); }

    int pthread_cond_destroy(pthread_cond_t *) { assert(false); }

    int pthread_cond_wait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict) { assert(false); }

    int pthread_cond_timedwait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict,
                               const struct timespec *__restrict)
    {
        assert(false);
    }

    int pthread_cond_signal(pthread_cond_t *) { assert(false); }

    int pthread_cond_broadcast(pthread_cond_t *) { assert(false); }

    int pthread_barrierattr_init(pthread_barrierattr_t *) { assert(false); }

    int pthread_barrierattr_destroy(pthread_barrierattr_t *) { assert(false); }

    int pthread_barrierattr_setpshared(pthread_barrierattr_t *, int) { assert(false); }

    int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_barrier_init(pthread_barrier_t *__restrict, const pthread_barrierattr_t *__restrict, unsigned int)
    {
        assert(false);
    }

    int pthread_barrier_destroy(pthread_barrier_t *) { assert(false); }

    int pthread_barrier_wait(pthread_barrier_t *) { assert(false); }

    int pthread_rwlockattr_init(pthread_rwlockattr_t *) { assert(false); }

    int pthread_rwlockattr_destroy(pthread_rwlockattr_t *) { assert(false); }

    int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int) { assert(false); }

    int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *__restrict, int *__restrict) { assert(false); }

    int pthread_rwlock_init(pthread_rwlock_t *__restrict, const pthread_rwlockattr_t *__restrict) { assert(false); }

    int pthread_rwlock_destroy(pthread_rwlock_t *) { assert(false); }

    int pthread_rwlock_trywrlock(pthread_rwlock_t *) { assert(false); }

    int pthread_rwlock_wrlock(pthread_rwlock_t *) { assert(false); }

    int pthread_rwlock_tryrdlock(pthread_rwlock_t *) { assert(false); }

    int pthread_rwlock_rdlock(pthread_rwlock_t *) { assert(false); }

    int pthread_rwlock_unlock(pthread_rwlock_t *) { assert(false); }

    int pthread_getcpuclockid(pthread_t, clockid_t *) { assert(false); }
}
