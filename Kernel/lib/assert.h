#pragma once

#define assert(expression)                                                                                             \
    (__builtin_expect(!(expression), 0) ? ___assert(#expression, __FILE__, __PRETTY_FUNCTION__, __LINE__) : (void)0)

__attribute__((noreturn)) void ___assert(const char *message, const char *file, const char *function, int line);
