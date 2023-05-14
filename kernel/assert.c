#include <kernel/assert.h>
#include <kernel/panic.h>
#include <kernel/processor.h>
#include <kernel/stdio.h>

void assert(const char *message, const char *file, const char *function, int line)
{
    DISABLE_INTERRUPTS();

    printf("Assertion: Message = %s\n", message);
    printf("Assertion: File = %s\n", file);
    printf("Assertion: Function = %s\n", function);
    printf("Assertion: Line = %d\n", line);

    PANIC("%s\n", message);
}