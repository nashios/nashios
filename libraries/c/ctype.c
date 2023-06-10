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
int isalnum(int c) { return __isalnum(c); }

#undef isalpha
int isalpha(int c) { return __isalpha(c); }

#undef iscntrl
int iscntrl(int c) { return __iscntrl(c); }

#undef isdigit
int isdigit(int c) { return __isdigit(c); }

#undef isxdigit
int isxdigit(int c) { return __isxdigit(c); }

#undef isspace
int isspace(int c) { return __isspace(c); }

#undef ispunct
int ispunct(int c) { return __ispunct(c); }

#undef isprint
int isprint(int c) { return __isprint(c); }

#undef isgraph
int isgraph(int c) { return __isgraph(c); }

#undef isupper
int isupper(int c) { return __isupper(c); }

#undef islower
int islower(int c) { return __islower(c); }

#undef isascii
int isascii(int c) { return __isascii(c); }

#undef isblank
int isblank(int c) { return __isblank(c); }

#undef toascii
int toascii(int c) { return __toascii(c); }

#undef tolower
int tolower(int c) { return __tolower(c); }

#undef toupper
int toupper(int c) { return __toupper(c); }
