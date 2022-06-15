#pragma once

#define SEEK_SET 0

#define stderr stderr

#include <stdarg.h>
#include <stddef.h>

typedef struct
{
    int unused;
} FILE;

enum vsn_flags
{
    VSN_ZEROPAD = 1,
    VSN_SIGN = 2,
    VSN_PLUS = 4,
    VSN_SPACE = 8,
    VSN_LEFT = 16,
    VSN_SMALL = 32,
    VSN_SPECIAL = 64
};

extern FILE *stderr;

int fclose(FILE *);
int fflush(FILE *);
FILE *fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
size_t fread(void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
void setbuf(FILE *, char *);
int vfprintf(FILE *, const char *, va_list);
int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap);
