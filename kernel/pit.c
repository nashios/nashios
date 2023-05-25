#include <kernel/interrupts/irq.h>
#include <kernel/pit.h>
#include <kernel/stdio.h>

static uint32_t s_pit_ticks = 0;

bool pit_handler(struct registers *registers)
{
    s_pit_ticks++;

    pic_send_eoi(registers->number);
    return ITR_CONTINUE;
}

void pit_init()
{
    irq_set_handler(0, pit_handler);

    printf("PIT: Initialized\n");
}
