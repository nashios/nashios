#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function)
{
    fprintf(stderr, "Assertion: Message = %s\n", assertion);
    fprintf(stderr, "Assertion: File = %s\n", file);
    fprintf(stderr, "Assertion: Line = %d\n", line);
    fprintf(stderr, "Assertion: Function = %s\n", function);

    abort();
}
