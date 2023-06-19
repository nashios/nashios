#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/isr.h>
#include <kernel/panic.h>
#include <kernel/processor.h>
#include <kernel/stdio.h>

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

void isr_init() { dbgln("ISR: Initialized"); }

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

    dbgln("ISR: Unhandled exception number = %d, message = %s", registers->number,
           s_isr_messages[registers->number]);

    dbgln("ISR: * EAX = 0x%p, EBX = 0x%p", registers->eax, registers->ebx);
    dbgln("ISR: * ECX = 0x%p, EDX = 0x%p", registers->ecx, registers->edx);
    dbgln("ISR: * ESI = 0x%p, EDI = 0x%p", registers->esi, registers->edi);
    dbgln("ISR: * EBP = 0x%p, ESP = 0x%p", registers->ebp, registers->esp);
    dbgln("ISR: * EIP = 0x%p, EFL = 0x%p", registers->eip, registers->eflags);
    dbgln("ISR: * ES = 0x%p, CS = 0x%p", registers->es, registers->cs);
    dbgln("ISR: * SS = 0x%p, DS = 0x%p", registers->ss, registers->ds);
    dbgln("ISR: * FS = 0x%p, GS = 0x%p", registers->fs, registers->gs);

    PANIC("Unhandled exception number = %d", registers->number);
}
