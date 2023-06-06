#ifndef _CTYPE_H
#define _CTYPE_H

#include <bits/locale_t.h>

#define _U 01
#define _L 02
#define _N 04
#define _S 010
#define _P 020
#define _C 040
#define _X 0100
#define _B 0200

#define toascii(c) ((c)&0x7f)

#ifdef __cplusplus
extern "C"
{
#endif

    extern const char _ctype_[256];

    int isalnum(int c);
    int isalpha(int c);
    int isblank(int c);
    int iscntrl(int c);
    int isdigit(int c);
    int isgraph(int c);
    int islower(int c);
    int isprint(int c);
    int ispunct(int c);
    int isspace(int c);
    int isupper(int c);
    int isxdigit(int c);
    int isascii(int c);
    int tolower(int c);
    int toupper(int c);
    int isalnum_l(int c, locale_t loc);
    int isalpha_l(int c, locale_t loc);
    int isblank_l(int c, locale_t loc);
    int iscntrl_l(int c, locale_t loc);
    int isdigit_l(int c, locale_t loc);
    int isgraph_l(int c, locale_t loc);
    int islower_l(int c, locale_t loc);
    int isprint_l(int c, locale_t loc);
    int ispunct_l(int c, locale_t loc);
    int isspace_l(int c, locale_t loc);
    int isupper_l(int c, locale_t loc);
    int isxdigit_l(int c, locale_t loc);
    int isascii_l(int c, locale_t loc);
    int tolower_l(int c, locale_t loc);
    int toupper_l(int c, locale_t loc);

#ifdef __cplusplus
}
#endif

#endif
