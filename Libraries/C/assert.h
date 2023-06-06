#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C"
{
#endif

    void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function);
    void __assert_fail_perror(int errno, const char *file, unsigned int line, const char *function);

#ifdef __cplusplus
}
#endif

#endif

#ifdef NDEBUG

#undef assert
#define assert(ignore) ((void)0)

#undef assert_perror
#define assert_perror(ignore) ((void)0)

#else

#undef assert
#define assert(assertion) ((void)((assertion) || (__assert_fail(#assertion, __FILE__, __LINE__, __func__), 0)))

#undef assert_perror
#define assert_perror(errno) (!(errno) || (__assert_fail_perror((errno), __FILE__, __LINE__, __func__), 0))

#endif

#ifndef __cplusplus
#undef static_assert
#define static_assert _Static_assert
#endif
