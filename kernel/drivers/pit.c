#include <kernel/drivers/pit.h>
#include <kernel/interrupts/handler.h>
#include <kernel/lib/stdio.h>

static uint32_t s_pit_ticks = 0;

bool pit_handler(struct itr_registers *registers)
{
    s_pit_ticks++;

    pic_send_eoi(registers->number);
    return ITR_CONTINUE;
}

void pit_init()
{
    itr_set_handler(32, pit_handler);

    printf("PIT: Initialized\n");
}
