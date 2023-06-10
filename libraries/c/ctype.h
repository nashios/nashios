#ifndef _CTYPE_H
#define _CTYPE_H

#define _U 01
#define _L 02
#define _N 04
#define _S 010
#define _P 020
#define _C 040
#define _X 0100
#define _B 0200

#if __cplusplus
extern "C"
{
#endif

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

    static inline int __isalnum(int c) { return _ctype_[(unsigned char)(c) + 1] & (_U | _L | _N); }

    static inline int __isalpha(int c) { return _ctype_[(unsigned char)(c) + 1] & (_U | _L); }

    static inline int __isascii(int c) { return (unsigned)c <= 127; }

    static inline int __iscntrl(int c) { return _ctype_[(unsigned char)(c) + 1] & (_C); }

    static inline int __isdigit(int c) { return _ctype_[(unsigned char)(c) + 1] & (_N); }

    static inline int __isxdigit(int c) { return _ctype_[(unsigned char)(c) + 1] & (_N | _X); }

    static inline int __isspace(int c) { return _ctype_[(unsigned char)(c) + 1] & (_S); }

    static inline int __ispunct(int c) { return _ctype_[(unsigned char)(c) + 1] & (_P); }

    static inline int __isprint(int c) { return _ctype_[(unsigned char)(c) + 1] & (_P | _U | _L | _N | _B); }

    static inline int __isgraph(int c) { return _ctype_[(unsigned char)(c) + 1] & (_P | _U | _L | _N); }

    static inline int __islower(int c) { return _ctype_[(unsigned char)(c) + 1] & (_L); }

    static inline int __isupper(int c) { return _ctype_[(unsigned char)(c) + 1] & (_U); }

    static inline int __isblank(int c) { return _ctype_[(unsigned char)(c) + 1] & (_B) || (c == '\t'); }

    static inline int __toascii(int c) { return c & 127; }

    static inline int __tolower(int c)
    {
        if (c >= 'A' && c <= 'Z')
            return c | 0x20;
        return c;
    }

    static inline int __toupper(int c)
    {
        if (c >= 'a' && c <= 'z')
            return c & ~0x20;
        return c;
    }

#define isalnum __isalnum
#define isalpha __isalpha
#define isascii __isascii
#define iscntrl __iscntrl
#define isdigit __isdigit
#define isxdigit __isxdigit
#define isspace __isspace
#define ispunct __ispunct
#define isprint __isprint
#define isgraph __isgraph
#define islower __islower
#define isupper __isupper
#define isblank __isblank
#define toascii __toascii
#define tolower __tolower
#define toupper __toupper

#if __cplusplus
}
#endif

#endif
