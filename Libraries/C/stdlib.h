#ifndef _STDLIB_H
#define _STDLIB_H

#include <alloca.h>
#include <bits/locale_t.h>
#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/wchar_t.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 0x7FFFFFFF

#define MB_CUR_MAX 4

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int quot, rem;
    } div_t;

    typedef struct
    {
        long quot, rem;
    } ldiv_t;

    typedef struct
    {
        long long quot, rem;
    } lldiv_t;

    typedef int (*comparison_fn_t)(const void *, const void *);

    double atof(const char *string);
    int atoi(const char *string);
    long atol(const char *string);
    long long atoll(const char *string);
    double strtod(const char *__restrict string, char **__restrict end);
    float strtof(const char *__restrict string, char **__restrict end);
    long double strtold(const char *__restrict string, char **__restrict end);
    long strtol(const char *__restrict string, char **__restrict end, int base);
    long long strtoll(const char *__restrict string, char **__restrict end, int base);
    unsigned long strtoul(const char *__restrict string, char **__restrict end, int base);
    unsigned long long strtoull(const char *__restrict string, char **__restrict end, int base);
    int rand(void);
    int rand_r(unsigned *);
    void srand(unsigned int);
    void *aligned_alloc(size_t alignment, size_t size);
    void *calloc(size_t count, size_t size);
    void free(void *pointer);
    void *malloc(size_t size);
    void *realloc(void *pointer, size_t size);
    int posix_memalign(void **, size_t, size_t);

    /**
     * @brief Cause abnormal program termination, unless SIGABRT is being caught and the signal handler does not return.
     *
     * @note Include the default actions of SIGABRT, such as flushing streams.
     *
     */
    __attribute__((__noreturn__)) void abort(void);
    int atexit(void (*func)(void));
    int at_quick_exit(void (*func)(void));

    /**
     * @brief Terminates the calling process immediately.
     *
     * @param status Exit status of the process.
     */
    __attribute__((__noreturn__)) void exit(int status);

    /**
     * @brief Terminates the calling process immediately.
     *
     * @note This function does not flush stdio buffers.
     *
     * @param status Exit status of the process.
     */
    __attribute__((__noreturn__)) void _Exit(int status);
    char *getenv(const char *name);
    __attribute__((__noreturn__)) void quick_exit(int status);
    int system(const char *string);
    char *mktemp(char *);
    void *bsearch(const void *key, const void *base, size_t count, size_t size,
                  int (*compare)(const void *, const void *));
    void qsort(void *base, size_t count, size_t size, int (*compare)(const void *, const void *));
    void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);
    int abs(int number);
    long labs(long number);
    long long llabs(long long number);
    div_t div(int number, int denom);
    ldiv_t ldiv(long number, long denom);
    lldiv_t lldiv(long long number, long long denom);
    int mblen(const char *, size_t);
    int mbtowc(wchar_t *__restrict wc, const char *__restrict mb_chr, size_t max_size);
    int wctomb(char *mb_chr, wchar_t wc);
    size_t mbstowcs(wchar_t *__restrict wc_string, const char *__restrict mb_string, size_t max_size);
    size_t wcstombs(char *mb_string, const wchar_t *__restrict wc_string, size_t max_size);
    long random(void);
    double drand48(void);
    void srand48(long int);
    char *initstate(unsigned int, char *, size_t);
    char *setstate(char *);
    void srandom(unsigned int);
    int putenv(char *);
    int setenv(const char *, const char *, int);
    int unsetenv(const char *);
    int mkstemp(char *);
    int mkostemp(char *, int flags);
    int mkostemps(char *pattern, int suffixlen, int flags);
    char *mkdtemp(char *path);
    char *realpath(const char *__restrict, char *__restrict);
    int posix_openpt(int flags);
    int grantpt(int fd);
    int unlockpt(int fd);
    char *ptsname(int fd);
    int ptsname_r(int fd, char *buf, size_t len);
    double strtod_l(const char *__restrict__ nptr, char **__restrict__ endptr, locale_t loc);
    long double strtold_l(const char *__restrict__ nptr, char **__restrict__ endptr, locale_t loc);
    float strtof_l(const char *__restrict string, char **__restrict end, locale_t loc);
    int getloadavg(double *, int);
    char *secure_getenv(const char *);
    char *canonicalize_file_name(const char *);
    void *reallocarray(void *, size_t, size_t);
    int clearenv(void);

#ifdef __cplusplus
}
#endif

#endif
