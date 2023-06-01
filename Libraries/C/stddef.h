#pragma once

#include <sys/cdefs.h>

#define offsetof(type, member) __builtin_offsetof(type, member)

#ifdef __cplusplus
#define NULL nullptr
#else
#define NULL ((void *)0)
#endif

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __SIZE_TYPE__ size_t;

typedef struct
{
} max_align_t;
