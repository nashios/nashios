#include <Kernel/interrupts/handler.h>
#include <Kernel/lib/stdio.h>
#include <Kernel/lib/stdlib.h>
#include <st/dlist.h>

struct itr_handler
{
    itr_handler_t handler;
    struct dlist_head list;
};

static struct dlist_head s_itr_handlers[256];

void itr_set_handler(int number, itr_handler_t handler)
{
    struct itr_handler *itr = (struct itr_handler *)calloc(1, sizeof(struct itr_handler));
    itr->handler = handler;

    dlist_add_tail(&itr->list, &s_itr_handlers[number]);
}

void itr_unset_handler(int number, itr_handler_t handler)
{
    struct dlist_head *list = &s_itr_handlers[number];
    if (!dlist_empty(list))
        return;

    struct itr_handler *itr;
    dlist_for_each_entry(itr, list, list)
    {
        if (itr->handler == handler)
            break;
    }

    dlist_remove(&itr->list);
    free(itr);
}

void itr_handler(struct itr_registers *registers)
{
    int number = registers->number;
    struct dlist_head *list = &s_itr_handlers[number];
    if (dlist_empty(list))
    {
        printf("ITR: Unhandled interrupt number = %d\n", number);
        return;
    }

    struct itr_handler *itr;
    dlist_for_each_entry(itr, list, list)
    {
        if (itr->handler(registers) == ITR_STOP)
            break;
    }
}

void itr_init()
{
    for (int i = 0; i < 256; i++)
        dlist_head_init(&s_itr_handlers[i]);

    printf("ITR: Initialized\n");
}
