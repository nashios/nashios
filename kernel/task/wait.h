#pragma once

#include <kernel/filesystem/virtual.h>

#define WAIT_INIT(name)                                                                                                \
    struct wait_queue_entry name = {                                                                                   \
        .thread = g_scheduler_thread,                                                                                  \
        .handler = virtual_fs_poll_wakeup,                                                                             \
    }

#define WAIT_EVENT(waiter, condition)                                                                                  \
    ({                                                                                                                 \
        WAIT_INIT(__wait);                                                                                             \
        dlist_add_tail(&__wait.list, &(waiter)->list);                                                                 \
        WAIT(condition);                                                                                               \
        dlist_remove(&__wait.list);                                                                                    \
    })
