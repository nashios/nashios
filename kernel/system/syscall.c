#include <kernel/system/syscall.h>
#include <kernel/api/posix/unistd.h>
#include <kernel/api/posix/errno.h>
#include <kernel/interrupts/isr.h>
#include <kernel/task/scheduler.h>
#include <kernel/stdio.h>

void syscall_exit(int status)
{
    sched_exit(status);
}

static void *syscall_list[] = {
    [__NR_exit] = syscall_exit};

static size_t syscall_size = sizeof(syscall_list) / sizeof(syscall_list[0]);

void syscall_handler(struct itr_registers *registers)
{
    uint32_t number = registers->eax;
    if (number >= syscall_size)
    {
        printf("Syscall: Invalid syscall number = %d\n", number);
        goto failed;
    }

    uint32_t (*handler)(unsigned int, ...) = syscall_list[number];
    if (!handler)
    {
        printf("Syscall: Invalid syscall handler, number = %d\n", number);
        goto failed;
    }

    registers->eax = handler(registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi);
    printf("Syscall: Called syscall number = %d, result = 0x%x\n", number, registers->eax);

    return;

failed:
    registers->eax = -ENOSYS;
}

void syscall_init()
{
    isr_add_handler(127, syscall_handler);

    printf("Syscall: Initialized\n");
}
