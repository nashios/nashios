#include <ctype.h>
#include <limits.h>
#include <st/math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __MIN_WRITE_BUF_LEN 32

#define __COPY_VA_INT()                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        const int __value = abs(va_arg(ap, int));                                                                      \
        char __buffer[32] = {};                                                                                        \
        p_format++;                                                                                                    \
        *p_specifier = '\0';                                                                                           \
        sprintf(__buffer, "%d", __value);                                                                              \
        strcat(p_specifier, __buffer);                                                                                 \
        while (*p_specifier)                                                                                           \
            p_specifier++;                                                                                             \
    } while (0)

#define __PRINT_CHAR(character)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        putc(character, stream);                                                                                       \
        p_format++;                                                                                                    \
        length++;                                                                                                      \
    } while (0)

#define __PRINT_TYPE(type)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        type __value = va_arg(ap, type);                                                                               \
        *p_specifier++ = *p_format++;                                                                                  \
        *p_specifier = '\0';                                                                                           \
        int __result = fprintf(stream, specifier, __value);                                                            \
        if (__result == -1)                                                                                            \
            return -1;                                                                                                 \
        else                                                                                                           \
        {                                                                                                              \
            length += __result;                                                                                        \
            continue;                                                                                                  \
        }                                                                                                              \
    } while (0)

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

int __stdio_write(const char *s, int size, FILE *stream)
{
    int remaining = stream->_IO_write_end - stream->_IO_write_ptr;
    int current = stream->_IO_write_end - stream->_IO_write_base;

    if (remaining < size)
    {
        int target_length = MAX(size, current * 2);
        int length = MAX(target_length, __MIN_WRITE_BUF_LEN);
        char *buffer = calloc(length, sizeof(char));
        if (buffer == NULL)
            return -1;

        memcpy(buffer, stream->_IO_write_base, current);
        free(stream->_IO_write_base);

        stream->_IO_write_base = buffer;
        stream->_IO_write_ptr = buffer + current - remaining;
        stream->_IO_write_end = buffer + length;
    }

    memcpy(stream->_IO_write_ptr, s, size);
    stream->_IO_write_ptr += size;
    stream->_offset += size;

    return size;
}

int __printf_internal(const char *format, va_list ap, FILE *stream)
{
    const char *p_format = format;
    int length = 0;
    char specifier[128] = {};

    while (*p_format != '\0')
    {
        if (*p_format != '%')
            __PRINT_CHAR(*p_format);
        else
        {
            char *p_specifier = specifier;

            *p_specifier++ = *p_format++;
            while (strchr("-+ #0", *p_format))
                *p_specifier++ = *p_format++;

            if (*p_format == '*')
                __COPY_VA_INT();
            else
                while (isdigit(*p_format))
                    *p_specifier++ = *p_format++;

            if (*p_format == '.')
            {
                *p_specifier++ = *p_format++;
                if (*p_format == '*')
                    __COPY_VA_INT();
                else
                    while (isdigit(*p_format))
                        *p_specifier++ = *p_format++;
            }

            int wide_width = 0;
            int short_width = 0;
            while (strchr("hlL", *p_format))
            {
                switch (*p_format)
                {
                case 'h':
                    short_width = 1;
                    break;
                case 'l':
                    wide_width++;
                    break;
                case 'L':
                    wide_width = 2;
                    break;
                default:
                    abort();
                }
                *p_specifier++ = *p_format++;
            }

            switch (*p_format)
            {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
            case 'c': {
                if (short_width)
                    __PRINT_TYPE(int);
                else
                {
                    switch (wide_width)
                    {
                    case 0:
                        __PRINT_TYPE(int);
                        break;
                    case 1:
                        __PRINT_TYPE(long);
                        break;
                    case 2:
                    default:
                        // FIXME: Implement long long
                        __PRINT_TYPE(long);
                        break;
                    }
                }
            }
            break;
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G': {
                if (wide_width == 0)
                    __PRINT_TYPE(double);
                else
                {
                    // FIXME: Implement long double
                    __PRINT_TYPE(double);
                }
            }
            break;
            case 's':
                __PRINT_TYPE(char *);
                break;
            case 'p':
                __PRINT_TYPE(void *);
                break;
            case '%':
                __PRINT_CHAR('%');
                break;
            default:
                abort();
            }
        }
    }

    return length;
}

int vprintf_buffer_size(const char *format, va_list ap)
{
    const char *p_format = format;
    int length = strlen(format) + 1;

    va_list ap_copy;
    va_copy(ap_copy, ap);

    while (*p_format != '\0')
    {
        if (*p_format++ == '%')
        {
            while (strchr("-+ #0", *p_format))
                p_format++;

            if (*p_format == '*')
            {
                p_format++;
                length += abs(va_arg(ap, int));
            }
            else
                length += strtoul(p_format, (char **)&p_format, 10);

            if (*p_format == '.')
            {
                p_format++;

                if (*p_format == '*')
                {
                    p_format++;
                    length += abs(va_arg(ap, int));
                }
                else
                    length += strtoul(p_format, (char **)&p_format, 10);
            }

            while (strchr("hlL", *p_format))
                p_format++;

            length += 30;
            switch (*p_format)
            {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
            case 'c':
                (void)va_arg(ap, int);
                break;

            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                (void)va_arg(ap, double);
                length += 307;
                break;

            case 's':
                length += strlen(va_arg(ap, char *));
                break;

            case 'p':
            case 'n':
                (void)va_arg(ap, char *);
                break;
            }
            p_format++;
        }
    }

    va_end(ap_copy);
    return length;
}

int vasprintf(char **strp, const char *fmt, va_list ap)
{
    int length = vprintf_buffer_size(fmt, ap);

    *strp = (char *)malloc(length);
    if (*strp != NULL)
        return vsprintf(*strp, fmt, ap);

    return -1;
}

int vsprintf(char *s, const char *format, va_list ap)
{
    FILE stream = {};
    int result = __printf_internal(format, ap, &stream);
    putc('\0', &stream);

    if (result > 0)
        memcpy(s, stream._IO_write_base, result + 1);

    return result;
}

int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap)
{
    char *buffer = NULL;
    int result = vasprintf(&buffer, format, ap);

    if (!buffer)
        return -1;

    if (result < 0)
    {
        free(buffer);
        return -1;
    }

    result = strlen(buffer);
    if (n > 0)
    {
        if ((long)n > result)
            memcpy(s, buffer, result + 1);
        else
        {
            memcpy(s, buffer, n - 1);
            s[n - 1] = 0;
        }
    }

    free(buffer);
    return result;
}

int sprintf(char *restrict s, const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    size_t length = vsnprintf(s, INT_MAX, format, args);
    va_end(args);
    return length;
}

int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap)
{
    return __printf_internal(format, ap, stream);
}

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}

int putc(int c, FILE *stream) { return fputc(c, stream); }

int fputc(int c, FILE *stream)
{
    char p_c = (unsigned char)c;
    __stdio_write(&p_c, 1, stream);
    return p_c;
}
