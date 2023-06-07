#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <bits/clockid_t.h>
#include <bits/cpu_set.h>
#include <bits/pthread_t.h>
#include <bits/size_t.h>
#include <bits/threads.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PTHREAD_CREATE_JOINABLE __LIBC_THREAD_CREATE_JOINABLE
#define PTHREAD_CREATE_DETACHED __LIBC_THREAD_CREATE_DETACHED

#define PTHREAD_SCOPE_SYSTEM 0
#define PTHREAD_SCOPE_PROCESS 1

#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

#define PTHREAD_MUTEX_DEFAULT __LIBC_THREAD_MUTEX_DEFAULT
#define PTHREAD_MUTEX_NORMAL __LIBC_THREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_ERRORCHECK __LIBC_THREAD_MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_RECURSIVE __LIBC_THREAD_MUTEX_RECURSIVE

#define PTHREAD_MUTEX_STALLED __LIBC_THREAD_MUTEX_STALLED
#define PTHREAD_MUTEX_ROBUST __LIBC_THREAD_MUTEX_ROBUST

#define PTHREAD_PROCESS_PRIVATE __LIBC_THREAD_PROCESS_PRIVATE
#define PTHREAD_PROCESS_SHARED __MLIBC_THREAD_PROCESS_SHARED

#define PTHREAD_PRIO_NONE __LIBC_THREAD_PRIO_NONE
#define PTHREAD_PRIO_INHERIT __LIBC_THREAD_PRIO_INHERIT
#define PTHREAD_PRIO_PROTECT __LIBC_THREAD_PRIO_PROTECT

#define PTHREAD_ONCE_INIT                                                                                              \
    {                                                                                                                  \
        0                                                                                                              \
    }
#define PTHREAD_COND_INITIALIZER                                                                                       \
    {                                                                                                                  \
        0                                                                                                              \
    }
#define PTHREAD_MUTEX_INITIALIZER                                                                                      \
    {                                                                                                                  \
        0, 0, 0, 0                                                                                                     \
    }
#define PTHREAD_RWLOCK_INITIALIZER                                                                                     \
    {                                                                                                                  \
        0, 0, 0                                                                                                        \
    }

#define PTHREAD_CANCELED ((void *)-1)

#define PTHREAD_BARRIER_SERIAL_THREAD -1

#define PTHREAD_DESTRUCTOR_ITERATIONS 8

#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_STACK_MIN 16384

