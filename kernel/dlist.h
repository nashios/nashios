/**
 * @file kernel/dlist.h
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief Doubly linked list
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

#include <stdbool.h>
#include <stddef.h>

#define dlist_container(head, type, name) ({          \
    const typeof(((type *)0)->name) *p_head = (head); \
    (type *)((char *)p_head - offsetof(type, name));  \
})

#define dlist_entry(head, type, name) dlist_container(head, type, name)

#define dlist_first_entry(head, type, name) dlist_entry((head)->next, type, name)

#define dlist_next_entry(entry, name) dlist_entry((entry)->name.next, typeof(*entry), name)

#define dlist_foreach_entry(entry, head, name)                  \
    for (entry = dlist_first_entry(head, typeof(*entry), name); \
         &entry->name != (head);                                \
         entry = dlist_next_entry(entry, name))

struct dlist_head
{
    struct dlist_head *next;
    struct dlist_head *previous;
};

static inline void dlist_head_init(struct dlist_head *list)
{
    list->next = list;
    list->previous = list;
}

static inline bool dlist_is_empty(const struct dlist_head *head)
{
    return head->next == head;
}

static inline void dlist_add(struct dlist_head *new, struct dlist_head *previous, struct dlist_head *next)
{
    next->previous = new;
    new->next = next;
    new->previous = previous;
    previous->next = new;
}

static inline void dlist_add_tail(struct dlist_head *new, struct dlist_head *head)
{
    dlist_add(new, head->previous, head);
}

static inline void dlist_remove_internal(struct dlist_head *previous, struct dlist_head *next)
{
    next->previous = previous;
    previous->next = next;
}

static inline void dlist_remove_entry(struct dlist_head *list)
{
    dlist_remove_internal(list->previous, list->next);
}

static inline void dlist_remove_init(struct dlist_head *list)
{
    dlist_remove_entry(list);
    dlist_head_init(list);
}
