/**
 * @file kernel/task/scheduler.h
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
#pragma once

#include <kernel/memory/virtual.h>
#include <kernel/dlist.h>
#include <stdint.h>

#define SCHED_STACK 0x2000
#define SCHED_PAGE_FAULT 0xFFFFFFFF

struct process
{
    struct page_dir *page_dir;
};

enum thread_state
{
    THREAD_NEW,
    THREAD_READY,
    THREAD_RUN,
    THREAD_WAIT,
    THREAD_EXIT,
};

struct thread
{
    uint32_t esp;
    uint32_t kernel_stack;
    enum thread_state state;
    struct process *process;
    struct dlist_head list;
};

struct thread_trap
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
    uint32_t addr;
    uint32_t param;
};

void sched_init(void *kernel_init);
void sched_schedule();
void sched_update_thread(struct thread *thread, enum thread_state state);
struct process *sched_current_process();
struct thread *sched_current_thread();
void sched_lock();
void sched_unlock();
