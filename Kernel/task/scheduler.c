#include <Kernel/arch/i686/task/tss.h>
#include <Kernel/cpu/processor.h>
#include <Kernel/filesystem/virtual.h>
#include <Kernel/interrupts/handler.h>
#include <Kernel/lib/stdio.h>
#include <Kernel/lib/stdlib.h>
#include <Kernel/lib/string.h>
#include <Kernel/memory/mmap.h>
#include <Kernel/panic.h>
#include <Kernel/task/elf.h>
#include <Kernel/task/scheduler.h>
#include <st/assert.h>

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

static struct plist_head s_scheduler_Kernel_list;
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
extern void scheduler_return_usermode(struct itr_registers *registers);

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

const char *scheduler_generate_process_name(const char *path)
{
    const char *found = strrstr(path, "/");
    if (!found)
        return path;

    size_t position = found - path;
    size_t length = strlen(path);
    char *name = calloc(length - position, sizeof(char));
    memcpy(name, path + position + 1, length - position - 1);

    return name;
}

struct process *scheduler_create_process(struct process *parent, const char *name)
{
    scheduler_lock();

    struct process *process = calloc(1, sizeof(struct process));
    process->pid = s_scheduler_pid++;
    process->name = scheduler_generate_process_name(name);
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

void scheduler_Kernel_thread_entry(struct thread *, void *flow())
{
    flow();
    scheduler_schedule();
}

void scheduler_create_elf_thread_stack(struct elf_layout *layout, int argc, char *const argv[], char *const envp[])
{
    assert((uint32_t)argv < KERNEL_HIGHER_HALF && (uint32_t)envp < KERNEL_HIGHER_HALF);

    layout->stack -= 4;
    *(uint32_t *)layout->stack = (uint32_t)envp;

    layout->stack -= 4;
    *(uint32_t *)layout->stack = (uint32_t)argv;

    layout->stack -= 4;
    *(uint32_t *)layout->stack = (uint32_t)argc;
}

void scheduler_elf_thread_entry(struct thread *thread, const char *path)
{
    scheduler_unlock();

    struct elf_layout *elf = elf_load(path);
    if (!elf)
        PANIC("Scheduler: Failed to load elf file = %s\n", path);

    thread->user_stack = elf->stack;
    tss_set_stack(thread->Kernel_stack);
    scheduler_create_elf_thread_stack(elf, 0, NULL, NULL);
    scheduler_enter_usermode(elf->stack, elf->entry, SCHED_PAGE_FAULT);
}

void scheduler_user_thread_entry(struct thread *thread)
{
    scheduler_unlock();
    tss_set_stack(thread->Kernel_stack);
    scheduler_return_usermode(&thread->registers);
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
    thread->Kernel_stack = (uint32_t)(calloc(SCHED_STACK_SIZE, sizeof(char)) + SCHED_STACK_SIZE);
    thread->esp = thread->Kernel_stack - sizeof(struct trap_frame);

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

    process->thread = thread;

    plist_node_init(&thread->plist, 1);

    scheduler_unlock();

    return thread;
}

struct plist_head *scheduler_get_list_from_thread(enum thread_state state, enum thread_type type)
{
    if (state == THREAD_READY_STATE)
    {
        if (type == THREAD_KERNEL_TYPE)
            return &s_scheduler_Kernel_list;
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
    tss_set_stack(g_scheduler_thread->Kernel_stack);
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
    struct thread *thread = scheduler_get_next_thread_from_list(&s_scheduler_Kernel_list);
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
    struct thread *thread = scheduler_pop_next_thread_from_list(&s_scheduler_Kernel_list);
    if (!thread)
        thread = scheduler_pop_next_thread_from_list(&s_scheduler_app_list);
    return thread;
}

void scheduler_wake_up(struct wait_queue *queue)
{
    struct wait_queue_entry *iter;
    struct wait_queue_entry *next;
    dlist_for_each_entry_safe(iter, next, &queue->list, list) { iter->handler(iter->thread); }
}

void scheduler_exit(int code)
{
    scheduler_lock();

    struct process *process = g_scheduler_process;
    process->exit_code = code;

    struct process_vm *iter;
    struct process_vm *next;
    dlist_for_each_entry_safe(iter, next, &process->memory->list, list)
    {
        if (!iter->file)
            virtual_mm_unmap_range(process->directory, iter->start, iter->end);

        dlist_remove(&iter->list);
        free(iter);
    }

    for (int i = 0; i < MAX_FD; i++)
    {
        struct vfs_file *file = process->files->fd[i];
        if (!file || !file->op->release)
            continue;

        file->op->release(file->dentry->inode, file);
        free(file);
        process->files->fd[i] = 0;
    }

    struct thread *thread = process->thread;
    scheduler_update_thread(thread, THREAD_TERMINATED_STATE);
    virtual_mm_unmap_range(process->directory, thread->user_stack - SCHED_STACK_SIZE, thread->user_stack);

    printf("Scheduler: Process pid = %d, exit code = %d\n", process->pid, code);

    scheduler_unlock();
    scheduler_schedule();
}

bool scheduler_handler(struct itr_registers *)
{
    if (g_scheduler_thread->type != THREAD_APPLICATION_TYPE || g_scheduler_thread->state != THREAD_RUNNING_STATE)
        return ITR_CONTINUE;

    scheduler_lock();

    bool should_schedule = false;
    g_scheduler_thread->timing++;
    if (g_scheduler_thread->timing >= 8)
    {
        struct thread *thread = scheduler_get_next_thread();
        if (thread)
        {
            if (thread->type == THREAD_APPLICATION_TYPE)
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

    return ITR_CONTINUE;
}

bool scheduler_fault_handler(struct itr_registers *registers)
{
    uint32_t address = 0;
    asm volatile("mov %%cr2, %0" : "=r"(address));

    if (registers->cs == 0x1B)
    {
        printf("Scheduler: Userspace page fault at address = 0x%x\n", address);
        scheduler_exit(registers->eax);
        return ITR_STOP;
    }

    assert_not_reached();
    return ITR_CONTINUE;
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
    struct process *process = scheduler_create_process(g_scheduler_process, path);
    struct thread *thread =
        scheduler_create_thread(process, (uint32_t)strdup(path), (uint32_t)scheduler_elf_thread_entry,
                                THREAD_APPLICATION_TYPE, THREAD_READY_STATE);
    scheduler_queue_thread(thread);
}

struct process_mm *scheduler_memory_clone(struct process *parent)
{
    struct process_mm *memory = calloc(1, sizeof(struct process_mm));
    memcpy(memory, parent->memory, sizeof(struct process_mm));
    dlist_head_init(&memory->list);

    struct process_vm *virtual = NULL;
    dlist_for_each_entry(virtual, &parent->memory->list, list)
    {
        struct process_vm *new_virtual = calloc(1, sizeof(struct process_vm));
        new_virtual->start = virtual->start;
        new_virtual->end = virtual->end;
        new_virtual->file = virtual->file;
        new_virtual->flags = virtual->flags;
        new_virtual->memory = memory;

        dlist_add_tail(&new_virtual->list, &memory->list);
    }

    return memory;
}

struct process *scheduler_fork(struct process *parent)
{
    scheduler_lock();

    struct process *process = calloc(1, sizeof(struct process));
    process->pid = s_scheduler_pid++;
    process->parent = parent;
    process->name = strdup(parent->name);
    process->memory = scheduler_memory_clone(parent);

    dlist_head_init(&process->children);
    dlist_add_tail(&process->sibling, &parent->children);

    process->fs = (struct process_fs *)calloc(1, sizeof(struct process_fs));
    memcpy(process->fs, parent->fs, sizeof(struct process_fs));

    process->files = (struct process_files *)calloc(1, sizeof(struct process_files));
    memcpy(process->files, parent->files, sizeof(struct process_files));

    process->directory = virtual_mm_fork(parent->directory);

    struct thread *thread = calloc(1, sizeof(struct thread));
    thread->tid = s_scheduler_tid++;
    thread->process = process;
    thread->type = THREAD_APPLICATION_TYPE;
    thread->state = THREAD_READY_STATE;
    thread->Kernel_stack = (uint32_t)(calloc(SCHED_STACK_SIZE, sizeof(char)) + SCHED_STACK_SIZE);
    thread->user_stack = parent->thread->user_stack;
    thread->esp = thread->Kernel_stack - sizeof(struct trap_frame);

    plist_node_init(&thread->plist, parent->thread->plist.priority);

    memcpy(&thread->registers, &parent->thread->registers, sizeof(struct itr_registers));
    thread->registers.eax = 0;

    struct trap_frame *frame = (struct trap_frame *)thread->esp;
    frame->param1 = (uint32_t)thread;
    frame->return_addr = SCHED_PAGE_FAULT;
    frame->eip = (uint32_t)scheduler_user_thread_entry;
    frame->eax = 0;
    frame->ecx = 0;
    frame->edx = 0;
    frame->ebx = 0;
    frame->esp = 0;
    frame->ebp = 0;
    frame->esi = 0;
    frame->edi = 0;

    process->thread = thread;

    scheduler_unlock();

    return process;
}

int scheduler_count_argv(char *const argv[])
{
    if (!argv)
        return 0;

    const int *p_argv = (const int *)argv;
    while (*p_argv)
        p_argv++;
    return p_argv - (int *)argv;
}

int scheduler_execve(const char *path, char *const argv[], char *const envp[])
{
    int argv_length = scheduler_count_argv(argv);
    char **Kernel_argv = calloc(argv_length, sizeof(char *));
    for (int i = 0; i < argv_length; ++i)
    {
        size_t length = strlen(argv[i]);
        Kernel_argv[i] = calloc(length + 1, sizeof(char));
        memcpy(Kernel_argv[i], argv[i], length);
    }

    int envp_length = scheduler_count_argv(envp);
    char **Kernel_envp = calloc(envp_length, sizeof(char *));
    for (int i = 0; i < envp_length; ++i)
    {
        size_t length = strlen(envp[i]);
        Kernel_envp[i] = calloc(length + 1, sizeof(char));
        memcpy(Kernel_envp[i], envp[i], length);
    }

    free((void *)g_scheduler_process->name);
    g_scheduler_process->name = scheduler_generate_process_name(path);

    char *p_path = strdup(path);
    elf_unload();

    struct elf_layout *elf = elf_load(p_path);
    free(p_path);

    char **user_argv = (char **)mmap_sbrk(argv_length + 1);
    memset(user_argv, 0, argv_length + 1);

    for (int i = 0; i < argv_length; ++i)
    {
        size_t length = strlen(Kernel_argv[i]);
        user_argv[i] = (char *)mmap_sbrk(length + 1);
        memcpy(user_argv[i], Kernel_argv[i], length);
    }

    char **user_envp = (char **)mmap_sbrk(envp_length + 1);
    memset(user_envp, 0, envp_length + 1);
    for (int i = 0; i < envp_length; ++i)
    {
        size_t length = strlen(Kernel_envp[i]);
        user_envp[i] = (char *)mmap_sbrk(length + 1);
        memcpy(user_envp[i], Kernel_envp[i], length);
    }

    scheduler_create_elf_thread_stack(elf, argv_length, user_argv, user_envp);
    tss_set_stack(g_scheduler_thread->Kernel_stack);
    scheduler_enter_usermode(elf->stack, elf->entry, SCHED_PAGE_FAULT);

    return 0;
}

void scheduler_init(void *init)
{
    plist_head_init(&s_scheduler_Kernel_list);
    plist_head_init(&s_scheduler_app_list);
    plist_head_init(&s_scheduler_waiting_list);
    plist_head_init(&s_scheduler_terminated_list);

    g_scheduler_process = scheduler_create_process(NULL, "swapper");
    g_scheduler_thread = scheduler_create_thread(g_scheduler_process, 0x00, (uint32_t)scheduler_Kernel_thread_entry,
                                                 THREAD_KERNEL_TYPE, THREAD_RUNNING_STATE);

    struct process *process = scheduler_create_process(g_scheduler_process, "init");
    struct thread *thread = scheduler_create_thread(process, (uint32_t)init, (uint32_t)scheduler_Kernel_thread_entry,
                                                    THREAD_KERNEL_TYPE, THREAD_WAITING_STATE);
    scheduler_update_thread(g_scheduler_thread, THREAD_TERMINATED_STATE);
    scheduler_update_thread(thread, THREAD_READY_STATE);

    itr_set_handler(32, scheduler_handler);
    itr_set_handler(14, scheduler_fault_handler);

    printf("Scheduler: Initialized\n");
}
