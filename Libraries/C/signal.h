#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <bits/pthread_t.h>
#include <bits/signal.h>
#include <bits/time_t.h>
#include <bits/timespec.h>
#include <stddef.h>
#include <stdint.h>

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

#define _NSIG NSIG

#define FPE_INTDIV 1
#define FPE_INTOVF 2
#define FPE_FLTDIV 3
#define FPE_FLTOVF 4
#define FPE_FLTUND 5
#define FPE_FLTRES 6
#define FPE_FLTINV 7
#define FPE_FLTSUB 8

#define TRAP_BRKPT 1
#define TRAP_TRACE 2

#ifdef __cplusplus
extern "C"
{
#endif

    struct _libc_fpxreg
    {
        unsigned short int significand[4];
        unsigned short int exponent;
        unsigned short int __glibc_reserved1[3];
    };

    struct _libc_xmmreg
    {
        uint32_t element[4];
    };

    struct _libc_fpstate
    {
        uint16_t cwd;
        int16_t swd;
        uint16_t ftw;
        uint16_t fop;
        uint64_t rip;
        uint64_t dp;
        uint32_t mxcsr;
        uint32_t mxcr_mask;
        struct _libc_fpxreg _st[8];
        struct _libc_xmmreg _xmm[16];
        uint32_t __glibc_reserved1[24];
    };

    typedef struct _libc_fpstate *fpregset_t;
    typedef unsigned long int greg_t;
    typedef int sig_atomic_t;

    __sighandler signal(int sig, __sighandler handler);

    /**
     * @brief Send a signal to the executing process.
     *
     * @param sig The signal to send
     * @return 0 on success, a non-zero error code on failure
     */
    int raise(int sig);
    int sigemptyset(sigset_t *);
    int sigfillset(sigset_t *);
    int sigaddset(sigset_t *, int);
    int sigdelset(sigset_t *, int);
    int sigismember(const sigset_t *, int);
    int sigsuspend(const sigset_t *);
    int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict);
    int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict);

    /**
     * @brief Request that a signal be sent to a thread.
     *
     * @param thread The thread to send the signal to
     * @param sig The signal to send
     *
     * @return int 0 on success, a non-zero error code on failure
     */
    int pthread_kill(pthread_t thread, int sig);
    int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
    int sigpending(sigset_t *);
    int siginterrupt(int sig, int flag);
    int sigaltstack(const stack_t *__restrict ss, stack_t *__restrict oss);
    int kill(pid_t, int);
    int killpg(int, int);
    int sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info,
                     const struct timespec *__restrict timeout);
    int sigwait(const sigset_t *__restrict set, int *__restrict sig);
    int sigwaitinfo(const sigset_t *__restrict set, siginfo_t *__restrict info);
    int sigisemptyset(const sigset_t *set);
    int sigqueue(pid_t pid, int sig, const union sigval value);
    int tgkill(int, int, int);

#ifdef __cplusplus
}
#endif

#endif
