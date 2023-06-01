#pragma once

#define __PID_T_TYPE __S32_TYPE
#define __MODE_T_TYPE __U32_TYPE
#define __SSIZE_T_TYPE __SWORD_TYPE
#define __OFF_T_TYPE __SLONGWORD_TYPE
#define __CLOCK_T_TYPE __SLONGWORD_TYPE
#define __TIME_T_TYPE __SLONGWORD_TYPE

#if __TIMESIZE == 64
#define __TIME64_T_TYPE __TIME_T_TYPE
#else
#define __TIME64_T_TYPE __SQUAD_TYPE
#endif
