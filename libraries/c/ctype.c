#include <ctype.h>

int isalnum(int ch)
{
    return isalpha(ch) || isdigit(ch);
}

int isalpha(int ch)
{
    return isupper(ch) || islower(ch);
}

int isascii(int ch)
{
    return (unsigned)ch <= 0x7F;
}

int iscntrl(int ch)
{
    return 0 <= ch && ch < 32;
}

int isdigit(int ch)
{
    return ch >= '0' && ch <= '9';
}

int isgraph(int ch)
{
    return '!' <= ch && ch <= '~';
}

int islower(int ch)
{
    return ch >= 'a' && ch <= 'z';
}

int isprint(int ch)
{
    return ch >= ' ' && ch <= '~';
}

int ispunct(int ch)
{
    return isprint(ch) && ch != ' ' && !isalnum(ch);
}

int isspace(int ch)
{
    return ch == ' ' || (ch >= '\t' && ch <= '\r');
}

int isupper(int ch)
{
    return ch >= 'A' && ch <= 'Z';
}

int isxdigit(int ch)
{
    return isdigit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}

int toascii(int ch)
{
    return (unsigned)ch & 0x7F;
}

int tolower(int ch)
{
    return ch + 0x20 * ((ch >= 'A') && (ch <= 'Z'));
}

int toupper(int ch)
{
    return ch - 0x20 * ((ch >= 'a') && (ch <= 'z'));
}
