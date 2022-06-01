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
#include <kernel/task/scheduler.h>
#include <kernel/task/tss.h>
#include <kernel/interrupts/irq.h>
#include <kernel/system/sys.h>
#include <kernel/stdlib.h>
#include <kernel/stdio.h>
#include <kernel/string.h>

static struct process *sched_process = NULL;
static struct thread *sched_thread = NULL;
static uint32_t sched_locks = 0;
static struct dlist_head sched_ready_list = {};
static struct dlist_head sched_wait_list = {};
static struct dlist_head sched_exit_list = {};

extern void sched_switch(uint32_t *old_esp, uint32_t new_esp, uint32_t physical);

struct thread *sched_current_thread()
{
    return sched_thread;
}

struct process *sched_current_process()
{
    return sched_process;
}

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

void sched_thread_entry(void *kernel_init())
{
    kernel_init();
    sched_schedule();
}

struct thread *sched_create_thread(struct process *process, uint32_t parameter)
{
    struct thread *thread = calloc(1, sizeof(struct thread));
    thread->process = process;
    thread->state = THREAD_NEW;
    thread->kernel_stack = (uint32_t)calloc(SCHED_STACK, sizeof(char)) + SCHED_STACK;
    thread->esp = thread->kernel_stack - sizeof(struct thread_trap);

    struct thread_trap *trap = (struct thread_trap *)thread->esp;
    memset(trap, 0x0, sizeof(struct thread_trap));

    trap->eip = (uint32_t)sched_thread_entry;
    trap->param = parameter;
    trap->addr = SCHED_PAGE_FAULT;

    dlist_head_init(&thread->list);

    return thread;
}

struct process *sched_create_process(struct page_dir *dir)
{
    struct process *process = calloc(1, sizeof(struct process));
    process->page_dir = dir ? virt_mm_create_address() : virt_mm_dir;

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

    printf("Scheduler: Switch thread old esp = 0x%x, new esp = 0x%x, page dir = 0x%x\n", &p_thread->esp, sched_thread->esp, physical);
    sched_switch(&p_thread->esp, sched_thread->esp, physical);

    sched_unlock();
}

void sched_init(void *kernel_init)
{
    dlist_head_init(&sched_ready_list);
    dlist_head_init(&sched_wait_list);
    dlist_head_init(&sched_exit_list);

    sched_process = sched_create_process(NULL);
    sched_thread = sched_create_thread(sched_process, 0);

    struct process *process = sched_create_process(sched_process->page_dir);
    struct thread *thread = sched_create_thread(process, (uint32_t)kernel_init);

    sched_update_thread(sched_thread, THREAD_EXIT);
    sched_update_thread(thread, THREAD_READY);

    printf("Scheduler: Initialized\n");
}
