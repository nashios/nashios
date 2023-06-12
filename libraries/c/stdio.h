#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0

#ifdef __cplusplus
extern "C"
{
#endif

    struct _FILE
    {
        int _flags;
        int _offset;
        char *_IO_write_base;
        char *_IO_write_ptr;
        char *_IO_write_end;
    };

    typedef struct _FILE FILE;

    extern FILE *stdin;
    extern FILE *stdout;
    extern FILE *stderr;

#define stdin stdin
#define stdout stdout
#define stderr stderr

    int fclose(FILE *);
    int fflush(FILE *);
    FILE *fopen(const char *, const char *);

    /**
     * @brief Write formatted output to a stream
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fprintf.html
     *
     * @param stream The stream to write to
     * @param format The format string
     * @param ... The arguments to the format string
     * @return int On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    int fprintf(FILE *restrict stream, const char *restrict format, ...);
    size_t fread(void *, size_t, size_t, FILE *);
    int fseek(FILE *, long, int);
    long ftell(FILE *);
    size_t fwrite(const void *, size_t, size_t, FILE *);
    void setbuf(FILE *, char *);

    /**
     * @brief Write formatted output using a pointer to a list of arguments
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/vfprintf.html
     *
     * @param stream The stream to write to
     * @param format The format string
     * @param ap The arguments to the format string
     * @return int On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap);

    /**
     * @brief Write formatted output
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/vsprintf.html
     *
     * @param s The buffer to write to
     * @param format The format string
     * @param ap The arguments to the format string
     * @return int On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    int vsprintf(char *s, const char *format, va_list ap);

    /**
     * @brief Write formatted output to sized buffer
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/vsnprintf.html
     *
     * @param s The buffer to write to
     * @param n The size of the buffer
     * @param format The format string
     * @param ap The arguments to the format string
     * @return int On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap);

    /**
     * @brief Write formatted output to buffer
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/sprintf.html
     *
     * @param s The buffer to write to
     * @param format The format string
     * @param ... The arguments to the format string
     * @return int On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    int sprintf(char *restrict s, const char *restrict format, ...);

    /**
     * @brief Print to allocated string
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/vasprintf.html
     *
     * @param strp The string to write to
     * @param fmt The format string
     * @param ap The arguments to the format string
     * @return int On success, the total number of characters written is returned. On failure, a negative number is
     * returned.
     */
    int vasprintf(char **strp, const char *fmt, va_list ap);

    /**
     * @brief Put a byte on a stream
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/putc.html
     *
     * @param c The byte to put
     * @param stream The stream to put the byte on
     * @return int On success, the character written is returned. On failure, EOF is returned.
     */
    int putc(int c, FILE *stream);

    /**
     * @brief Put a byte on a stream
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fputc.html
     *
     * @param c The byte to put
     * @param stream The stream to put the byte on
     * @return int On success, the character written is returned. On failure, EOF is returned.
     */
    int fputc(int c, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
