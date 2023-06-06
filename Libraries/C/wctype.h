#ifndef _WCTYPE_H
#define _WCTYPE_H

#include <bits/locale_t.h>
#include <bits/wint_t.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned long wctype_t;
    typedef unsigned long wctrans_t;

    int iswalnum(wint_t);
    int iswalpha(wint_t);
    int iswblank(wint_t);
    int iswcntrl(wint_t);
    int iswdigit(wint_t);
    int iswgraph(wint_t);
    int iswlower(wint_t);
    int iswprint(wint_t);
    int iswpunct(wint_t);
    int iswspace(wint_t);
    int iswupper(wint_t);
    int iswxdigit(wint_t);
    wctype_t wctype(const char *);
    int iswctype(wint_t, wctype_t);
    wint_t towlower(wint_t);
    wint_t towupper(wint_t);
    wctrans_t wctrans(const char *);
    wint_t towctrans(wint_t, wctrans_t);
    int iswalnum_l(wint_t, locale_t);
    int iswblank_l(wint_t, locale_t);
    int iswcntrl_l(wint_t, locale_t);
    int iswdigit_l(wint_t, locale_t);
    int iswgraph_l(wint_t, locale_t);
    int iswlower_l(wint_t, locale_t);
    int iswprint_l(wint_t, locale_t);
    int iswpunct_l(wint_t, locale_t);
    int iswspace_l(wint_t, locale_t);
    int iswupper_l(wint_t, locale_t);
    int iswxdigit_l(wint_t, locale_t);
    int iswalpha_l(wint_t, locale_t);
    wctype_t wctype_l(const char *);
    int iswctype_l(wint_t, wctype_t);
    wint_t towlower_l(wint_t, locale_t);
    wint_t towupper_l(wint_t, locale_t);
    wctrans_t wctrans_l(const char *, locale_t);
    wint_t towctrans_l(wint_t, wctrans_t, locale_t);

#ifdef __cplusplus
}
#endif

#endif
