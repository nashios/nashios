#include <sys/syscall.h>
#include <time.h>

extern "C"
{
    _syscall1(time, time_t *);
    time_t time(time_t *tloc) { SYSCALL_RETURN(syscall_time(tloc)); }
}
