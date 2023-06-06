#ifndef _ERRNO_H
#define _ERRNO_H

#include <bits/errno.h>

#define errno __errno

#ifdef __cplusplus
extern "C"
{
#endif

    extern int __errno;
    extern char *program_invocation_name;
    extern char *program_invocation_short_name;
    extern char *__progname;
    extern char *__progname_full;

    int *__errno_location(void);

#ifdef __cplusplus
}
#endif

#endif
