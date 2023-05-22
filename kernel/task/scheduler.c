#include <kernel/interrupts/irq.h>
#include <kernel/panic.h>
#include <kernel/processor.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>
#include <kernel/task/elf.h>
#include <kernel/task/scheduler.h>
#include <kernel/tss.h>

#define SCHED_PAGE_FAULT 0xFFFFFFFF

struct trap_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t return_addr;
    uint32_t param1;
    uint32_t param2;
};

static struct plist_head s_scheduler_kernel_list;
static struct plist_head s_scheduler_app_list;
static struct plist_head s_scheduler_waiting_list;
static struct plist_head s_scheduler_terminated_list;
static uint32_t s_scheduler_locks = 0;
static uint32_t s_scheduler_pid = 0;
static uint32_t s_scheduler_tid = 0;
struct process *g_scheduler_process = NULL;
struct thread *g_scheduler_thread = NULL;

extern void scheduler_switch(uint32_t *old_esp, uint32_t new_esp, uint32_t cr3);
extern void scheduler_enter_usermode(uint32_t eip, uint32_t esp, uint32_t failed);

void scheduler_lock()
{
    DISABLE_INTERRUPTS();
    s_scheduler_locks++;
}

void scheduler_unlock()
{
    s_scheduler_locks--;
    if (s_scheduler_locks == 0)
        ENABLE_INTERRUPTS();
}

struct process *scheduler_create_process(struct process *parent)
{
    scheduler_lock();

    struct process *process = calloc(1, sizeof(struct process));
    process->pid = s_scheduler_pid++;
    process->parent = parent;
    process->directory = g_virtual_directory;
    process->fs = (struct process_fs *)calloc(1, sizeof(struct process_fs));
    process->files = (struct process_files *)calloc(1, sizeof(struct process_files));
    process->memory = (struct process_mm *)calloc(1, sizeof(struct process_mm));

    if (process->parent)
    {
        process->directory = virtual_mm_create_address();
        memcpy(process->fs, parent->fs, sizeof(struct process_fs));
        memcpy(process->files, parent->files, sizeof(struct process_files));
        dlist_add_tail(&process->sibling, &parent->children);
    }

    dlist_head_init(&process->memory->list);
    dlist_head_init(&process->children);

    scheduler_unlock();

    return process;
}

void scheduler_kernel_thread_entry(struct thread *, void *flow())
{
    flow();
    scheduler_schedule();
}

void scheduler_elf_thread_entry(struct thread *thread, const char *path)
{
    scheduler_unlock();

    struct elf_layout *elf = elf_load(path);
    if (!elf)
        PANIC("Scheduler: Failed to load elf file = %s\n", path);

    thread->user_stack = elf->stack;
    tss_set_stack(0x10, thread->kernel_stack);
    scheduler_enter_usermode(elf->stack, elf->entry, SCHED_PAGE_FAULT);
}

struct thread *scheduler_create_thread(struct process *process, uint32_t param, uint32_t eip, enum thread_type type,
                                       enum thread_state state)
{
    scheduler_lock();

    struct thread *thread = calloc(1, sizeof(struct thread));
    thread->tid = s_scheduler_tid++;
    thread->process = process;
    thread->type = type;
    thread->state = state;
    thread->kernel_stack = (uint32_t)(calloc(SCHED_STACK_SIZE, sizeof(char)) + SCHED_STACK_SIZE);
    thread->esp = thread->kernel_stack - sizeof(struct trap_frame);

    struct trap_frame *frame = (struct trap_frame *)thread->esp;
    memset(frame, 0x00, sizeof(struct trap_frame));

    frame->param1 = (uint32_t)thread;
    frame->param2 = param;
    frame->return_addr = SCHED_PAGE_FAULT;
    frame->eip = eip;
    frame->eax = 0;
    frame->ecx = 0;
    frame->edx = 0;
    frame->ebx = 0;
    frame->esp = 0;
    frame->ebp = 0;
    frame->esi = 0;
    frame->edi = 0;

    plist_node_init(&thread->plist, 1);

    scheduler_unlock();

    return thread;
}

struct plist_head *scheduler_get_list_from_thread(enum thread_state state, enum thread_type type)
{
    if (state == THREAD_READY_STATE)
    {
        if (type == THREAD_KERNEL_TYPE)
            return &s_scheduler_kernel_list;
        else
            return &s_scheduler_app_list;
    }
    else if (state == THREAD_WAITING_STATE)
        return &s_scheduler_waiting_list;
    else if (state == THREAD_TERMINATED_STATE)
        return &s_scheduler_terminated_list;
    return NULL;
}

void scheduler_queue_thread(struct thread *thread)
{
    struct plist_head *head = scheduler_get_list_from_thread(thread->state, thread->type);
    if (!head)
        return;

    plist_add(&thread->plist, head);
}

void scheduler_remove_thread(struct thread *thread)
{
    struct plist_head *head = scheduler_get_list_from_thread(thread->state, thread->type);
    if (!head)
        return;

    plist_remove(&thread->plist, head);
}

void scheduler_update_thread(struct thread *thread, enum thread_state state)
{
    if (thread->state == state)
        return;

    scheduler_lock();
    scheduler_remove_thread(thread);
    thread->state = state;
    scheduler_queue_thread(thread);
    scheduler_unlock();
}

