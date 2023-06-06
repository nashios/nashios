#include <assert.h>
#include <locale.h>

extern "C"
{
    char *setlocale(int category, const char *locale) { assert(false); }

    struct lconv *localeconv(void) { assert(false); }

    locale_t newlocale(int category_mask, const char *locale, locale_t base) { assert(false); }

    void freelocale(locale_t locobj) { assert(false); }

    locale_t uselocale(locale_t locobj) { assert(false); }

    locale_t duplocale(locale_t locobj) { assert(false); }
}
