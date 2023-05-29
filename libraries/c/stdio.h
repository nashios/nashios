#pragma once

#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0

#define stderr stderr

typedef struct
{
    int unused;
} FILE;

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
int snprintf(char *s, size_t n, const char *format, ...);
int sprintf(char *s, const char *format, ...);
