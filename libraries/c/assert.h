#pragma once

#include <sys/cdefs.h>

#ifdef NDEBUG
#define assert(expression) ((void)(0))
#else
#define assert(expression)                                                                                             \
    (__builtin_expect(!(expression), 0) ? ___assert(#expression, __FILE__, __PRETTY_FUNCTION__, __LINE__) : (void)0)

___BEGIN_DECLS

void ___assert(const char *message, const char *file, const char *function, int line);

___END_DECLS

#endif
