#include <assert.h>
#include <stdlib.h>

void _assert(const char *message, const char *file, const char *function, int line) { abort(); }
