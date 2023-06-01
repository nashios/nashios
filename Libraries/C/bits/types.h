#pragma once

#include <bits/typesizes.h>
#include <bits/worldsize.h>

#define __S32_TYPE int
#define __U32_TYPE unsigned int
#define __SLONGWORD_TYPE long int

#if __WORDSIZE == 32
#define __SQUAD_TYPE __int64_t
#define __SWORD_TYPE int
#define __STD_TYPE __extension__ typedef

#endif

#if __WORDSIZE == 32
__extension__ typedef signed long long int __int64_t;
#endif

__STD_TYPE __OFF_T_TYPE __off_t;
__STD_TYPE __PID_T_TYPE __pid_t;
__STD_TYPE __CLOCK_T_TYPE __clock_t;
__STD_TYPE __TIME_T_TYPE __time_t;
__STD_TYPE __SSIZE_T_TYPE __ssize_t;
__STD_TYPE __MODE_T_TYPE __mode_t;

#if __TIMESIZE == 64 && defined __LIBC
#define __time64_t __time_t
#elif __TIMESIZE != 64
__STD_TYPE __TIME64_T_TYPE __time64_t;
#endif

typedef int __sig_atomic_t;
