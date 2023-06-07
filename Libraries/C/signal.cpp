#include <asm/tcb.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <syscall.h>
#include <unistd.h>

extern "C"
{
    __sighandler signal(int, __sighandler) { assert(false); }

    int raise(int sig) { pthread_kill(pthread_self(), sig); }

    int sigemptyset(sigset_t *) { assert(false); }

    int sigfillset(sigset_t *) { assert(false); }

    int sigaddset(sigset_t *, int) { assert(false); }

    int sigdelset(sigset_t *, int) { assert(false); }

    int sigismember(const sigset_t *, int) { assert(false); }

    int sigsuspend(const sigset_t *) { assert(false); }

    int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict) { assert(false); }

    int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict) { assert(false); }

    int pthread_kill(pthread_t thread, int sig)
    {
        auto tcb_thread = reinterpret_cast<tcb *>(thread);
        return syscall(SYS_tgkill, getpid(), tcb_thread->tid, sig);
    }

    int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict) { assert(false); }

    int sigpending(sigset_t *) { assert(false); }

    int siginterrupt(int, int) { assert(false); }

    int sigaltstack(const stack_t *__restrict, stack_t *__restrict) { assert(false); }

    int kill(pid_t, int) { assert(false); }

    int killpg(int, int) { assert(false); }

    int sigtimedwait(const sigset_t *__restrict, siginfo_t *__restrict, const struct timespec *__restrict)
    {
        assert(false);
    }

    int sigwait(const sigset_t *__restrict, int *__restrict) { assert(false); }

    int sigwaitinfo(const sigset_t *__restrict, siginfo_t *__restrict) { assert(false); }

    int sigisemptyset(const sigset_t *) { assert(false); }

    int sigqueue(pid_t, int, const union sigval) { assert(false); }

    int tgkill(int, int, int) { assert(false); }
}