void scheduler_switch_thread(struct thread *thread)
{
    if (g_scheduler_thread == thread)
    {
        g_scheduler_thread->timing = 0;
        scheduler_update_thread(g_scheduler_thread, THREAD_RUNNING_STATE);
        return;
    }

    struct thread *p_thread = g_scheduler_thread;
    g_scheduler_thread = thread;
    g_scheduler_thread->timing = 0;
    scheduler_update_thread(g_scheduler_thread, THREAD_RUNNING_STATE);

    g_scheduler_process = g_scheduler_thread->process;

    uint32_t physical_address = virtual_mm_get_physical((uint32_t)g_scheduler_thread->process->directory);
    tss_set_stack(0x10, g_scheduler_thread->kernel_stack);
    scheduler_switch(&p_thread->esp, g_scheduler_thread->esp, physical_address);
}

struct thread *scheduler_get_next_thread_from_list(struct plist_head *list)
{
    if (plist_head_empty(list))
        return NULL;

    return plist_first_entry(list, struct thread, plist);
}

struct thread *scheduler_get_next_thread()
{
    struct thread *thread = scheduler_get_next_thread_from_list(&s_scheduler_kernel_list);
    if (!thread)
        thread = scheduler_get_next_thread_from_list(&s_scheduler_app_list);
    return thread;
}

struct thread *scheduler_pop_next_thread_from_list(struct plist_head *list)
{
    if (plist_head_empty(list))
        return NULL;

    struct thread *thread = plist_first_entry(list, struct thread, plist);
    plist_remove(&thread->plist, list);
    return thread;
}

struct thread *scheduler_pop_next_thread()
{
    struct thread *thread = scheduler_pop_next_thread_from_list(&s_scheduler_kernel_list);
    if (!thread)
        thread = scheduler_pop_next_thread_from_list(&s_scheduler_app_list);
    return thread;
}

bool scheduler_handler(struct registers *)
{
    if (g_scheduler_thread->type != THREAD_KERNEL_TYPE || g_scheduler_thread->state != THREAD_RUNNING_STATE)
        return ITR_CONTINUE;

    scheduler_lock();

    bool should_schedule = false;
    g_scheduler_thread->timing++;
    if (g_scheduler_thread->timing >= 8)
    {
        struct thread *thread = scheduler_get_next_thread();
        if (thread)
        {
            if (thread->type == THREAD_KERNEL_TYPE)
            {
                struct thread *first_thread = plist_first_entry(&s_scheduler_app_list, struct thread, plist);
                struct thread *last_thread = plist_last_entry(&s_scheduler_app_list, struct thread, plist);
                struct thread *iter;
                plist_for_each_entry(iter, &s_scheduler_app_list, plist)
                {
                    iter->plist.priority -= first_thread->plist.priority;
                }
                g_scheduler_thread->plist.priority = last_thread->plist.priority + 1;
            }

            scheduler_update_thread(g_scheduler_thread, THREAD_READY_STATE);
            should_schedule = true;
        }
    }

    scheduler_unlock();

    if (should_schedule && !s_scheduler_locks)
    {
        printf("Scheduler: Round-robin thread = %d\n", g_scheduler_thread->tid);
        scheduler_schedule();
    }

    pic_send_eoi(0);
    return true;
}

void scheduler_schedule()
{
    if (g_scheduler_thread->state == THREAD_RUNNING_STATE)
        return;

    scheduler_lock();
    struct thread *thread = scheduler_pop_next_thread();
    if (!thread)
    {
        do
        {
            scheduler_unlock();
            PAUSE();
            scheduler_lock();
            thread = scheduler_pop_next_thread();
            if (!thread && g_scheduler_thread->state == THREAD_RUNNING_STATE)
                thread = g_scheduler_thread;
        } while (!thread);
    }

    scheduler_switch_thread(thread);
    scheduler_unlock();
}

void scheduler_open(const char *path)
{
    struct process *process = scheduler_create_process(g_scheduler_process);
    struct thread *thread =
        scheduler_create_thread(process, (uint32_t)strdup(path), (uint32_t)scheduler_elf_thread_entry,
                                THREAD_APPLICATION_TYPE, THREAD_READY_STATE);
    scheduler_queue_thread(thread);
}

void scheduler_init(void *init)
{
    plist_head_init(&s_scheduler_kernel_list);
    plist_head_init(&s_scheduler_app_list);
    plist_head_init(&s_scheduler_waiting_list);
    plist_head_init(&s_scheduler_terminated_list);

    g_scheduler_process = scheduler_create_process(NULL);
    g_scheduler_thread = scheduler_create_thread(g_scheduler_process, 0x00, (uint32_t)scheduler_kernel_thread_entry,
                                                 THREAD_KERNEL_TYPE, THREAD_RUNNING_STATE);

    struct process *process = scheduler_create_process(g_scheduler_process);
    struct thread *thread = scheduler_create_thread(process, (uint32_t)init, (uint32_t)scheduler_kernel_thread_entry,
                                                    THREAD_KERNEL_TYPE, THREAD_WAITING_STATE);
    scheduler_update_thread(g_scheduler_thread, THREAD_TERMINATED_STATE);
    scheduler_update_thread(thread, THREAD_READY_STATE);

    irq_set_handler(0, scheduler_handler);

    printf("Scheduler: Initialized\n");
}