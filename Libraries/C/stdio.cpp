#include <assert.h>
#include <stdio.h>

extern "C"
{
    FILE *stderr;
    FILE *stdin;
    FILE *stdout;

    int remove(const char *) { assert(false); }

    int rename(const char *, const char *) { assert(false); }

    int renameat(int, const char *, int, const char *) { assert(false); }

    FILE *tmpfile(void) { assert(false); }

    char *tmpnam(char *) { assert(false); }

    int fclose(FILE *) { assert(false); }

    int fflush(FILE *) { assert(false); }

    FILE *fopen(const char *__restrict, const char *__restrict) { assert(false); }

    FILE *freopen(const char *__restrict, const char *__restrict, FILE *__restrict) { assert(false); }

    void setbuf(FILE *__restrict, char *__restrict) { assert(false); }

    int setvbuf(FILE *__restrict, char *__restrict, int, size_t) { assert(false); }

    void setlinebuf(FILE *) { assert(false); }

    void set(FILE *, char *, size_t) { assert(false); }

    __attribute__((__format__(gnu_printf, 2, 3))) int fprintf(FILE *__restrict, const char *__restrict, ...)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_scanf, 2, 3))) int fscanf(FILE *__restrict, const char *__restrict, ...)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 1, 2))) int printf(const char *__restrict, ...) { assert(false); }

    __attribute__((__format__(gnu_scanf, 1, 2))) int scanf(const char *__restrict, ...) { assert(false); }

    __attribute__((__format__(gnu_printf, 3, 4))) int snprintf(char *__restrict, size_t, const char *__restrict, ...)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 2, 3))) int sprintf(char *__restrict, const char *__restrict, ...)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_scanf, 2, 3))) int sscanf(const char *__restrict, const char *__restrict, ...)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 2, 0))) int vfprintf(FILE *__restrict, const char *__restrict,
                                                               __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_scanf, 2, 0))) int vfscanf(FILE *__restrict, const char *__restrict,
                                                             __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 1, 0))) int vprintf(const char *__restrict, __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_scanf, 1, 0))) int vscanf(const char *__restrict, __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 3, 0))) int vsnprintf(char *__restrict, size_t, const char *__restrict,
                                                                __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 2, 0))) int vsprintf(char *__restrict, const char *__restrict,
                                                               __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_scanf, 2, 0))) int vsscanf(const char *__restrict, const char *__restrict,
                                                             __builtin_va_list)
    {
        assert(false);
    }

    __attribute__((__format__(gnu_printf, 2, 0))) int vasprintf(char **, const char *, __builtin_va_list)
    {
        assert(false);
    }

    int fgetc(FILE *) { assert(false); }

    char *fgets(char *__restrict, size_t, FILE *__restrict) { assert(false); }

    int fputc(int, FILE *) { assert(false); }

    int fputs(const char *__restrict, FILE *__restrict) { assert(false); }

    char *gets(char *) { assert(false); }

    int getc(FILE *) { assert(false); }

    int getchar(void) { assert(false); }

    int putc(int, FILE *) { assert(false); }

    int putchar(int) { assert(false); }

    int puts(const char *) { assert(false); }

    int ungetc(int c, FILE *) { assert(false); }

    size_t fread(void *__restrict, size_t, size_t, FILE *__restrict) { assert(false); }

    size_t fwrite(const void *__restrict, size_t, size_t, FILE *__restrict) { assert(false); }

    int fgetpos(FILE *__restrict, fpos_t *__restrict) { assert(false); }

    int fseek(FILE *, long, int) { assert(false); }

    int fsetpos(FILE *, const fpos_t *) { assert(false); }

    long ftell(FILE *) { assert(false); }

    void rewind(FILE *) { assert(false); }

    void clearerr(FILE *) { assert(false); }

    int feof(FILE *) { assert(false); }

    int ferror(FILE *) { assert(false); }

    void perror(const char *) { assert(false); }

    int getc_unlocked(FILE *) { assert(false); }

    int getchar_unlocked(void) { assert(false); }

    int putc_unlocked(int, FILE *) { assert(false); }

    int putchar_unlocked(int) { assert(false); }

    ssize_t getline(char **, size_t *, FILE *) { assert(false); }

    ssize_t getdelim(char **, size_t *, int, FILE *) { assert(false); }

    int asprintf(char **, const char *, ...) { assert(false); }

    void flockfile(FILE *) { assert(false); }

    void funlockfile(FILE *) { assert(false); }

    int ftrylockfile(FILE *) { assert(false); }

    void clearerr_unlocked(FILE *) { assert(false); }

    int feof_unlocked(FILE *) { assert(false); }

    int ferror_unlocked(FILE *) { assert(false); }

    int fileno_unlocked(FILE *) { assert(false); }

    int fflush_unlocked(FILE *) { assert(false); }

    int fgetc_unlocked(FILE *) { assert(false); }

    int fputc_unlocked(int, FILE *) { assert(false); }

    size_t fread_unlocked(void *__restrict, size_t, size_t, FILE *__restrict) { assert(false); }

    size_t fwrite_unlocked(const void *__restrict, size_t, size_t, FILE *__restrict) { assert(false); }

    char *fgets_unlocked(char *, int, FILE *) { assert(false); }

    int fputs_unlocked(const char *, FILE *) { assert(false); }

    int fileno(FILE *) { assert(false); }

    FILE *fdopen(int, const char *) { assert(false); }

    FILE *fmemopen(void *__restrict, size_t, const char *__restrict) { assert(false); }

    int pclose(FILE *) { assert(false); }

    FILE *popen(const char *, const char *) { assert(false); }

    FILE *open_mem(char **, size_t *) { assert(false); }

    int fseeko(FILE *, off_t, int) { assert(false); }

    off_t ftello(FILE *) { assert(false); }

    int dprintf(int, const char *, ...) { assert(false); }

    int vdprintf(int, const char *, __builtin_va_list) { assert(false); }

    char *fgetln(FILE *, size_t *) { assert(false); }

    FILE *fopencookie(void *__restrict, const char *__restrict, cookie_io_functions_t) { assert(false); }
}
