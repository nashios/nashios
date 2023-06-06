#pragma once

#define _STDIO_H

#include <kernel/api/filesystem.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#define EOF (-1)

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSIZ 1024

___BEGIN_DECLS

typedef struct
{
    int unused;
} FILE;

typedef off_t fpos_t;

extern FILE *stdout;
extern FILE *stdin;
extern FILE *stderr;

int fclose(FILE *);
int fflush(FILE *);
FILE *fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
size_t fread(void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);
void setbuf(FILE *, char *);
int snprintf(char *s, size_t n, const char *format, ...);
int sprintf(char *s, const char *format, ...);
int fileno(FILE *stream);
FILE *fdopen(int fildes, const char *mode);
int printf(const char *format, ...);
int feof(FILE *stream);
void clearerr(FILE *stream);
int ferror(FILE *stream);
int fgetc(FILE *stream);
int fgetpos(FILE *stream, fpos_t *pos);
char *fgets(char *s, int n, FILE *stream);
int fputc(int c, FILE *stream);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
int fscanf(FILE *stream, const char *format, ...);
int scanf(const char *format, ...);
int sscanf(const char *s, const char *format, ...);
int fsetpos(FILE *stream, const fpos_t *pos);
int getc(FILE *stream);
int getchar(void);
void perror(const char *s);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);
int remove(const char *path);
int rename(const char *old_filename, const char *new_filename);
void rewind(FILE *stream);
int setvbuf(FILE *stream, char *buf, int type, size_t size);
FILE *tmpfile(void);
int ungetc(int c, FILE *stream);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vprintf(const char *format, va_list ap);
int vsnprintf(char *s, size_t n, const char *format, va_list ap);
int vsprintf(char *s, const char *format, va_list ap);
int vdprintf(int fildes, const char *format, va_list ap);
int fputs(const char *s, FILE *stream);

___END_DECLS
