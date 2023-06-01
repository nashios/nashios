#include <assert.h>
#include <stdlib.h>

extern "C"
{
    void ___assert(const char *, const char *, const char *, int) { abort(); }
}
