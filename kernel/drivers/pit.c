#include <kernel/drivers/pit.h>
#include <kernel/interrupts/handler.h>
#include <kernel/lib/stdio.h>

static uint32_t s_pit_ticks = 0;

bool pit_handler(struct itr_registers *)
{
    s_pit_ticks++;
    return ITR_CONTINUE;
}

void pit_init()
{
    itr_set_handler(32, pit_handler);

    printf("PIT: Initialized\n");
}
