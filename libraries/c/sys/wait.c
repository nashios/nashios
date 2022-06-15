#include <sys/wait.h>
#include <syscall.h>

pid_t wait(int *stat_loc)
{
    return waitpid(-1, stat_loc, 0);
}

_syscall3(waitpid, pid_t, int *, int);
pid_t waitpid(pid_t pid, int *stat_loc, int options)
{
    SYSCALL_RETURN(syscall_waitpid(pid, stat_loc, options));
}

_syscall4(waitid, idtype_t, id_t, siginfo_t *, int);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
    SYSCALL_RETURN(syscall_waitid(idtype, id, infop, options));
}
