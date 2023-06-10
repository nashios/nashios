#include <stdio.h>

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

int vfprintf(FILE *restrict, const char *restrict, va_list) { return -1; }

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}
