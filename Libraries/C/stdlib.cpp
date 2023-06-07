#include <assert.h>
#include <signal.h>
#include <stdlib.h>

extern "C"
{
    double atof(const char *) { assert(false); }

    int atoi(const char *) { assert(false); }

    long atol(const char *) { assert(false); }

    long long atoll(const char *) { assert(false); }

    double strtod(const char *__restrict, char **__restrict) { assert(false); }

    float strtof(const char *__restrict, char **__restrict) { assert(false); }

    long double strtold(const char *__restrict, char **__restrict) { assert(false); }

    long strtol(const char *__restrict, char **__restrict, int) { assert(false); }

    long long strtoll(const char *__restrict, char **__restrict, int) { assert(false); }

    unsigned long strtoul(const char *__restrict, char **__restrict, int) { assert(false); }

    unsigned long long strtoull(const char *__restrict, char **__restrict, int) { assert(false); }

    int rand(void) { assert(false); }

    int rand_r(unsigned *) { assert(false); }

    void srand(unsigned int) { assert(false); }

    void *aligned_alloc(size_t, size_t) { assert(false); }

    void *calloc(size_t, size_t) { assert(false); }

    void free(void *) { assert(false); }

    void *malloc(size_t) { assert(false); }

    void *realloc(void *, size_t) { assert(false); }

    int posix_memalign(void **, size_t, size_t) { assert(false); }

    __attribute__((__noreturn__)) void abort(void)
    {
        raise(SIGABRT);

        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGABRT);
        sigprocmask(SIG_UNBLOCK, &set, nullptr);
        raise(SIGABRT);
    }

    int atexit(void (*func)(void)) { assert(false); }

    int at_quick_exit(void (*func)(void)) { assert(false); }

    __attribute__((__noreturn__)) void exit(int) { assert(false); }

    __attribute__((__noreturn__)) void _Exit(int) { assert(false); }

    char *getenv(const char *) { assert(false); }

    __attribute__((__noreturn__)) void quick_exit(int) { assert(false); }

    int system(const char *) { assert(false); }

    char *mktemp(char *) { assert(false); }

    void *bsearch(const void *, const void *, size_t, size_t, int (*compare)(const void *, const void *))
    {
        assert(false);
    }

    void qsort(void *, size_t, size_t, int (*compare)(const void *, const void *)) { assert(false); }

    void qsort_r(void *, size_t, size_t, int (*compar)(const void *, const void *, void *), void *) { assert(false); }

    int abs(int) { assert(false); }

    long labs(long) { assert(false); }

    long long llabs(long long) { assert(false); }

    div_t div(int, int) { assert(false); }

    ldiv_t ldiv(long, long) { assert(false); }

    lldiv_t lldiv(long long, long long) { assert(false); }

    int mblen(const char *, size_t) { assert(false); }

    int mbtowc(wchar_t *__restrict, const char *__restrict, size_t) { assert(false); }

    int wctomb(char *, wchar_t) { assert(false); }

    size_t mbstowcs(wchar_t *__restrict, const char *__restrict, size_t) { assert(false); }

    size_t wcstombs(char *, const wchar_t *__restrict, size_t) { assert(false); }

    long random(void) { assert(false); }

    double drand48(void) { assert(false); }

    void srand48(long int) { assert(false); }

    char *initstate(unsigned int, char *, size_t) { assert(false); }

    char *setstate(char *) { assert(false); }

    void srandom(unsigned int) { assert(false); }

    int putenv(char *) { assert(false); }

    int setenv(const char *, const char *, int) { assert(false); }

    int unsetenv(const char *) { assert(false); }

    int mkstemp(char *) { assert(false); }

    int mkostemp(char *, int) { assert(false); }

    int mkostemps(char *, int, int) { assert(false); }

    char *mkdtemp(char *) { assert(false); }

    char *realpath(const char *__restrict, char *__restrict) { assert(false); }

    int posix_openpt(int) { assert(false); }

    int grantpt(int) { assert(false); }

    int unlockpt(int) { assert(false); }

    char *ptsname(int) { assert(false); }

    int ptsname_r(int, char *, size_t) { assert(false); }

    double strtod_l(const char *__restrict__, char **__restrict__, locale_t) { assert(false); }

    long double strtold_l(const char *__restrict__, char **__restrict__, locale_t) { assert(false); }

    float strtof_l(const char *__restrict, char **__restrict, locale_t) { assert(false); }

    int getloadavg(double *, int) { assert(false); }

    char *secure_getenv(const char *) { assert(false); }

    char *canonicalize_file_name(const char *) { assert(false); }

    void *reallocarray(void *, size_t, size_t) { assert(false); }

    int clearenv(void) { assert(false); }
}
