#include <assert.h>
#include <wchar.h>

extern "C"
{
    int fwprintf(FILE *__restrict, const wchar_t *__restrict, ...) { assert(false); }

    int fwscanf(FILE *__restrict, const wchar_t *__restrict, ...) { assert(false); }

    int vfwprintf(FILE *__restrict, const wchar_t *__restrict, __builtin_va_list) { assert(false); }

    int vfwscanf(FILE *__restrict, const wchar_t *__restrict, __builtin_va_list) { assert(false); }

    int swprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...) { assert(false); }

    int swscanf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...) { assert(false); }

    int vswprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, __builtin_va_list) { assert(false); }

    int vswscanf(wchar_t *__restrict, size_t, const wchar_t *__restrict, __builtin_va_list) { assert(false); }

    int wprintf(const wchar_t *__restrict, ...) { assert(false); }

    int wscanf(const wchar_t *__restrict, ...) { assert(false); }

    int vwprintf(const wchar_t *__restrict, __builtin_va_list) { assert(false); }

    int vwscanf(const wchar_t *__restrict, __builtin_va_list) { assert(false); }

    wint_t fgetwc(FILE *) { assert(false); }

    wchar_t *fgetws(wchar_t *__restrict, int, FILE *__restrict) { assert(false); }

    wint_t fputwc(wchar_t, FILE *) { assert(false); }

    int fputws(const wchar_t *__restrict, FILE *__restrict) { assert(false); }

    int fwide(FILE *, int) { assert(false); }

    wint_t getwc(FILE *) { assert(false); }

    wint_t getwchar(void) { assert(false); }

    wint_t putwc(wchar_t, FILE *) { assert(false); }

    wint_t putwchar(wchar_t) { assert(false); }

    wint_t ungetwc(wint_t, FILE *) { assert(false); }

    double wcstod(const wchar_t *__restrict, wchar_t **__restrict) { assert(false); }

    float wcstof(const wchar_t *__restrict, wchar_t **__restrict) { assert(false); }

    long double wcstold(const wchar_t *__restrict, wchar_t **__restrict) { assert(false); }

    long wcstol(const wchar_t *__restrict, wchar_t **__restrict, int) { assert(false); }

    long long wcstoll(const wchar_t *__restrict, wchar_t **__restrict, int) { assert(false); }

    unsigned long wcstoul(const wchar_t *__restrict, wchar_t **__restrict, int) { assert(false); }

    unsigned long long wcstoull(const wchar_t *__restrict, wchar_t **__restrict, int) { assert(false); }

    wchar_t *wcscpy(wchar_t *__restrict, const wchar_t *__restrict) { assert(false); }

    wchar_t *wcsncpy(wchar_t *__restrict, const wchar_t *__restrict, size_t) { assert(false); }

    wchar_t *wmemcpy(wchar_t *__restrict, const wchar_t *__restrict, size_t) { assert(false); }

    wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t) { assert(false); }

    wchar_t *wcscat(wchar_t *__restrict, const wchar_t *__restrict) { assert(false); }

    wchar_t *wcsncat(wchar_t *__restrict, const wchar_t *__restrict, size_t) { assert(false); }

    int wcscmp(const wchar_t *, const wchar_t *) { assert(false); }

    int wcscoll(const wchar_t *, const wchar_t *) { assert(false); }

    int wcsncmp(const wchar_t *, const wchar_t *, size_t) { assert(false); }

    int wcsxfrm(wchar_t *__restrict, const wchar_t *__restrict, size_t) { assert(false); }

    int wmemcmp(const wchar_t *, const wchar_t *, size_t) { assert(false); }

    wchar_t *wcschr(const wchar_t *, wchar_t) { assert(false); }

    size_t wcscspn(const wchar_t *, const wchar_t *) { assert(false); }

    wchar_t *wcspbrk(const wchar_t *, const wchar_t *) { assert(false); }

    wchar_t *wcsrchr(const wchar_t *, wchar_t) { assert(false); }

    size_t wcsspn(const wchar_t *, const wchar_t *) { assert(false); }

    wchar_t *wcsstr(const wchar_t *, const wchar_t *) { assert(false); }

    wchar_t *wcstok(wchar_t *__restrict, const wchar_t *__restrict, wchar_t **__restrict) { assert(false); }

    wchar_t *wmemchr(const wchar_t *, wchar_t, size_t) { assert(false); }

    size_t wcslen(const wchar_t *) { assert(false); }

    wchar_t *wmemset(wchar_t *, wchar_t, size_t) { assert(false); }

    size_t wcsftime(wchar_t *__restrict, size_t, const wchar_t *__restrict, const struct tm *__restrict)
    {
        assert(false);
    }

    wint_t btowc(int c) { assert(false); }

    int wctob(wint_t) { assert(false); }

    int mbsinit(const mbstate_t *) { assert(false); }

    size_t mbrlen(const char *__restrict, size_t, mbstate_t *__restrict) { assert(false); }

    size_t mbrtowc(wchar_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict) { assert(false); }

    size_t wcrtomb(char *__restrict, wchar_t, mbstate_t *__restrict) { assert(false); }

    size_t mbsrtowcs(wchar_t *__restrict, const char **__restrict, size_t, mbstate_t *__restrict) { assert(false); }

    size_t mbsnrtowcs(wchar_t *__restrict, const char **__restrict, size_t, size_t, mbstate_t *__restrict)
    {
        assert(false);
    }

    size_t wcsrtombs(char *__restrict, const wchar_t **__restrict, size_t, mbstate_t *__restrict) { assert(false); }

    size_t wcsnrtombs(char *__restrict, const wchar_t **__restrict, size_t, size_t, mbstate_t *__restrict)
    {
        assert(false);
    }

    int wcwidth(wchar_t) { assert(false); }

    int wcswidth(const wchar_t *, size_t) { assert(false); }

    wchar_t *wcsdup(const wchar_t *) { assert(false); }

    int wcsncasecmp(const wchar_t *, const wchar_t *, size_t) { assert(false); }

    int wcscasecmp(const wchar_t *, const wchar_t *) { assert(false); }
}
