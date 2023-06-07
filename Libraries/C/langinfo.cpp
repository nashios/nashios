#include <assert.h>
#include <langinfo.h>

extern "C"
{
    char *nl_langinfo(nl_item) { assert(false); }

    char *nl_langinfo_1(nl_item, locale_t) { assert(false); }
}
