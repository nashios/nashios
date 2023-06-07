#include <Kernel/interrupts/idt.h>
#include <Kernel/interrupts/isr.h>
#include <Kernel/panic.h>
#include <Kernel/processor.h>
#include <Kernel/stdio.h>

#define ISR_CHAIN_SIZE 33
#define ISR_CHAIN_DEPTH 4

static itr_handler_t s_isr_handlers[ISR_CHAIN_SIZE * ISR_CHAIN_DEPTH] = {};

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

void isr_set_handler(uint8_t index, itr_handler_t handler)
{
    for (uint8_t i = 0; i < ISR_CHAIN_DEPTH; i++)
    {
        if (s_isr_handlers[i * ISR_CHAIN_SIZE + index])
            continue;

        s_isr_handlers[i * ISR_CHAIN_SIZE + index] = handler;
        break;
    }
}

void isr_unset_handler(uint8_t index)
{
    for (uint8_t i = 0; i < ISR_CHAIN_DEPTH; i++)
        s_isr_handlers[i * ISR_CHAIN_SIZE + index] = NULL;
}

void isr_init() { printf("ISR: Initialized\n"); }

void isr_handler(struct registers *registers)
{
    for (size_t i = 0; i < ISR_CHAIN_DEPTH; i++)
    {
        itr_handler_t handler = s_isr_handlers[i * ISR_CHAIN_SIZE + registers->number];
        if (!handler)
            break;

        if (handler(registers) == ITR_STOP)
            return;
    }

    printf("ISR: Unhandled exception number = %d, message = %s\n", registers->number,
           s_isr_messages[registers->number]);

    printf("ISR: * EAX = 0x%p, EBX = 0x%p\n", registers->eax, registers->ebx);
    printf("ISR: * ECX = 0x%p, EDX = 0x%p\n", registers->ecx, registers->edx);
    printf("ISR: * ESI = 0x%p, EDI = 0x%p\n", registers->esi, registers->edi);
    printf("ISR: * EBP = 0x%p, ESP = 0x%p\n", registers->ebp, registers->esp);
    printf("ISR: * EIP = 0x%p, EFL = 0x%p\n", registers->eip, registers->eflags);
    printf("ISR: * ES = 0x%p, CS = 0x%p\n", registers->es, registers->cs);
    printf("ISR: * SS = 0x%p, DS = 0x%p\n", registers->ss, registers->ds);
    printf("ISR: * FS = 0x%p, GS = 0x%p\n", registers->fs, registers->gs);

    PANIC("Unhandled exception number = %d\n", registers->number);
}
