#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

FILE *stdin;
FILE *stdout;
FILE *stderr;

int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap)
{
    // FIXME implement
    return -1;
}

int vprintf(const char *restrict format, va_list ap)
{
    return vfprintf(stdout, format, ap);
}

int vsnprintf_skip(const char *restrict *s)
{
    int i = 0;
    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

char *vsnprintf_number(char *s, long number, int base, int width, int precision, int flags)
{
    if (base < 2 || base > 16)
        return NULL;

    if (flags & VSN_LEFT)
        flags &= ~VSN_ZEROPAD;

    char sign = 0;
    if (flags & VSN_SIGN)
    {
        if (number < 0)
        {
            sign = '-';
            number = -number;
            width--;
        }
        else if (flags & VSN_PLUS)
        {
            sign = '+';
            width--;
        }
        else if (flags & VSN_SPACE)
        {
            sign = ' ';
            width--;
        }
    }

    if (flags & VSN_SPECIAL)
    {
        if (base == 16)
            width -= 2;
        else if (base == 8)
            width--;
    }

    char tmp[66] = {};
    char lowcase = (flags & VSN_SMALL);
    int i = 0;
    if (number == 0)
        tmp[i++] = '0';
    else
    {
        const char digits[16] = "0123456789ABCDEF";
        while (number != 0)
        {
            tmp[i++] = (digits[((unsigned long)number) % (unsigned)base] | lowcase);
            number = ((unsigned long)number) / (unsigned)base;
        }
    }

    if (i > precision)
        precision = i;
    width -= precision;

    if (!(flags & (VSN_ZEROPAD + VSN_LEFT)))
    {
        while (width-- > 0)
            *s++ = ' ';
    }

    if (sign)
        *s++ = sign;

    if (flags & VSN_SPECIAL)
    {
        if (base == 8)
            *s++ = '0';
        else if (base == 16)
        {
            *s++ = '0';
            *s++ = ('X' | lowcase);
        }
    }

    if (!(flags & VSN_LEFT))
    {
        char ch = (flags & VSN_ZEROPAD) ? '0' : ' ';
        while (width-- > 0)
            *s++ = ch;
    }

    while (i < precision--)
        *s++ = '0';

    while (i-- > 0)
        *s++ = tmp[i];

    while (width-- > 0)
        *s++ = ' ';

    return s;
}

int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap)
{
    if ((int)n < 0)
        return 0;

    char *p_s;
    for (p_s = s; *format; format++)
    {
        if (*format != '%')
        {
            *p_s++ = *format;
            continue;
        }

        int flags = 0;
    repeat_flags:
        switch (*++format)
        {
        case '-':
            flags |= VSN_LEFT;
            goto repeat_flags;
        case '+':
            flags |= VSN_PLUS;
            goto repeat_flags;
        case ' ':
            flags |= VSN_SPACE;
            goto repeat_flags;
        case '#':
            flags |= VSN_SPECIAL;
            goto repeat_flags;
        case '0':
            flags |= VSN_ZEROPAD;
            goto repeat_flags;
        }

        int width = -1;
        if (isdigit(*format))
            width = vsnprintf_skip(&format);
        else if (*format == '*')
        {
            format++;
            width = va_arg(ap, int);
            if (width < 0)
            {
                width = -width;
                flags |= VSN_LEFT;
            }
        }

        int precision = -1;
        if (*format == '.')
        {
            format++;

            if (isdigit(*format))
                precision = vsnprintf_skip(&format);
            else if (*format == '*')
            {
                format++;
                precision = va_arg(ap, int);
            }

            if (precision < 0)
                precision = 0;
        }

        int length = -1;
        if (*format == 'h' || *format == 'l' || *format == 'L')
            length = *format++;

        int base;
        switch (*format)
        {
        case 'd':
        case 'i':
            flags |= VSN_SIGN;
            [[fallthrough]];
        case 'u':
            base = 10;
            break;
        case 'o':
            base = 8;
            break;
        case 'x':
            flags |= VSN_SMALL;
            [[fallthrough]];
        case 'X':
            base = 16;
            break;
        case 'c':
            if (!(flags & VSN_LEFT))
            {
                while (--width > 0)
                    *p_s++ = ' ';
            }

            *p_s++ = (unsigned char)va_arg(ap, int);

            while (--width > 0)
                *p_s++ = ' ';

            continue;
        case 's':
        {
            char *string = va_arg(ap, char *);
            size_t string_len = strnlen_s(string, precision);

            if (!(flags & VSN_LEFT))
            {
                while ((int)string_len < width--)
                    *p_s++ = ' ';
            }

            for (size_t i = 0; i < string_len; ++i)
                *p_s++ = *string++;

            while ((int)string_len < width--)
                *p_s++ = ' ';

            continue;
        }
        case 'p':
        {
            if (width == -1)
            {
                width = 2 * sizeof(void *);
                flags |= VSN_ZEROPAD;
            }
            unsigned long number = (unsigned long)va_arg(ap, void *);
            p_s = vsnprintf_number(p_s, number, 16, width, precision, flags);
            continue;
        }
        case 'n':
            if (length == 'l')
            {
                long *location = va_arg(ap, long *);
                *location = (p_s - s);
            }
            else
            {
                int *location = va_arg(ap, int *);
                *location = (p_s - s);
            }
            continue;
        case '%':
            *p_s++ = '%';
            continue;
        default:
            *p_s++ = '%';

            if (*format)
                *p_s++ = *format;
            else
                --format;

            continue;
        }

        unsigned long number;
        if (length == 'l')
            number = va_arg(ap, unsigned long);
        else if (length == 'h')
        {
            number = (unsigned short)va_arg(ap, int);
            if (flags & VSN_SIGN)
                number = (short)number;
        }
        else if (flags & VSN_SIGN)
            number = va_arg(ap, int);
        else
            number = va_arg(ap, unsigned int);

        p_s = vsnprintf_number(p_s, number, base, width, precision, flags);
    }

    *p_s = '\0';
    return p_s - s;
}

int vsprintf(char *restrict s, const char *restrict format, va_list ap)
{
    return vsnprintf(s, INT32_MAX, format, ap);
}

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vfprintf(stream, format, ap);
    va_end(ap);
    return result;
}

int printf(const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vprintf(format, ap);
    va_end(ap);
    return result;
}

int snprintf(char *restrict s, size_t n, const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(s, n, format, ap);
    va_end(ap);
    return result;
}

int sprintf(char *restrict s, const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsprintf(s, format, ap);
    va_end(ap);
    return result;
}