#define PTHREAD_ATTR_NO_SIGMASK_NP (-1)

    typedef struct __libc_threadattr pthread_attr_t;

    typedef uintptr_t pthread_key_t;

    struct __mlibc_once
    {
        unsigned int __mlibc_done;
    };
    typedef struct __mlibc_once pthread_once_t;

    typedef struct __libc_mutexattr pthread_mutexattr_t;

    typedef struct __libc_mutex pthread_mutex_t;

    typedef struct __libc_condattr pthread_condattr_t;

    typedef struct __libc_cond pthread_cond_t;

    struct __mlibc_barrierattr_struct
    {
        int __mlibc_pshared;
    };
    typedef struct __mlibc_barrierattr_struct pthread_barrierattr_t;

    struct __mlibc_barrier
    {
        unsigned int __mlibc_waiting;
        unsigned int __mlibc_inside;
        unsigned int __mlibc_count;
        unsigned int __mlibc_seq;
        unsigned int __mlibc_flags;
    };
    typedef struct __mlibc_barrier pthread_barrier_t;

    struct __mlibc_fair_rwlock
    {
        unsigned int __mlibc_m;  // Mutex.
        unsigned int __mlibc_rc; // Reader count (not reference count).
        unsigned int __mlibc_flags;
    };
    typedef struct __mlibc_fair_rwlock pthread_rwlock_t;

    struct __mlibc_rwlockattr
    {
        int __mlibc_pshared;
    };
    typedef struct __mlibc_rwlockattr pthread_rwlockattr_t;

    int pthread_attr_init(pthread_attr_t *);
    int pthread_attr_destroy(pthread_attr_t *);
    int pthread_attr_getdetachstate(const pthread_attr_t *, int *);
    int pthread_attr_setdetachstate(pthread_attr_t *, int);
    int pthread_attr_getstacksize(const pthread_attr_t *__restrict, size_t *__restrict);
    int pthread_attr_setstacksize(pthread_attr_t *, size_t);
    int pthread_attr_getstackaddr(const pthread_attr_t *, void **);
    int pthread_attr_setstackaddr(pthread_attr_t *, void *);
    int pthread_attr_getguardsize(const pthread_attr_t *__restrict, size_t *__restrict);
    int pthread_attr_setguardsize(pthread_attr_t *, size_t);
    int pthread_attr_getscope(const pthread_attr_t *, int *);
    int pthread_attr_setscope(pthread_attr_t *, int);
    int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict, int *__restrict);
    int pthread_attr_setschedpolicy(pthread_attr_t *__restrict, int);
    int pthread_attr_getinheritsched(const pthread_attr_t *__restrict, int *__restrict);
    int pthread_attr_setinheritsched(pthread_attr_t *__restrict, int);
    int pthread_attr_getschedparam(const pthread_attr_t *__restrict, struct sched_param *__restrict);
    int pthread_attr_setschedparam(pthread_attr_t *__restrict, const struct sched_param *__restrict);
    int pthread_attr_getaffinity_np(const pthread_attr_t *__restrict, size_t, cpu_set_t *__restrict);
    int pthread_attr_setaffinity_np(pthread_attr_t *__restrict, size_t, const cpu_set_t *__restrict);
    int pthread_attr_getsigmask_np(const pthread_attr_t *__restrict, sigset_t *__restrict);
    int pthread_attr_setsigmask_np(pthread_attr_t *__restrict, const sigset_t *__restrict);
    int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset);
    int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);
    int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict, void *(*)(void *), void *__restrict);

    /**
     * @brief Get the calling thread ID
     *
     * @return pthread_t The calling thread ID
     */
    pthread_t pthread_self(void);
    int pthread_equal(pthread_t, pthread_t);
    int pthread_exit(void *);
    int pthread_join(pthread_t, void **);
    int pthread_detach(pthread_t);
    void pthread_cleanup_push(void (*)(void *), void *);
    void pthread_cleanup_pop(int);
    int pthread_setname_np(pthread_t, const char *);
    int pthread_getname_np(pthread_t, char *, size_t);
    int pthread_attr_setstack(pthread_attr_t *, void *, size_t);
    int pthread_attr_getstack(const pthread_attr_t *, void **, size_t *);
    int pthread_getattr_np(pthread_t, pthread_attr_t *);
    int pthread_setschedparam(pthread_t, int, const struct sched_param *);
    int pthread_getschedparam(pthread_t, int *, struct sched_param *);
    int pthread_setcanceltype(int, int *);
    int pthread_setcancelstate(int, int *);
    void pthread_testcancel(void);
    int pthread_cancel(pthread_t);
    int pthread_atfork(void (*)(void), void (*)(void), void (*)(void));
    int pthread_key_create(pthread_key_t *, void (*)(void *));
    int pthread_key_delete(pthread_key_t);
    void *pthread_getspecific(pthread_key_t);
    int pthread_setspecific(pthread_key_t, const void *);
    int pthread_once(pthread_once_t *, void (*)(void));
    int pthread_mutexattr_init(pthread_mutexattr_t *);
    int pthread_mutexattr_destroy(pthread_mutexattr_t *);
    int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict, int *__restrict);
    int pthread_mutexattr_settype(pthread_mutexattr_t *, int);
    int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict, int *__restrict);
    int pthread_mutexattr_setrobust(pthread_mutexattr_t *, int);
    int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *);
    int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
    int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict, int *__restrict);
    int pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
    int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *, int *);
    int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
    int pthread_mutex_init(pthread_mutex_t *__restrict, const pthread_mutexattr_t *__restrict);
    int pthread_mutex_destroy(pthread_mutex_t *);
    int pthread_mutex_lock(pthread_mutex_t *);
    int pthread_mutex_trylock(pthread_mutex_t *);
    int pthread_mutex_timedlock(pthread_mutex_t *__restrict, const struct timespec *__restrict);
    int pthread_mutex_unlock(pthread_mutex_t *);
    int pthread_mutex_consistent(pthread_mutex_t *);
    int pthread_condattr_init(pthread_condattr_t *);
    int pthread_condattr_destroy(pthread_condattr_t *);
    int pthread_condattr_getclock(const pthread_condattr_t *__restrict, clockid_t *__restrict);
    int pthread_condattr_setclock(pthread_condattr_t *, clockid_t);
    int pthread_condattr_getpshared(const pthread_condattr_t *__restrict, int *__restrict);
    int pthread_condattr_setpshared(pthread_condattr_t *, int);
    int pthread_cond_init(pthread_cond_t *__restrict, const pthread_condattr_t *__restrict);
    int pthread_cond_destroy(pthread_cond_t *);
    int pthread_cond_wait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict);
    int pthread_cond_timedwait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict,
                               const struct timespec *__restrict);
    int pthread_cond_signal(pthread_cond_t *);
    int pthread_cond_broadcast(pthread_cond_t *);
    int pthread_barrierattr_init(pthread_barrierattr_t *);
    int pthread_barrierattr_destroy(pthread_barrierattr_t *);
    int pthread_barrierattr_setpshared(pthread_barrierattr_t *, int);
    int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict, int *__restrict);
    int pthread_barrier_init(pthread_barrier_t *__restrict, const pthread_barrierattr_t *__restrict, unsigned int);
    int pthread_barrier_destroy(pthread_barrier_t *);
    int pthread_barrier_wait(pthread_barrier_t *);
    int pthread_rwlockattr_init(pthread_rwlockattr_t *);
    int pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
    int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);
    int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *__restrict, int *__restrict);
    int pthread_rwlock_init(pthread_rwlock_t *__restrict, const pthread_rwlockattr_t *__restrict);
    int pthread_rwlock_destroy(pthread_rwlock_t *);
    int pthread_rwlock_trywrlock(pthread_rwlock_t *);
    int pthread_rwlock_wrlock(pthread_rwlock_t *);
    int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
    int pthread_rwlock_rdlock(pthread_rwlock_t *);
    int pthread_rwlock_unlock(pthread_rwlock_t *);
    int pthread_getcpuclockid(pthread_t, clockid_t *);

#ifdef __cplusplus
}
#endif

#endif
