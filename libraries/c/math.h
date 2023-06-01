#pragma once

#include <sys/cdefs.h>

#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4

___BEGIN_DECLS

double acos(double x);
float acosf(float x);
long double acosl(long double x);
double asin(double x);
float asinf(float x);
long double asinl(long double x);
double atan(double x);
float atanf(float x);
long double atanl(long double x);
double atan2(double y, double x);
float atan2f(float y, float x);
long double atan2l(long double y, long double x);
double ceil(double x);
float ceilf(float x);
long double ceill(long double x);
double cos(double x);
float cosf(float x);
long double cosl(long double x);
double cosh(double x);
float coshf(float x);
long double coshl(long double x);
double exp(double x);
float expf(float x);
long double expl(long double x);
double fabs(double x);
float fabsf(float x);
long double fabsl(long double x);
double floor(double x);
float floorf(float x);
long double floorl(long double x);
double fmod(double x, double y);
float fmodf(float x, float y);
long double fmodl(long double x, long double y);
double frexp(double num, int *exp);
float frexpf(float num, int *exp);
long double frexpl(long double num, int *exp);
double ldexp(double x, int exp);
float ldexpf(float x, int exp);
long double ldexpl(long double x, int exp);
double log(double x);
float logf(float x);
long double logl(long double x);
double log10(double x);
float log10f(float x);
long double log10l(long double x);
double modf(double x, double *iptr);
float modff(float value, float *iptr);
long double modfl(long double value, long double *iptr);
double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);
double sinh(double x);
float sinhf(float x);
long double sinhl(long double x);
double sin(double x);
float sinf(float x);
long double sinl(long double x);
double sqrt(double x);
float sqrtf(float x);
long double sqrtl(long double x);
double tan(double x);
float tanf(float x);
long double tanl(long double x);
double tanh(double x);
float tanhf(float x);
long double tanhl(long double x);

___END_DECLS
