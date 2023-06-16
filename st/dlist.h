#pragma once

#include <stdbool.h>
#include <stddef.h>

#define dlist_container_of(ptr, type, member)                                                                          \
    ({                                                                                                                 \
        const typeof(((type *)0)->member) *__mptr = (ptr);                                                             \
        (type *)((char *)__mptr - offsetof(type, member));                                                             \
    })

#define dlist_entry(ptr, type, member) dlist_container_of(ptr, type, member)

#define dlist_first_entry(ptr, type, member) dlist_entry((ptr)->next, type, member)

#define dlist_first_entry_or_null(ptr, type, member)                                                                   \
    ({                                                                                                                 \
        struct dlist_head *__head = (ptr);                                                                             \
        struct dlist_head *__position = __head->next;                                                                  \
        __position != __head ? dlist_entry(__position, type, member) : NULL;                                           \
    })

#define dlist_last_entry(ptr, type, member) dlist_entry((ptr)->previous, type, member)

#define dlist_next_entry(pos, member) dlist_entry((pos)->member.next, typeof(*(pos)), member)

#define dlist_prev_entry(pos, member) dlist_entry((pos)->member.previous, typeof(*(pos)), member)

#define dlist_for_each_entry(pos, head, member)                                                                        \
    for (pos = dlist_first_entry(head, typeof(*pos), member); &pos->member != (head);                                  \
         pos = dlist_next_entry(pos, member))

#define dlist_for_each_entry_safe(pos, n, head, member)                                                                \
    for (pos = dlist_first_entry(head, typeof(*pos), member), n = dlist_next_entry(pos, member);                       \
         &pos->member != (head); pos = n, n = dlist_next_entry(n, member))

#define dlist_for_each_entry_reverse(pos, head, member)                                                                \
    for (pos = dlist_last_entry(head, typeof(*pos), member); &pos->member != (head);                                   \
         pos = dlist_prev_entry(pos, member))

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

static inline bool dlist_empty(const struct dlist_head *head) { return head->next == head; }

static inline bool dlist_is_last(const struct dlist_head *list, const struct dlist_head *head)
{
    return list->next == head;
}

static inline bool dlist_remove_entry_valid(struct dlist_head *entry)
{
    return entry->next != NULL && entry->previous != NULL;
}

static inline void dlist_remove_internal(struct dlist_head *previous, struct dlist_head *next)
{
    next->previous = previous;
    previous->next = next;
}

static inline void dlist_remove_entry(struct dlist_head *entry)
{
    if (!dlist_remove_entry_valid(entry))
        return;

    dlist_remove_internal(entry->previous, entry->next);
}

static inline void dlist_remove_init(struct dlist_head *entry)
{
    dlist_remove_entry(entry);
    dlist_head_init(entry);
}

static inline void dlist_remove(struct dlist_head *entry)
{
    dlist_remove_entry(entry);

    entry->next = NULL;
    entry->previous = NULL;
}

static inline void ___dlist_add(struct dlist_head *new, struct dlist_head *previous, struct dlist_head *next)
{
    next->previous = new;
    new->next = next;
    new->previous = previous;
    previous->next = new;
}

static inline void dlist_add(struct dlist_head *new, struct dlist_head *head) { ___dlist_add(new, head, head->next); }

static inline void dlist_add_tail(struct dlist_head *new, struct dlist_head *head)
{
    ___dlist_add(new, head->previous, head);
}

static inline bool dlist_is_poison(const struct dlist_head *entry)
{
    return entry->previous == NULL && entry->next == NULL;
}
