#pragma once

#include <bits/types.h>
#include <kernel/api/posix/time.h>
#include <locale.h>
#include <stddef.h>
#include <sys/cdefs.h>

___BEGIN_DECLS

struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
};

typedef __clock_t clock_t;

#ifdef __USE_TIME_BITS64
typedef __time64_t time_t;
#else
typedef __time_t time_t;
#endif

clock_t clock(void);
double difftime(time_t time1, time_t time0);
time_t mktime(struct tm *timeptr);
time_t time(time_t *tloc);
char *asctime(const struct tm *timeptr);
char *asctime_r(const struct tm *tm, char *buf);
char *ctime(const time_t *clock);
char *ctime_r(const time_t *clock, char *buf);
struct tm *gmtime(const time_t *timer);
struct tm *gmtime_r(const time_t *clock, struct tm *result);
struct tm *localtime(const time_t *timer);
struct tm *localtime_r(const time_t *timer, struct tm *result);
size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr);
size_t strftime_l(char *s, size_t maxsize, const char *format, const struct tm *timeptr, locale_t locale);

___END_DECLS
