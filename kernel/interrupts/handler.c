#include <kernel/interrupts/handler.h>
#include <kernel/lib/stdio.h>
#include <kernel/lib/stdlib.h>
#include <kernel/panic.h>
#include <st/dlist.h>

struct itr_handler
{
    itr_handler_t handler;
    struct dlist_head list;
};

static struct dlist_head s_itr_handlers[256];
static const char *s_isr_messages[32] = {"Division Error",
                                         "Debug",
                                         "Non-maskable Interrupt",
                                         "Breakpoint",
                                         "Overflow",
                                         "Bound Ranged Exceeded",
                                         "Invalid Opcode",
                                         "Device Not Available",
                                         "Double fault",
                                         "Coprocessor Segment Overrun",
                                         "Invalid TSS",
                                         "Segment Not Present",
                                         "Stack-Segment Fault",
                                         "General Protection Fault",
                                         "Page Fault",
                                         "Reserved",
                                         "x87 Floating-Point Exception",
                                         "Alignment Check",
                                         "Machine Check",
                                         "SIMD Floating-Point Exception",
                                         "Virtualization Exception",
                                         "Control Protection Exception",
                                         "Reserved",
                                         "Reserved",
                                         "Reserved",
                                         "Reserved",
                                         "Reserved",
                                         "Reserved",
                                         "Hypervisor Injection Exception",
                                         "VMM Communication Exception",
                                         "Security Exception",
                                         "Reserved"};

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
    int number = registers->number & 0xFF;
    struct dlist_head *list = &s_itr_handlers[number];
    if (dlist_empty(list))
    {
        printf("ITR: Unhandled interrupt number = %d\n", number);
        return;
    }

    struct itr_handler *itr;
    dlist_for_each_entry_reverse(itr, list, list)
    {
        if (itr->handler(registers) == ITR_STOP)
            break;
    }
}

bool isr_handler(struct itr_registers *registers)
{
    printf("ISR: Exception number = %d, message = %s\n", registers->number, s_isr_messages[registers->number]);
    PANIC("Exception");
    return ITR_STOP;
}

void itr_init()
{
    for (int i = 0; i < 256; i++)
        dlist_head_init(&s_itr_handlers[i]);

    itr_set_handler(0, isr_handler);
    itr_set_handler(1, isr_handler);
    itr_set_handler(2, isr_handler);
    itr_set_handler(3, isr_handler);
    itr_set_handler(4, isr_handler);
    itr_set_handler(5, isr_handler);
    itr_set_handler(6, isr_handler);
    itr_set_handler(7, isr_handler);
    itr_set_handler(8, isr_handler);
    itr_set_handler(9, isr_handler);
    itr_set_handler(10, isr_handler);
    itr_set_handler(11, isr_handler);
    itr_set_handler(12, isr_handler);
    itr_set_handler(13, isr_handler);
    itr_set_handler(14, isr_handler);
    itr_set_handler(16, isr_handler);
    itr_set_handler(17, isr_handler);
    itr_set_handler(18, isr_handler);
    itr_set_handler(19, isr_handler);
    itr_set_handler(20, isr_handler);
    itr_set_handler(21, isr_handler);
    itr_set_handler(28, isr_handler);
    itr_set_handler(29, isr_handler);
    itr_set_handler(30, isr_handler);

    printf("ITR: Initialized\n");
}
