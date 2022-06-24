/**
 * @file kernel/task/scheduler.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Process scheduling
 * @version 0.1
 * @date 2022-06-01
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/interrupts/irq.h>
#include <kernel/stdio.h>
#include <kernel/stdlib.h>
#include <kernel/string.h>
#include <kernel/system/sys.h>
#include <kernel/task/elf.h>
#include <kernel/task/scheduler.h>
#include <kernel/task/tss.h>

static pid_t sched_next_pid = 0;
static struct process *sched_process = NULL;
static struct thread *sched_thread = NULL;
static uint32_t sched_locks = 0;
static struct dlist_head sched_ready_list = {};
static struct dlist_head sched_wait_list = {};
static struct dlist_head sched_exit_list = {};

extern void sched_switch(uint32_t *old_esp, uint32_t new_esp, uint32_t physical);
extern void sched_enter_user(uint32_t eip, uint32_t esp, uint32_t failed_addr);

struct thread *sched_current_thread() { return sched_thread; }

struct process *sched_current_process() { return sched_process; }

void sched_lock()
{
    sys_cli();
    sched_locks++;
}

void sched_unlock()
{
    sched_locks--;
    if (sched_locks == 0)
        sys_sti();
}

struct dlist_head *sched_get_thread_list(enum thread_state state)
{
    if (state == THREAD_READY)
        return &sched_ready_list;
    else if (state == THREAD_WAIT)
        return &sched_wait_list;
    else if (state == THREAD_EXIT)
        return &sched_exit_list;
    return NULL;
}

void sched_add_thread(struct thread *thread)
{
    struct dlist_head *list = sched_get_thread_list(thread->state);
    if (!list)
        return;

    dlist_add_tail(&thread->list, list);
}

void sched_remove_thread(struct thread *thread)
{
    struct dlist_head *list = sched_get_thread_list(thread->state);
    if (!list)
        return;

    dlist_remove_init(&thread->list);
}

void sched_update_thread(struct thread *thread, enum thread_state state)
{
    if (thread->state == state)
        return;

    sched_lock();

    sched_remove_thread(thread);
    thread->state = state;
    sched_add_thread(thread);

    sched_unlock();
}

void sched_kernel_thread_entry(void *kernel_init())
{
    sched_unlock();
    kernel_init();
    sched_schedule();
}

void sched_elf_init_stack(struct elf_layout *layout, int argc, char *const argv[], char *const envp[])
{
    layout->stack -= 4;
    *(uint32_t *)layout->stack = (uint32_t)envp;

    layout->stack -= 4;
    *(uint32_t *)layout->stack = (uint32_t)argv;

    layout->stack -= 4;
    *(uint32_t *)layout->stack = (uint32_t)argc;
}

void sched_elf_thread_entry(struct thread *thread, const char *path)
{
    sched_unlock();

    struct elf_layout *elf = elf_open(path);
    if (!elf)
    {
        printf("Scheduler: Failed to load elf file path = %s\n", path);
        sched_remove_thread(thread);
        return;
    }
    sched_elf_init_stack(elf, 0, NULL, NULL);

    tss_set_stack(thread->kernel_stack);

    printf("Scheduler: Enter usermode path = %s, stack = 0x%x, entry = 0x%x\n", path, elf->stack, elf->entry);
    sched_enter_user(elf->stack, elf->entry, SCHED_PAGE_FAULT);
}

struct thread *sched_create_thread(struct process *process)
{
    struct thread *thread = calloc(1, sizeof(struct thread));
    thread->process = process;
    thread->state = THREAD_NEW;
    thread->kernel_stack = (uint32_t)calloc(SCHED_STACK, sizeof(char)) + SCHED_STACK;
    thread->esp = thread->kernel_stack - sizeof(struct thread_trap);

    struct thread_trap *trap = (struct thread_trap *)thread->esp;
    memset(trap, 0x0, sizeof(struct thread_trap));
    trap->addr = SCHED_PAGE_FAULT;

    dlist_head_init(&thread->list);

    process->thread = thread;

    return thread;
}

struct thread *sched_create_kernel_thread(struct process *process, uint32_t parameter)
{
    struct thread *thread = sched_create_thread(process);
    struct thread_trap *trap = (struct thread_trap *)thread->esp;
    trap->eip = (uint32_t)sched_kernel_thread_entry;
    trap->param1 = parameter;
    return thread;
}

struct thread *sched_create_elf_thread(struct process *process, uint32_t path)
{
    struct thread *thread = sched_create_thread(process);
    struct thread_trap *trap = (struct thread_trap *)thread->esp;
    trap->eip = (uint32_t)sched_elf_thread_entry;
    trap->param1 = (uint32_t)thread;
    trap->param2 = path;
    return thread;
}

struct process *sched_create_process(struct process *parent)
{
    struct process *process = calloc(1, sizeof(struct process));
    process->pid = sched_next_pid++;
    process->mount = calloc(1, sizeof(struct vfs_mount));
    process->root = calloc(1, sizeof(struct vfs_dentry));
    process->mm = calloc(1, sizeof(struct mmap_mm));

    dlist_head_init(&process->mm->list);

    if (parent)
    {
        process->page_dir = virt_mm_create_address();
        memcpy(process->mount, parent->mount, sizeof(struct vfs_mount));
        memcpy(process->root, parent->root, sizeof(struct vfs_dentry));
        memcpy(process->files, parent->files, sizeof(struct vfs_file));
    }
    else
    {
        process->page_dir = virt_mm_dir;
    }

    return process;
}

struct thread *sched_get_next_thread()
{
    if (dlist_is_empty(&sched_ready_list))
        return NULL;

    return dlist_first_entry(&sched_ready_list, struct thread, list);
}

struct thread *sched_pop_next_thread()
{
    if (dlist_is_empty(&sched_ready_list))
        return NULL;

    struct thread *thread = dlist_first_entry(&sched_ready_list, struct thread, list);
    dlist_remove_init(&thread->list);
    return thread;
}

void sched_schedule()
{
    if (sched_thread->state == THREAD_RUN)
        return;

    sched_lock();

    struct thread *thread = sched_pop_next_thread();
    if (!thread)
    {
        do
        {
            sched_unlock();
            sys_hlt();
            sched_lock();

            thread = sched_pop_next_thread();
            if (!thread && sched_thread->state == THREAD_RUN)
                thread = sched_thread;
        } while (!thread);
    }

    if (sched_thread == thread)
    {
        sched_update_thread(sched_thread, THREAD_RUN);
        return;
    }

    struct thread *p_thread = sched_thread;
    sched_thread = thread;
    sched_update_thread(sched_thread, THREAD_RUN);

    struct process *process = sched_thread->process;
    sched_process = process;

    uint32_t physical = virt_mm_get_phys_addr((uint32_t)process->page_dir);

    tss_set_stack(sched_thread->kernel_stack);

    printf("Scheduler: Switch thread old esp = 0x%x, new esp = 0x%x, page dir = 0x%x\n", &p_thread->esp,
           sched_thread->esp, physical);
    sched_switch(&p_thread->esp, sched_thread->esp, physical);

    sched_unlock();
}

void sched_open(const char *path)
{
    struct process *process = sched_create_process(sched_process);
    struct thread *thread = sched_create_elf_thread(process, (uint32_t)strdup(path));
    thread->state = THREAD_READY;
    sched_add_thread(thread);
    printf("Scheduler: Queued elf process path = %s\n", path);
}

void sched_exit(int status)
{
    sched_lock();

    struct process *process = sched_process;
    process->exit_status = status;

    sched_update_thread(process->thread, THREAD_EXIT);
    printf("Scheduler: Process pid = %d exited\n", process->pid);

    sched_unlock();
    sched_schedule();
}

void sched_init(void *kernel_init)
{
    dlist_head_init(&sched_ready_list);
    dlist_head_init(&sched_wait_list);
    dlist_head_init(&sched_exit_list);

    sched_process = sched_create_process(NULL);
    sched_thread = sched_create_thread(sched_process);

    struct process *process = sched_create_process(sched_process);
    struct thread *thread = sched_create_kernel_thread(process, (uint32_t)kernel_init);

    sched_update_thread(sched_thread, THREAD_EXIT);
    sched_update_thread(thread, THREAD_READY);

    printf("Scheduler: Initialized\n");
}
