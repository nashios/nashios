#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Used to implement assert() macro. Prints assertion message, file, line and function to stderr and aborts.
     *
     * @param assertion The assertion message
     * @param file The filename
     * @param line The file line
     * @param function The function name
     */
    void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function);
    void __assert_fail_perror(int errno, const char *file, unsigned int line, const char *function);

#ifdef __cplusplus
}
#endif

#endif

#ifdef NDEBUG

#undef assert

/**
 * @brief Does nothing, as NDEBUG is defined.
 *
 */
#define assert(ignore) ((void)0)

#undef assert_perror
#define assert_perror(ignore) ((void)0)

#else

#undef assert

/**
 * @brief Evaluates the assertion and if it is false, prints assertion message, file, line and function to stderr and
 * aborts.
 *
 */
#define assert(assertion) ((void)((assertion) || (__assert_fail(#assertion, __FILE__, __LINE__, __func__), 0)))

#undef assert_perror
#define assert_perror(errno) (!(errno) || (__assert_fail_perror((errno), __FILE__, __LINE__, __func__), 0))

#endif

#ifndef __cplusplus
#undef static_assert
#define static_assert _Static_assert
#endif
