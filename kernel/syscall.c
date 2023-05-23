#include <kernel/interrupts/isr.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/task/scheduler.h>

static void *s_syscall_list[] = {[0] = NULL};

bool syscall_handler(struct registers *registers)
{
    uint32_t number = registers->eax;
    uint32_t (*handler)(uint32_t, ...) = s_syscall_list[number];
    if (!handler)
    {
        printf("Syscall: Called syscall number = %d without handler\n", number);
        return ITR_STOP;
    }

    memcpy(&g_scheduler_thread->registers, registers, sizeof(struct registers));
    uint32_t result = handler(registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi);
    registers->eax = result;
    printf("Syscall: Called syscall number = %d, result = %d\n", number, result);

    return ITR_CONTINUE;
}

void syscall_init()
{
    isr_set_handler(127, syscall_handler);
    printf("Syscall: Initialized\n");
}
