#pragma once

#include <stdbool.h>

#define dlist_container_of(ptr, type, member)                                                                          \
    ({                                                                                                                 \
        const typeof(((type *)0)->member) *__mptr = (ptr);                                                             \
        (type *)((char *)__mptr - offsetof(type, member));                                                             \
    })

#define dlist_entry(ptr, type, member) dlist_container_of(ptr, type, member)

#define dlist_first_entry(ptr, type, member) dlist_entry((ptr)->next, type, member)

#define dlist_next_entry(pos, member) dlist_entry((pos)->member.next, typeof(*(pos)), member)

#define dlist_for_each_entry(pos, head, member)                                                                        \
    for (pos = dlist_first_entry(head, typeof(*pos), member); &pos->member != (head);                                  \
         pos = dlist_next_entry(pos, member))

#define dlist_for_each_entry_safe(pos, n, head, member)                                                                \
    for (pos = dlist_first_entry(head, typeof(*pos), member), n = dlist_next_entry(pos, member);                       \
         &pos->member != (head); pos = n, n = dlist_next_entry(n, member))

struct dlist_head
{
    struct dlist_head *next;
    struct dlist_head *previous;
};

void dlist_head_init(struct dlist_head *list);
bool dlist_empty(const struct dlist_head *head);
bool dlist_is_last(const struct dlist_head *list, const struct dlist_head *head);
void dlist_add(struct dlist_head *new, struct dlist_head *head);
void dlist_add_tail(struct dlist_head *new, struct dlist_head *head);
void dlist_remove(struct dlist_head *entry);
void dlist_remove_init(struct dlist_head *entry);
