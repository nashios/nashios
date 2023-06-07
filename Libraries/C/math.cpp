#include <assert.h>
#include <math.h>

extern "C"
{
    int signgam;

    int __fpclassify(double x) { assert(false); }
    int __fpclassifyf(float x) { assert(false); }
    int __fpclassifyl(long double x) { assert(false); }

    void sincos(double, double *, double *) { assert(false); }

    void sincosf(float, float *, float *) { assert(false); }

    void sincosl(long double, long double *, long double *) { assert(false); }

    double exp10(double) { assert(false); }

    float exp10f(float) { assert(false); }

    long double exp10l(long double) { assert(false); }

    double pow10(double) { assert(false); }

    float pow10f(float) { assert(false); }

    long double pow10l(long double) { assert(false); }

    double acos(double x) { assert(false); }

    float acosf(float x) { assert(false); }

    long double acosl(long double x) { assert(false); }

    double asin(double x) { assert(false); }

    float asinf(float x) { assert(false); }

    long double asinl(long double x) { assert(false); }

    double atan(double x) { assert(false); }

    float atanf(float x) { assert(false); }

    long double atanl(long double x) { assert(false); }

    double atan2(double x, double y) { assert(false); }

    float atan2f(float x, float y) { assert(false); }

    long double atan2l(long double x, long double y) { assert(false); }

    double cos(double x) { assert(false); }

    float cosf(float x) { assert(false); }

    long double cosl(long double x) { assert(false); }

    double sin(double x) { assert(false); }

    float sinf(float x) { assert(false); }

    long double sinl(long double x) { assert(false); }

    double tan(double x) { assert(false); }

    float tanf(float x) { assert(false); }

    long double tanl(long double x) { assert(false); }

    double acosh(double x) { assert(false); }

    float acoshf(float x) { assert(false); }

    long double acoshl(long double x) { assert(false); }

    double asinh(double x) { assert(false); }

    float asinhf(float x) { assert(false); }

    long double asinhl(long double x) { assert(false); }

    double atanh(double x) { assert(false); }

    float atanhf(float x) { assert(false); }

    long double atanhl(long double x) { assert(false); }

    double cosh(double x) { assert(false); }

    float coshf(float x) { assert(false); }

    long double coshl(long double x) { assert(false); }

    double sinh(double x) { assert(false); }

    float sinhf(float x) { assert(false); }

    long double sinhl(long double x) { assert(false); }

    double tanh(double x) { assert(false); }

    float tanhf(float x) { assert(false); }

    long double tanhl(long double x) { assert(false); }

    double exp(double x) { assert(false); }

    float expf(float x) { assert(false); }

    long double expl(long double x) { assert(false); }

    double exp2(double x) { assert(false); }

    float exp2f(float x) { assert(false); }

    long double exp2l(long double x) { assert(false); }

    double expm1(double x) { assert(false); }

    float expm1f(float x) { assert(false); }

    long double expm1l(long double x) { assert(false); }

    double frexp(double x, int *power) { assert(false); }

    float frexpf(float x, int *power) { assert(false); }

    long double frexpl(long double x, int *power) { assert(false); }

    int ilogb(double x) { assert(false); }

    int ilogbf(float x) { assert(false); }

    int ilogbl(long double x) { assert(false); }

    double ldexp(double x, int power) { assert(false); }

    float ldexpf(float x, int power) { assert(false); }

    long double ldexpl(long double x, int power) { assert(false); }

    double log(double x) { assert(false); }

    float logf(float x) { assert(false); }

    long double logl(long double x) { assert(false); }

    double log10(double x) { assert(false); }

    float log10f(float x) { assert(false); }

    long double log10l(long double x) { assert(false); }

    double log1p(double x) { assert(false); }

    float log1pf(float x) { assert(false); }

    long double log1pl(long double x) { assert(false); }

    double log2(double x) { assert(false); }

    float log2f(float x) { assert(false); }

    long double log2l(long double x) { assert(false); }

    double logb(double x) { assert(false); }

    float logbf(float x) { assert(false); }

    long double logbl(long double x) { assert(false); }

    double modf(double x, double *integral) { assert(false); }

    float modff(float x, float *integral) { assert(false); }

    long double modfl(long double x, long double *integral) { assert(false); }

    double scalbn(double x, int power) { assert(false); }

    float scalbnf(float x, int power) { assert(false); }

    long double scalbnl(long double x, int power) { assert(false); }

    double scalbln(double x, long power) { assert(false); }

    float scalblnf(float x, long power) { assert(false); }

    long double scalblnl(long double x, long power) { assert(false); }

    double cbrt(double x) { assert(false); }

    float cbrtf(float x) { assert(false); }

    long double cbrtl(long double x) { assert(false); }

    double fabs(double x) { assert(false); }

    float fabsf(float x) { assert(false); }

    long double fabsl(long double x) { assert(false); }

    double hypot(double x, double y) { assert(false); }

    float hypotf(float x, float y) { assert(false); }

    long double hypotl(long double x, long double y) { assert(false); }

    double pow(double x, double y) { assert(false); }

    float powf(float x, float y) { assert(false); }

    long double powl(long double x, long double y) { assert(false); }

    double sqrt(double x) { assert(false); }

    float sqrtf(float x) { assert(false); }

    long double sqrtl(long double x) { assert(false); }

    double erf(double x) { assert(false); }

    float erff(float x) { assert(false); }

    long double erfl(long double x) { assert(false); }

    double erfc(double x) { assert(false); }

    float erfcf(float x) { assert(false); }

    long double erfcl(long double x) { assert(false); }

    double lgamma(double x) { assert(false); }

    float lgammaf(float x) { assert(false); }

    long double lgammal(long double x) { assert(false); }

    double tgamma(double x) { assert(false); }

    float tgammaf(float x) { assert(false); }

    long double tgammal(long double x) { assert(false); }

    double ceil(double x) { assert(false); }

    float ceilf(float x) { assert(false); }

    long double ceill(long double x) { assert(false); }

    double floor(double x) { assert(false); }

    float floorf(float x) { assert(false); }

    long double floorl(long double x) { assert(false); }

    double nearbyint(double x) { assert(false); }

    float nearbyintf(float x) { assert(false); }

    long double nearbyintl(long double x) { assert(false); }

    double rint(double x) { assert(false); }

    float rintf(float x) { assert(false); }

    long double rintl(long double x) { assert(false); }

    long lrint(double x) { assert(false); }

    long lrintf(float x) { assert(false); }

    long lrintl(long double x) { assert(false); }

    long long llrint(double x) { assert(false); }

    long long llrintf(float x) { assert(false); }

    long long llrintl(long double x) { assert(false); }

    double round(double x) { assert(false); }

    float roundf(float x) { assert(false); }

    long double roundl(long double x) { assert(false); }

    long lround(double x) { assert(false); }

    long lroundf(float x) { assert(false); }

    long lroundl(long double x) { assert(false); }

    long long llround(double x) { assert(false); }

    long long llroundf(float x) { assert(false); }

    long long llroundl(long double x) { assert(false); }

    double trunc(double x) { assert(false); }

    float truncf(float x) { assert(false); }

    long double truncl(long double x) { assert(false); }

    double fmod(double x, double y) { assert(false); }

    float fmodf(float x, float y) { assert(false); }

    long double fmodl(long double x, long double y) { assert(false); }

    double remainder(double x, double y) { assert(false); }

    float remainderf(float x, float y) { assert(false); }

    long double remainderl(long double x, long double y) { assert(false); }

    double remquo(double x, double y, int *quotient) { assert(false); }

    float remquof(float x, float y, int *quotient) { assert(false); }

    long double remquol(long double x, long double y, int *quotient) { assert(false); }

    double copysign(double x, double sign) { assert(false); }

    float copysignf(float x, float sign) { assert(false); }

    long double copysignl(long double x, long double sign) { assert(false); }

    double nan(const char *tag) { assert(false); }

    float nanf(const char *tag) { assert(false); }

    long double nanl(const char *tag) { assert(false); }

    double nextafter(double x, double dir) { assert(false); }

    float nextafterf(float x, float dir) { assert(false); }

    long double nextafterl(long double x, long double dir) { assert(false); }

    double nexttoward(double x, long double dir) { assert(false); }

    float nexttowardf(float x, long double dir) { assert(false); }

    long double nexttowardl(long double x, long double dir) { assert(false); }

    double fdim(double x, double y) { assert(false); }

    float fdimf(float x, float y) { assert(false); }

    long double fdiml(long double x, long double y) { assert(false); }

    double fmax(double x, double y) { assert(false); }

    float fmaxf(float x, float y) { assert(false); }

    long double fmaxl(long double x, long double y) { assert(false); }

    double fmin(double x, double y) { assert(false); }

    float fminf(float x, float y) { assert(false); }

    long double fminl(long double x, long double y) { assert(false); }

    double fma(double, double, double) { assert(false); }

    float fmaf(float, float, float) { assert(false); }

    long double fmal(long double, long double, long double) { assert(false); }
}
