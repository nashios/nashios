#include <Kernel/cpu/processor.h>
#include <Kernel/lib/assert.h>
#include <Kernel/lib/stdio.h>
#include <Kernel/panic.h>

void ___assert(const char *message, const char *file, const char *function, int line)
{
    DISABLE_INTERRUPTS();

    printf("Assertion: Message = %s\n", message);
    printf("Assertion: File = %s\n", file);
    printf("Assertion: Function = %s\n", function);
    printf("Assertion: Line = %d\n", line);

    PANIC("%s\n", message);
}
