#include <kernel/api/posix/sys/syscall.h>
#include <kernel/interrupts/isr.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/task/scheduler.h>

#define MAX_SYSCALL (__NR_set_mempolicy_home_node + 1)

void syscall_exit(int status) { scheduler_exit(status); }

pid_t syscall_fork(void)
{
    struct process *process = scheduler_fork(g_scheduler_process);
    scheduler_queue_thread(process->thread);
    return process->pid;
}

static void *s_syscall_list[MAX_SYSCALL] = {[__NR_exit] = syscall_exit, [__NR_fork] = syscall_fork};

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

    return ITR_STOP;
}

void syscall_init()
{
    isr_set_handler(0x80, syscall_handler);
    printf("Syscall: Initialized\n");
}
