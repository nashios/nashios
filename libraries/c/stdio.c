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

enum _vfp_flags
{
    _VFP_ZEROPAD = 1,
    _VFP_SIGN = 2,
    _VFP_PLUS = 4,
    _VFP_SPACE = 8,
    _VFP_LEFT = 16,
    _VFP_SMALL = 32,
    _VFP_SPECIAL = 64
};

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

int vfprintf_atoi(const char *restrict *str)
{
    int i = 0;
    while (isdigit(**str))
        i = i * 10 + *((*str)++) - '0';
    return i;
}

char *vfprintf_number(char *str, char *end, long number, int base, int size, int precision, int type)
{
    char lower_case = (type & _VFP_SMALL);
    if (type & _VFP_LEFT)
        type &= ~_VFP_ZEROPAD;

    if (base < 2 || base > 16)
        return NULL;

    char c = (type & _VFP_ZEROPAD) ? '0' : ' ';
    char sign = 0;
    if (type & _VFP_SIGN)
    {
        if (number < 0)
        {
            sign = '-';
            number = -number;
            size--;
        }
        else if (type & _VFP_PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & _VFP_SPACE)
        {
            sign = ' ';
            size--;
        }
    }

    if (type & _VFP_SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }

    char tmp[66];
    int i = 0;
    if (number == 0)
        tmp[i++] = '0';
    else
    {
        const char digits[17] = "0123456789ABCDEF";
        while (number != 0)
        {
            tmp[i++] = digits[((unsigned long)number) % (unsigned)base] | lower_case;
            number = ((unsigned long)number) / (unsigned)base;
        }
    }

    if (i > precision)
        precision = i;
    size -= precision;

    if (!(type & (_VFP_ZEROPAD + _VFP_LEFT)))
    {
        while (size-- > 0)
        {
            if (str <= end)
                *str = ' ';
            str++;
        }
    }

    if (sign)
    {
        if (str <= end)
            *str = sign;
        str++;
    }

    if (type & _VFP_SPECIAL)
    {
        if (base == 8)
        {
            if (str <= end)
                *str = '0';
            str++;
        }
        else if (base == 16)
        {
            if (str <= end)
                *str = '0';
            str++;

            if (str <= end)
                *str = ('X' | lower_case);
            str++;
        }
    }

    if (!(type & _VFP_LEFT))
    {
        while (size-- > 0)
        {
            if (str <= end)
                *str = c;
            str++;
        }
    }

    while (i < precision--)
    {
        if (str <= end)
            *str = '0';
        str++;
    }

    while (i-- > 0)
    {
        if (str <= end)
            *str = tmp[i];
        str++;
    }

    while (size-- > 0)
    {
        if (str <= end)
            *str = ' ';
        str++;
    }

    return str;
}

int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap)
{
    int size = vprintf_buffer_size(format, ap);
    char *str = (char *)malloc(size);
    if (str == NULL)
        return -1;

    char *p_str = str;
    char *end = str + size;
    if (end < str)
    {
        end = ((void *)-1);
        size = end - str;
    }

    for (; *format; format++)
    {
        if (*format != '%')
        {
            if (p_str <= end)
                *p_str = *format;
            p_str++;
            continue;
        }

        int flags = 0;
    repeat_flags:
        format++;
        switch (*format)
        {
        case '-':
            flags |= _VFP_LEFT;
            goto repeat_flags;
        case '+':
            flags |= _VFP_PLUS;
            goto repeat_flags;
        case ' ':
            flags |= _VFP_SPACE;
            goto repeat_flags;
        case '#':
            flags |= _VFP_SPECIAL;
            goto repeat_flags;
        case '0':
            flags |= _VFP_ZEROPAD;
            goto repeat_flags;
        }

        int field_width = -1;
        if (isdigit(*format))
            field_width = vfprintf_atoi(&format);
        else if (*format == '*')
        {
            format++;
            field_width = va_arg(ap, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= _VFP_LEFT;
            }
        }

        int precision = -1;
        if (*format == '.')
        {
            format++;
            if (isdigit(*format))
                precision = vfprintf_atoi(&format);
            else if (*format == '*')
            {
                format++;
                precision = va_arg(ap, int);
            }
            if (precision < 0)
                precision = 0;
        }

        int qualifier = -1;
        if (*format == 'h' || *format == 'l' || *format == 'L')
        {
            qualifier = *format;
            format++;
        }

        int base = 10;
        switch (*format)
        {
        case 'c':
            if (!(flags & _VFP_LEFT))
            {
                while (--field_width > 0)
                    *p_str++ = ' ';
            }

            *p_str++ = (unsigned char)va_arg(ap, int);
            while (--field_width > 0)
                *p_str++ = ' ';

            continue;
        case 's': {
            const char *s = va_arg(ap, char *);
            size_t length = strnlen(s, precision);

            if (!(flags & _VFP_LEFT))
            {
                while ((int)length < field_width--)
                {
                    if (p_str <= end)
                        *p_str = ' ';
                    p_str++;
                }
            }

            for (size_t i = 0; i < length; i++)
            {
                if (p_str <= end)
                    *p_str = *s;

                p_str++;
                s++;
            }

            while ((int)length < field_width--)
            {
                if (p_str <= end)
                    *p_str = ' ';
                p_str++;
            }

            continue;
        }
        case 'p':
            if (field_width == -1)
            {
                field_width = 2 * sizeof(void *);
                flags |= _VFP_ZEROPAD;
            }

            p_str = vfprintf_number(p_str, end, (unsigned long)va_arg(ap, void *), 16, field_width, precision, flags);
            continue;
        case 'n':
            if (qualifier == 'l')
            {
                long *ip = va_arg(ap, long *);
                *ip = (p_str - str);
            }
            else
            {
                int *ip = va_arg(ap, int *);
                *ip = (p_str - str);
            }
            continue;
        case '%':
            if (p_str <= end)
                *p_str = '%';
            p_str++;
            continue;
        case 'o':
            base = 8;
            break;
        case 'x':
            flags |= _VFP_SMALL;
            // fall through
        case 'X':
            base = 16;
            break;
        case 'd':
        case 'i':
            flags |= _VFP_SIGN;
        case 'u':
            break;
        default:
            if (p_str <= end)
                *p_str = '%';
            p_str++;

            if (*format)
            {
                if (p_str <= end)
                    *p_str = *format;
                p_str++;
            }
            else
                format--;

            continue;
        }

        unsigned long number = 0;
        if (qualifier == 'l')
            number = va_arg(ap, unsigned long);
        else if (qualifier == 'h')
        {
            number = (unsigned short)va_arg(ap, int);
            if (flags & _VFP_SIGN)
                number = (short)number;
        }
        else if (flags & _VFP_SIGN)
            number = va_arg(ap, int);
        else
            number = va_arg(ap, unsigned int);

        p_str = vfprintf_number(p_str, end, number, base, field_width, precision, flags);
    }

    if (p_str <= end)
        *p_str = '\0';
    else if (size > 0)
        *end = '\0';

    __stdio_write(str, p_str - str, stream);
    return p_str - str;
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

int fputs(const char *restrict s, FILE *restrict stream)
{
    size_t length = strlen(s);
    return __stdio_write(s, length, stream);
}

int printf(const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int fileno(FILE *stream) { return stream->_fd; }