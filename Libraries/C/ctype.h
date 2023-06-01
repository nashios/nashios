#pragma once

#include <sys/cdefs.h>

___BEGIN_DECLS

#define _U 01
#define _L 02
#define _N 04
#define _S 010
#define _P 020
#define _C 040
#define _X 0100
#define _B 0200

extern const char _ctype_[256];

int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int iscntrl(int c);
int isdigit(int c);
int isxdigit(int c);
int isspace(int c);
int ispunct(int c);
int isprint(int c);
int isgraph(int c);
int islower(int c);
int isupper(int c);
int isblank(int c);
int toascii(int c);
int tolower(int c);
int toupper(int c);

static inline int ___isalnum(int c) { return _ctype_[(unsigned char)(c) + 1] & (_U | _L | _N); }

static inline int ___isalpha(int c) { return _ctype_[(unsigned char)(c) + 1] & (_U | _L); }

static inline int ___isascii(int c) { return (unsigned)c <= 127; }

static inline int ___iscntrl(int c) { return _ctype_[(unsigned char)(c) + 1] & (_C); }

static inline int ___isdigit(int c) { return _ctype_[(unsigned char)(c) + 1] & (_N); }

static inline int ___isxdigit(int c) { return _ctype_[(unsigned char)(c) + 1] & (_N | _X); }

static inline int ___isspace(int c) { return _ctype_[(unsigned char)(c) + 1] & (_S); }

static inline int ___ispunct(int c) { return _ctype_[(unsigned char)(c) + 1] & (_P); }

static inline int ___isprint(int c) { return _ctype_[(unsigned char)(c) + 1] & (_P | _U | _L | _N | _B); }

static inline int ___isgraph(int c) { return _ctype_[(unsigned char)(c) + 1] & (_P | _U | _L | _N); }

static inline int ___islower(int c) { return _ctype_[(unsigned char)(c) + 1] & (_L); }

static inline int ___isupper(int c) { return _ctype_[(unsigned char)(c) + 1] & (_U); }

static inline int ___isblank(int c) { return _ctype_[(unsigned char)(c) + 1] & (_B) || (c == '\t'); }

static inline int ___toascii(int c) { return c & 127; }

static inline int ___tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
        return c | 0x20;
    return c;
}

static inline int ___toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        return c & ~0x20;
    return c;
}

#define isalnum ___isalnum
#define isalpha ___isalpha
#define isascii ___isascii
#define iscntrl ___iscntrl
#define isdigit ___isdigit
#define isxdigit ___isxdigit
#define isspace ___isspace
#define ispunct ___ispunct
#define isprint ___isprint
#define isgraph ___isgraph
#define islower ___islower
#define isupper ___isupper
#define isblank ___isblank
#define toascii ___toascii
#define tolower ___tolower
#define toupper ___toupper

___END_DECLS
