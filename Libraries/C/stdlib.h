#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

___BEGIN_DECLS

typedef struct
{
} div_t;

typedef struct
{
} ldiv_t;

typedef struct
{
} lldiv_t;

void abort(void);
int atexit(void (*func)(void));
int atoi(const char *);
char *getenv(const char *name);
void *malloc(size_t size);
void *calloc(size_t nelem, size_t elsize);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);
void _Exit(int status);
int abs(int i);
double atof(const char *str);
long atol(const char *str);
long long atoll(const char *nptr);
void *bsearch(const void *key, const void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
div_t div(int numer, int denom);
long labs(long i);
long long llabs(long long i);
ldiv_t ldiv(long numer, long denom);
lldiv_t lldiv(long long numer, long long denom);
void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
int rand(void);
int rand_r(unsigned *seed);
void srand(unsigned seed);
double strtod(const char *str, char **endptr);
float strtof(const char *nptr, char **endptr);
long int strtol(const char *str, char **endptr, int base);
long double strtold(const char *nptr, char **endptr);
unsigned long int strtoul(const char *str, char **endptr, int base);
unsigned long long strtoull(const char *str, char **endptr, int base);
int system(const char *command);

___END_DECLS
