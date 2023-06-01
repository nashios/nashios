#pragma once

#include <sys/cdefs.h>

#define __LC_CTYPE 0
#define __LC_NUMERIC 1
#define __LC_TIME 2
#define __LC_COLLATE 3
#define __LC_MONETARY 4
#define __LC_ALL 6

#define LC_CTYPE __LC_CTYPE
#define LC_NUMERIC __LC_NUMERIC
#define LC_TIME __LC_TIME
#define LC_COLLATE __LC_COLLATE
#define LC_MONETARY __LC_MONETARY
#define LC_ALL __LC_ALL

___BEGIN_DECLS

struct lconv
{
};

typedef struct
{
} locale_t;

char *setlocale(int category, const char *locale);
struct lconv *localeconv(void);

___END_DECLS
