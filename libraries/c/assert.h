#ifndef _ASSERT_H
#define _ASSERT_H

#ifndef NDEBUG

/**
 * @brief Abort the program after false assertion
 *
 * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/assert.html
 *
 * @param assertion The expression to assert
 */
#define assert(assertion) ((assertion) ? (void)0 : __assert_fail(#assertion, __FILE__, __LINE__, __PRETTY_FUNCTION__))
#else

/**
 * @brief Abort the program after false assertion
 *
 * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/assert.html
 *
 * @param assertion The expression to assert
 */
#define assert(assertion) ((void)0)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Abort the program after false assertion
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/assert.html
     *
     * @param assertion The assertion that failed
     * @param file The file where the assertion failed
     * @param line The line where the assertion failed
     * @param function The function where the assertion failed
     */
    void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function);

#ifdef __cplusplus
}
#endif

#endif
