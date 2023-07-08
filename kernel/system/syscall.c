#include <kernel/api/posix/sys/syscall.h>
#include <kernel/interrupts/handler.h>
#include <kernel/system/syscall.h>
#include <st/debug.h>

static void *s_syscall_list[] = {[__NR_exit] = syscall_exit,
                                 [__NR_fork] = syscall_fork,
                                 [__NR_read] = syscall_read,
                                 [__NR_write] = syscall_write,
                                 [__NR_open] = syscall_open,
                                 [__NR_close] = syscall_close,
                                 [__NR_execve] = syscall_execve,
                                 [__NR_getpid] = syscall_getpid,
                                 [__NR_brk] = syscall_brk,
                                 [__NR_ioctl] = syscall_ioctl,
                                 [__NR_fcntl] = syscall_fcntl,
                                 [__NR_mmap] = syscall_mmap,
                                 [__NR_ftruncate] = syscall_ftruncate,
                                 [__NR_stat] = syscall_stat,
                                 [__NR_fstat] = syscall_fstat,
                                 [__NR_poll] = syscall_poll,
                                 [__NR_mq_open] = syscall_mq_open,
                                 [__NR_mq_timedsend] = syscall_mq_timedsend,
                                 [__NR_mq_timedreceive] = syscall_mq_timedreceive};

bool syscall_handler(struct itr_registers *registers)
{
    uint32_t number = registers->eax;
    uint32_t size = sizeof(s_syscall_list) / sizeof(s_syscall_list[0]);
    if (number >= size || !s_syscall_list[number])
    {
        dbgln("Syscall: Called syscall number = %d without handler", number);
        registers->eax = -ENOSYS;
        return ITR_STOP;
    }

    memcpy(&g_scheduler_thread->registers, registers, sizeof(struct itr_registers));
    uint32_t (*handler)(uint32_t, ...) = s_syscall_list[number];
    uint32_t result = handler(registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi);
    registers->eax = result;
    dbgln("Syscall: Called syscall number = %d, result = %p", number, registers->eax);

    return ITR_CONTINUE;
}

void syscall_init()
{
    itr_set_handler(128, syscall_handler);
    dbgln("Syscall: Initialized");
}
