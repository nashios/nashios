#ifndef _BITS_LIMITS_H
#define _BITS_LIMITS_H

#include <kernel/api/posix/limits.h>

#define SHRT_MIN (-SHRT_MAX - 1)
#define SHRT_MAX __SHRT_MAX__

#if __SHRT_MAX__ == __INT_MAX__
#define USHRT_MAX (SHRT_MAX * 2U + 1U)
#else
#define USHRT_MAX (SHRT_MAX * 2 + 1)
#endif

#define ULONG_MAX ((unsigned long)(~0L))

#endif
