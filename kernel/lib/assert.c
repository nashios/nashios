#include <kernel/cpu/processor.h>
#include <kernel/lib/assert.h>
#include <kernel/panic.h>
#include <st/debug.h>

void ___assert(const char *message, const char *file, const char *function, int line)
{
    DISABLE_INTERRUPTS();

    dbgln("Assertion: Message = %s", message);
    dbgln("Assertion: File = %s", file);
    dbgln("Assertion: Function = %s", function);
    dbgln("Assertion: Line = %d", line);

    PANIC("Assertion failed");
}
