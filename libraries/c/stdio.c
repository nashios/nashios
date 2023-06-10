#include <stdio.h>

int vfprintf(FILE *restrict , const char *restrict , va_list ) {}

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}
