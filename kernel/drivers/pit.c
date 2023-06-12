#include <kernel/arch/i686/cpu/io.h>
#include <kernel/drivers/pit.h>
#include <kernel/interrupts/handler.h>
#include <kernel/lib/stdio.h>

#define PIT_FREQUENCY 1193181
#define PIT_TICKS 1000

static uint32_t s_pit_ticks = 0;

bool pit_handler(struct itr_registers *)
{
    s_pit_ticks++;
    return ITR_CONTINUE;
}

void pit_init()
{
    int divisor = PIT_FREQUENCY / PIT_TICKS;
    io_outb(0x43, 0x34);
    io_outb(0x40, divisor & 0xff);
    io_outb(0x40, (divisor >> 8) & 0xff);

    itr_set_handler(32, pit_handler);

    printf("PIT: Initialized\n");
}
