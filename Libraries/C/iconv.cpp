#include <assert.h>
#include <iconv.h>

extern "C"
{
    size_t iconv(iconv_t, char **__restrict, size_t *__restrict, char **__restrict, size_t *__restrict)
    {
        assert(false);
    }

    int iconv_close(iconv_t) { assert(false); }

    iconv_t iconv_open(const char *, const char *) { assert(false); }
}
