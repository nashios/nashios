#pragma once

#include <kernel/api/posix/sys/types.h>
#include <kernel/memory/virtual.h>
#include <kernel/plist.h>

enum thread_state
{
    THREAD_READY_STATE,
    THREAD_RUNNING_STATE,
    THREAD_WAITING_STATE,
    THREAD_TERMINATED_STATE
};

enum thread_type
{
    THREAD_KERNEL_TYPE,
    THREAD_APPLICATION_TYPE
};

struct process
{
    pid_t pid;
    struct process *parent;
    struct page_directory *directory;
    struct dlist_head sibling;
    struct dlist_head children;
};

struct thread
{
    pid_t tid;
    uint32_t timing;
    uint32_t esp;
    uint32_t kernel_stack;
    enum thread_type type;
    enum thread_state state;
    struct process *process;
    struct plist_node plist;
};

extern struct process *g_scheduler_process;
extern struct thread *g_scheduler_thread;

void scheduler_init(void *init);
void scheduler_schedule();
void scheduler_lock();
void scheduler_unlock();
void scheduler_update_thread(struct thread *thread, enum thread_state state);