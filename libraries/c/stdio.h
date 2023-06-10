#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0

typedef struct
{
    int unused;
} FILE;

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif

#endif
