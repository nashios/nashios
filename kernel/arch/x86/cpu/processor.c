#include <kernel/cpu/processor.h>
#include <stdint.h>

static volatile int s_itr_depth = 0;

void ___enable_interrupts()
{
    if (s_itr_depth == 0 || s_itr_depth == 1)
        asm volatile("sti");
    else
        s_itr_depth--;
}

void ___disable_interrupts()
{
    uint32_t flags;
    asm volatile("pushf\n\t"
                 "pop %%eax\n\t"
                 "movl %%eax, %0\n\t"
                 : "=r"(flags)
                 :
                 : "%eax");

    asm volatile("cli");

    if (flags & (1 << 9))
        s_itr_depth = 1;
    else
        s_itr_depth++;
}

void ___pause() { asm volatile("hlt"); }
