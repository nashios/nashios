#include <Kernel/drivers/pit.h>
#include <Kernel/interrupts/handler.h>
#include <Kernel/lib/stdio.h>

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
