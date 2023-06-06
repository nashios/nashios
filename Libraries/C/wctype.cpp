#include <assert.h>
#include <wctype.h>

extern "C"
{
    int iswalnum(wint_t) { assert(false); }

    int iswalpha(wint_t) { assert(false); }

    int iswblank(wint_t) { assert(false); }

    int iswcntrl(wint_t) { assert(false); }

    int iswdigit(wint_t) { assert(false); }

    int iswgraph(wint_t) { assert(false); }

    int iswlower(wint_t) { assert(false); }

    int iswprint(wint_t) { assert(false); }

    int iswpunct(wint_t) { assert(false); }

    int iswspace(wint_t) { assert(false); }

    int iswupper(wint_t) { assert(false); }

    int iswxdigit(wint_t) { assert(false); }

    wctype_t wctype(const char *) { assert(false); }

    int iswctype(wint_t, wctype_t) { assert(false); }

    wint_t towlower(wint_t) { assert(false); }

    wint_t towupper(wint_t) { assert(false); }

    wctrans_t wctrans(const char *) { assert(false); }

    wint_t towctrans(wint_t, wctrans_t) { assert(false); }

    int iswalnum_l(wint_t, locale_t) { assert(false); }

    int iswblank_l(wint_t, locale_t) { assert(false); }

    int iswcntrl_l(wint_t, locale_t) { assert(false); }

    int iswdigit_l(wint_t, locale_t) { assert(false); }

    int iswgraph_l(wint_t, locale_t) { assert(false); }

    int iswlower_l(wint_t, locale_t) { assert(false); }

    int iswprint_l(wint_t, locale_t) { assert(false); }

    int iswpunct_l(wint_t, locale_t) { assert(false); }

    int iswspace_l(wint_t, locale_t) { assert(false); }

    int iswupper_l(wint_t, locale_t) { assert(false); }

    int iswxdigit_l(wint_t, locale_t) { assert(false); }

    int iswalpha_l(wint_t, locale_t) { assert(false); }

    wctype_t wctype_l(const char *) { assert(false); }

    int iswctype_l(wint_t, wctype_t) { assert(false); }

    wint_t towlower_l(wint_t, locale_t) { assert(false); }

    wint_t towupper_l(wint_t, locale_t) { assert(false); }

    wctrans_t wctrans_l(const char *, locale_t) { assert(false); }

    wint_t towctrans_l(wint_t, wctrans_t, locale_t) { assert(false); }
}
