#include <assert.h>
#include <time.h>

extern "C"
{
    int daylight;
    long timezone;
    char *tzname[2];

    clock_t clock(void) { assert(false); }

    double difftime(time_t, time_t) { assert(false); }

    time_t mktime(struct tm *) { assert(false); }

    time_t time(time_t *) { assert(false); }

    int timespec_get(struct timespec *, int) { assert(false); }

    char *asctime(const struct tm *) { assert(false); }

    char *ctime(const time_t *) { assert(false); }

    struct tm *gmtime(const time_t *) { assert(false); }

    struct tm *gmtime_r(const time_t *__restrict, struct tm *__restrict) { assert(false); }

    struct tm *localtime(const time_t *) { assert(false); }

    size_t strftime(char *__restrict, size_t, const char *__restrict, const struct tm *__restrict) { assert(false); }

    void tzset(void) { assert(false); }

    int nanosleep(const struct timespec *, struct timespec *) { assert(false); }

    int clock_getres(clockid_t, struct timespec *) { assert(false); }

    int clock_gettime(clockid_t, struct timespec *) { assert(false); }

    int clock_nanosleep(clockid_t, int, const struct timespec *, struct timespec *) { assert(false); }

    int clock_settime(clockid_t, const struct timespec *) { assert(false); }

    struct tm *localtime_r(const time_t *, struct tm *) { assert(false); }

    char *asctime_r(const struct tm *, char *) { assert(false); }

    char *ctime_r(const time_t *, char *) { assert(false); }

    char *strptime(const char *__restrict, const char *__restrict, struct tm *__restrict) { assert(false); }

    time_t timelocal(struct tm *) { assert(false); }

    time_t timegm(struct tm *) { assert(false); }

    int utimes(const char *, const struct timeval[2]) { assert(false); }

    int futimes(int, const struct timeval[2]) { assert(false); }
}
