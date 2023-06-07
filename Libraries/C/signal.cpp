#include <assert.h>
#include <signal.h>

extern "C"
{
    __sighandler signal(int, __sighandler) { assert(false); }

    int raise(int) { assert(false); }

    int sigemptyset(sigset_t *) { assert(false); }

    int sigfillset(sigset_t *) { assert(false); }

    int sigaddset(sigset_t *, int) { assert(false); }

    int sigdelset(sigset_t *, int) { assert(false); }

    int sigismember(const sigset_t *, int) { assert(false); }

    int sigsuspend(const sigset_t *) { assert(false); }

    int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict) { assert(false); }

    int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict) { assert(false); }

    int pthread_kill(pthread_t, int) { assert(false); }

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
