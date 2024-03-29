#include <kernel/arch/x86/cpu/io.h>
#include <kernel/cpu/processor.h>
#include <kernel/lib/string.h>
#include <kernel/panic.h>
#include <st/debug.h>

__attribute__((noreturn)) void panic_shutdown()
{
    io_outw(0xB004, 0x2000);
    io_outw(0x604, 0x2000);

    io_outw(0x4004, 0x3400);
    STOP();
}

void panic_backtrace_handler(uint32_t base)
{
    dbgln("Panic: Backtrace base = 0x%x", base);

    uint32_t stack_buffer[2];
    uint32_t *stack_ptr = (uint32_t *)base;
    while (stack_ptr && memcpy(stack_buffer, stack_ptr, sizeof(stack_buffer)))
    {
        uint32_t result_address = stack_buffer[1];
        dbgln("Panic: * Address = 0x%p, next = 0x%p", result_address, stack_ptr ? (uint32_t *)stack_buffer[0] : 0);
        stack_ptr = (uint32_t *)stack_buffer[0];
    }
    return;
}

void panic_backtrace()
{
    static bool s_panic_backtrace = false;
    if (s_panic_backtrace)
        return;

    s_panic_backtrace = true;

    uint32_t base = (uint32_t)__builtin_frame_address(0);
    panic_backtrace_handler(base);

    s_panic_backtrace = false;
}

void panic(const char *file, int line, const char *function)
{
    dbgln("Panic: File = %s", file);
    dbgln("Panic: Line = %d", line);
    dbgln("Panic: Function = %s", function);

    panic_backtrace();
    panic_shutdown();
}
