#include <assert.h>
#include <ctype.h>

extern "C"
{
    const char _ctype_[256] = {0,       _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C | _S, _C | _S,
                               _C | _S, _C | _S,
                               _C | _S, _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      _C,
                               _C,      (char)(_S | _B),
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _P,      _N,
                               _N,      _N,
                               _N,      _N,
                               _N,      _N,
                               _N,      _N,
                               _N,      _P,
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _U | _X, _U | _X,
                               _U | _X, _U | _X,
                               _U | _X, _U | _X,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _U,      _U,
                               _P,      _P,
                               _P,      _P,
                               _P,      _P,
                               _L | _X, _L | _X,
                               _L | _X, _L | _X,
                               _L | _X, _L | _X,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _L,      _L,
                               _P,      _P,
                               _P,      _P,
                               _C};

    int isalnum(int) { assert(false); }

    int isalpha(int) { assert(false); }

    int isblank(int) { assert(false); }

    int iscntrl(int) { assert(false); }

    int isdigit(int) { assert(false); }

    int isgraph(int) { assert(false); }

    int islower(int) { assert(false); }

    int isprint(int) { assert(false); }

    int ispunct(int) { assert(false); }

    int isspace(int) { assert(false); }

    int isupper(int) { assert(false); }

    int isxdigit(int) { assert(false); }

    int isascii(int) { assert(false); }

    int tolower(int) { assert(false); }

    int toupper(int) { assert(false); }

    int isalnum_l(int, locale_t) { assert(false); }

    int isalpha_l(int, locale_t) { assert(false); }

    int isblank_l(int, locale_t) { assert(false); }

    int iscntrl_l(int, locale_t) { assert(false); }

    int isdigit_l(int, locale_t) { assert(false); }

    int isgraph_l(int, locale_t) { assert(false); }

    int islower_l(int, locale_t) { assert(false); }

    int isprint_l(int, locale_t) { assert(false); }

    int ispunct_l(int, locale_t) { assert(false); }

    int isspace_l(int, locale_t) { assert(false); }

    int isupper_l(int, locale_t) { assert(false); }

    int isxdigit_l(int, locale_t) { assert(false); }

    int isascii_l(int, locale_t) { assert(false); }

    int tolower_l(int, locale_t) { assert(false); }

    int toupper_l(int, locale_t) { assert(false); }
}
