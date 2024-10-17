#include <kernel/arch/lib/stdio.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/string.h>

int printf(const char *format, ...)
{
    arch_printf(format, strlen(format));

    return 0;
}