#ifndef _STDIO_H
#define _STDIO_H

#include <bits/null.h>
#include <bits/off_t.h>
#include <bits/seek-whence.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <stdarg.h>

#define P_tmpdir "/tmp"

#define RENAME_EXCHANGE (1 << 1)

#define __MLIBC_EOF_BIT 1
#define __MLIBC_ERROR_BIT 2

#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 3

#define BUFSIZ 512

#define EOF (-1)

#define FOPEN_MAX 1024
#define FILENAME_MAX 256
#define L_tmpnam 256

#define TMP_MAX 1024

#ifdef __cplusplus
extern "C"
{
#endif

    struct __FILE
    {
        char *__buffer_ptr;
        size_t __buffer_size;
        size_t __offset;
        size_t __io_offset;
        size_t __valid_limit;
        size_t __dirty_begin;
        size_t __dirty_end;
        char *__unget_ptr;
        int __io_mode;
        int __status_bits;
    };

    typedef struct __FILE FILE;
    typedef size_t fpos_t;

    typedef ssize_t(cookie_read_function_t)(void *, char *, size_t);
    typedef ssize_t(cookie_write_function_t)(void *, const char *, size_t);
    typedef int(cookie_seek_function_t)(void *, off_t *, int);
    typedef int(cookie_close_function_t)(void *);

    typedef struct _IO_cookie_io_functions_t
    {
        cookie_read_function_t *read;
        cookie_write_function_t *write;
        cookie_seek_function_t *seek;
        cookie_close_function_t *close;
    } cookie_io_functions_t;

    extern FILE *stderr;
    extern FILE *stdin;
    extern FILE *stdout;

    int remove(const char *filename);
    int rename(const char *old_path, const char *new_path);
    int renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path);
    FILE *tmpfile(void);
    char *tmpnam(char *buffer);
    int fclose(FILE *stream);
    int fflush(FILE *stream);
    FILE *fopen(const char *__restrict filename, const char *__restrict mode);
    FILE *freopen(const char *__restrict filename, const char *__restrict mode, FILE *__restrict stream);
    void setbuf(FILE *__restrict stream, char *__restrict buffer);
    int setvbuf(FILE *__restrict stream, char *__restrict buffer, int mode, size_t size);
    void setlinebuf(FILE *stream);
    void setbuffer(FILE *, char *, size_t);

    /**
     * @brief Writes a formatted string to a stream. If format includes format specifiers (subsequences beginning with
     * %), the additional arguments following format are formatted and inserted in the resulting string replacing their
     * respective specifiers.
     *
     * @param stream Pointer to a FILE object that identifies an output stream
     * @param format C string that contains the text to be written to the stream
     *
     * @return On success, the total number of characters written is returned. On failure, the error indicator (ferror)
     * is set and a negative number is returned.
     */
    __attribute__((__format__(gnu_printf, 2, 3))) int fprintf(FILE *__restrict stream, const char *__restrict format,
                                                              ...);
    __attribute__((__format__(gnu_scanf, 2, 3))) int fscanf(FILE *__restrict stream, const char *__restrict format,
                                                            ...);

    /**
     * @brief Writes a formatted string to stdout. If format includes format specifiers (subsequences beginning with %),
     * the additional arguments following format are formatted and inserted in the resulting string replacing their
     * respective specifiers.
     *
     * @param format C string that contains the text to be written to stdout
     *
     * @return On success, the total number of characters written is returned. On failure, the error indicator (ferror)
     * is set and a negative number is returned.
     */
    __attribute__((__format__(gnu_printf, 1, 2))) int printf(const char *__restrict format, ...);
    __attribute__((__format__(gnu_scanf, 1, 2))) int scanf(const char *__restrict format, ...);

    /**
     * @brief Writes a formatted string to a buffer. If format includes format specifiers (subsequences beginning with
     * %), the additional arguments following format are formatted and inserted in the resulting string replacing their
     * respective specifiers
     *
     * @param s Pointer to a buffer where the resulting C-string is stored. The buffer should be large enough to
     * contain the resulting string
     * @param n Maximum number of bytes to be used in the buffer
     * @param format C string that contains the text to be written to the buffer
     *
     * @return On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    __attribute__((__format__(gnu_printf, 3, 4))) int snprintf(char *__restrict s, size_t n,
                                                               const char *__restrict format, ...);

    /**
     * @brief Writes a formatted string to a buffer. If format includes format specifiers (subsequences beginning with
     * %), the additional arguments following format are formatted and inserted in the resulting string replacing their
     * respective specifiers
     *
     * @param str Pointer to a buffer where the resulting C-string is stored. The buffer should be large enough to
     * contain the resulting string
     * @param format C string that contains the text to be written to the buffer
     *
     * @return On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    __attribute__((__format__(gnu_printf, 2, 3))) int sprintf(char *__restrict str, const char *__restrict format, ...);
    __attribute__((__format__(gnu_scanf, 2, 3))) int sscanf(const char *__restrict buffer,
                                                            const char *__restrict format, ...);

    /**
     * @brief Writes a formatted string to a stream using a pointer to a list of arguments.
     *
     * @param stream Pointer to a FILE object that identifies an output stream.
     * @param format C string that contains the text to be written to the stream
     * @param args A value identifying a variable arguments list initialized with va_start.
     *
     * @return On success, the total number of characters written is returned. On failure, the error indicator (ferror)
     * is set and a negative number is returned.
     */
    __attribute__((__format__(gnu_printf, 2, 0))) int vfprintf(FILE *__restrict stream, const char *__restrict format,
                                                               va_list args);
    __attribute__((__format__(gnu_scanf, 2, 0))) int vfscanf(FILE *__restrict stream, const char *__restrict format,
                                                             va_list args);
    __attribute__((__format__(gnu_printf, 1, 0))) int vprintf(const char *__restrict format, va_list args);
    __attribute__((__format__(gnu_scanf, 1, 0))) int vscanf(const char *__restrict format, va_list args);
    __attribute__((__format__(gnu_printf, 3, 0))) int vsnprintf(char *__restrict buffer, size_t max_size,
                                                                const char *__restrict format, va_list args);
    __attribute__((__format__(gnu_printf, 2, 0))) int vsprintf(char *__restrict buffer, const char *__restrict format,
                                                               va_list args);
    __attribute__((__format__(gnu_scanf, 2, 0))) int vsscanf(const char *__restrict buffer,
                                                             const char *__restrict format, va_list args);
    __attribute__((__format__(gnu_printf, 2, 0))) int vasprintf(char **, const char *, va_list);

    int fgetc(FILE *stream);
    char *fgets(char *__restrict buffer, size_t max_size, FILE *__restrict stream);
    int fputc(int c, FILE *stream);
    int fputs(const char *__restrict string, FILE *__restrict stream);
    char *gets(char *s);
    int getc(FILE *stream);
    int getchar(void);
    int putc(int c, FILE *stream);
    int putchar(int c);
    int puts(const char *string);
    int ungetc(int c, FILE *stream);
    size_t fread(void *__restrict buffer, size_t size, size_t count, FILE *__restrict stream);
    size_t fwrite(const void *__restrict buffer, size_t size, size_t count, FILE *__restrict stream);
    int fgetpos(FILE *__restrict stream, fpos_t *__restrict position);
    int fseek(FILE *stream, long offset, int whence);
    int fsetpos(FILE *stream, const fpos_t *position);
    long ftell(FILE *stream);
    void rewind(FILE *stream);
    void clearerr(FILE *stream);
    int feof(FILE *stream);
    int ferror(FILE *stream);
    void perror(const char *string);
    int getc_unlocked(FILE *);
    int getchar_unlocked(void);
    int putc_unlocked(int, FILE *);
    int putchar_unlocked(int);
    ssize_t getline(char **, size_t *, FILE *);
    ssize_t getdelim(char **, size_t *, int, FILE *);
    int asprintf(char **, const char *, ...);
    void flockfile(FILE *);
    void funlockfile(FILE *);
    int ftrylockfile(FILE *);
    void clearerr_unlocked(FILE *);
    int feof_unlocked(FILE *);
    int ferror_unlocked(FILE *);
    int fileno_unlocked(FILE *);
    int fflush_unlocked(FILE *);
    int fgetc_unlocked(FILE *);
    int fputc_unlocked(int, FILE *);
    size_t fread_unlocked(void *__restrict, size_t, size_t, FILE *__restrict);
    size_t fwrite_unlocked(const void *__restrict, size_t, size_t, FILE *__restrict);
    char *fgets_unlocked(char *, int, FILE *);
    int fputs_unlocked(const char *, FILE *);
    int fileno(FILE *file);
    FILE *fdopen(int fd, const char *mode);
    FILE *fmemopen(void *__restrict, size_t, const char *__restrict);
    int pclose(FILE *);
    FILE *popen(const char *, const char *);
    FILE *open_memstream(char **, size_t *);
    int fseeko(FILE *stream, off_t offset, int whence);
    off_t ftello(FILE *stream);
    int dprintf(int fd, const char *format, ...);
    int vdprintf(int fd, const char *format, va_list args);
    char *fgetln(FILE *, size_t *);
    FILE *fopencookie(void *__restrict cookie, const char *__restrict mode, cookie_io_functions_t io_funcs);

#ifdef __cplusplus
}
#endif

#endif
