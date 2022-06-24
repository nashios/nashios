#include <kernel/api/posix/errno.h>
#include <kernel/api/posix/unistd.h>
#include <kernel/interrupts/isr.h>
#include <kernel/memory/mmap.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/system/syscall.h>
#include <kernel/task/scheduler.h>

int syscall_execve(const char *filename, char *const argv[], char *const envp[])
{
    return sched_execve(filename, argv, envp);
}

pid_t syscall_fork() { return sched_fork(); }

void syscall_exit(int status) { sched_exit(status); }

int syscall_brk(void *addr)
{
    uint32_t brk = (uint32_t)addr;
    struct mmap_mm *mm = sched_current_process()->mm;
    if (brk == 0)
        return mm->brk_middle;

    if (brk < mm->brk_start)
        return -ENOMEM;

    uint32_t result = mmap_brk(mm->brk_start, brk - mm->brk_start);
    if (result != 0)
        return result;

    return 0;
}

static void *syscall_list[] = {
    [__NR_exit] = syscall_exit, [__NR_fork] = syscall_fork, [__NR_execve] = syscall_execve, [__NR_brk] = syscall_brk};

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

    memcpy(&sched_current_thread()->registers, registers, sizeof(struct itr_registers));
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
