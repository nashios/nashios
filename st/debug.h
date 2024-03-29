#pragma once

#if defined(KERNEL)
#include <kernel/drivers/pit.h>
#include <kernel/lib/stdio.h>
#include <kernel/task/scheduler.h>
#else
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#if defined(KERNEL)
extern void early_printf(const char *buffer, size_t length);
#endif

static inline void dbgln(const char *format, ...)
{
#if defined(KERNEL)
    char sn_buffer[1024] = {};
    if (g_scheduler_process && g_scheduler_thread)
        sprintf(sn_buffer, "[%d.%d] [%s(%d:%d)] %s\n", g_pit_ticks, g_pit_subticks, g_scheduler_process->name,
                g_scheduler_process->pid, g_scheduler_thread->tid, format);
    else
        sprintf(sn_buffer, "[%d.%d] [Kernel] %s\n", g_pit_ticks, g_pit_subticks, format);

    char buffer[1024] = {};
    va_list args;
    va_start(args, format);
    size_t length = vsnprintf(buffer, sizeof(buffer), sn_buffer, args);
    va_end(args);

    early_printf(buffer, length);
#else
    static int fd = -1;
    if (fd == -1)
    {
        fd = open("/dev/debug", O_WRONLY);
        if (fd == -1)
            return;
    }

    va_list args1;
    va_start(args1, format);

    va_list args2;
    va_copy(args2, args1);

    int length = vsnprintf(NULL, 0, format, args1);
    va_end(args1);

    if (length < 0)
        return;

    char *buffer = (char *)calloc(length + 1, sizeof(char));
    if (buffer == NULL)
        return;

    length = vsnprintf(buffer, length + 1, format, args2);
    if (length < 0)
        return;
    va_end(args2);

    if (write(fd, buffer, length) < 0)
        return;

    free(buffer);

#endif
}

#define dbgln_if(condition, format, ...)                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((condition))                                                                                               \
            dbgln(format, ##__VA_ARGS__);                                                                              \
    } while (0);
