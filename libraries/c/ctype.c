#include <ctype.h>

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


#undef isalnum
int isalnum(int c)
{
    return ___isalnum(c);
}

#undef isalpha
int isalpha(int c)
{
    return ___isalpha(c);
}

#undef iscntrl
int iscntrl(int c)
{
    return ___iscntrl(c);
}

#undef isdigit
int isdigit(int c)
{
    return ___isdigit(c);
}

#undef isxdigit
int isxdigit(int c)
{
    return ___isxdigit(c);
}

#undef isspace
int isspace(int c)
{
    return ___isspace(c);
}

#undef ispunct
int ispunct(int c)
{
    return ___ispunct(c);
}

#undef isprint
int isprint(int c)
{
    return ___isprint(c);
}

#undef isgraph
int isgraph(int c)
{
    return ___isgraph(c);
}

#undef isupper
int isupper(int c)
{
    return ___isupper(c);
}

#undef islower
int islower(int c)
{
    return ___islower(c);
}

#undef isascii
int isascii(int c)
{
    return ___isascii(c);
}

#undef isblank
int isblank(int c)
{
    return ___isblank(c);
}

#undef toascii
int toascii(int c)
{
    return ___toascii(c);
}

#undef tolower
int tolower(int c)
{
    return ___tolower(c);
}

#undef toupper
int toupper(int c)
{
    return ___toupper(c);
}
