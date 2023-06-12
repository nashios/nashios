#include <kernel/api/posix/limits.h>
#include <kernel/lib/ctype.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/string.h>
#include <kernel/task/scheduler.h>

enum VSNFlags
{
    VSN_ZEROPAD = 1,
    VSN_SIGN = 2,
    VSN_PLUS = 4,
    VSN_SPACE = 8,
    VSN_LEFT = 16,
    VSN_SMALL = 32,
    VSN_SPECIAL = 64
};

extern void early_printf(const char *buffer, size_t length);

int vsnprintf_atoi(const char **str)
{
    int i = 0;
    while (isdigit(**str))
        i = i * 10 + *((*str)++) - '0';
    return i;
}

char *vsnprintf_number(char *str, char *end, long number, int base, int size, int precision, int type)
{
    char lower_case = (type & VSN_SMALL);
    if (type & VSN_LEFT)
        type &= ~VSN_ZEROPAD;

    if (base < 2 || base > 16)
        return NULL;

    char c = (type & VSN_ZEROPAD) ? '0' : ' ';
    char sign = 0;
    if (type & VSN_SIGN)
    {
        if (number < 0)
        {
            sign = '-';
            number = -number;
            size--;
        }
        else if (type & VSN_PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & VSN_SPACE)
        {
            sign = ' ';
            size--;
        }
    }

    if (type & VSN_SPECIAL)
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
        const char digits[16] = "0123456789ABCDEF";
        while (number != 0)
        {
            tmp[i++] = digits[((unsigned long)number) % (unsigned)base] | lower_case;
            number = ((unsigned long)number) / (unsigned)base;
        }
    }

    if (i > precision)
        precision = i;
    size -= precision;

    if (!(type & (VSN_ZEROPAD + VSN_LEFT)))
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

    if (type & VSN_SPECIAL)
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

    if (!(type & VSN_LEFT))
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

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
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

        int field_width = -1;
        if (isdigit(*format))
            field_width = vsnprintf_atoi(&format);
        else if (*format == '*')
        {
            format++;
            field_width = va_arg(ap, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= VSN_LEFT;
            }
        }

        int precision = -1;
        if (*format == '.')
        {
            format++;
            if (isdigit(*format))
                precision = vsnprintf_atoi(&format);
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
            if (!(flags & VSN_LEFT))
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

            if (!(flags & VSN_LEFT))
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
                flags |= VSN_ZEROPAD;
            }

            p_str = vsnprintf_number(p_str, end, (unsigned long)va_arg(ap, void *), 16, field_width, precision, flags);
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
            flags |= VSN_SMALL;
            // fall through
        case 'X':
            base = 16;
            break;
        case 'd':
        case 'i':
            flags |= VSN_SIGN;
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
            if (flags & VSN_SIGN)
                number = (short)number;
        }
        else if (flags & VSN_SIGN)
            number = va_arg(ap, int);
        else
            number = va_arg(ap, unsigned int);

        p_str = vsnprintf_number(p_str, end, number, base, field_width, precision, flags);
    }

    if (p_str <= end)
        *p_str = '\0';
    else if (size > 0)
        *end = '\0';

    return p_str - str;
}

int vsprintf(char *s, const char *format, va_list ap) { return vsnprintf(s, INT_MAX, format, ap); }

int sprintf(char *s, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    size_t length = vsnprintf(s, INT_MAX, format, args);
    va_end(args);

    return length;
}

int printf(const char *format, ...)
{
    char buffer[1024];
    if (g_scheduler_process && g_scheduler_thread)
        sprintf(buffer, "[%s(%d:%d)] %s", g_scheduler_process->name, g_scheduler_process->pid, g_scheduler_thread->tid,
                format);
    else
        sprintf(buffer, "[Kernel] %s", format);

    char second_buffer[1024];

    va_list args;
    va_start(args, format);
    size_t length = vsprintf(second_buffer, buffer, args);
    va_end(args);

    early_printf(second_buffer, length);
    return length;
}
