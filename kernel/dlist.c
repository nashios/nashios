#include <kernel/dlist.h>
#include <stddef.h>

void dlist_head_init(struct dlist_head *list)
{
    list->next = list;
    list->previous = list;
}

bool dlist_empty(const struct dlist_head *head) { return head->next == head; }

bool dlist_is_last(const struct dlist_head *list, const struct dlist_head *head) { return list->next == head; }

bool dlist_remove_entry_valid(struct dlist_head *entry) { return entry->next != NULL && entry->previous != NULL; }

void dlist_remove_internal(struct dlist_head *previous, struct dlist_head *next)
{
    next->previous = previous;
    previous->next = next;
}

void dlist_remove_entry(struct dlist_head *entry)
{
    if (!dlist_remove_entry_valid(entry))
        return;

    dlist_remove_internal(entry->previous, entry->next);
}

void dlist_remove_init(struct dlist_head *entry)
{
    dlist_remove_entry(entry);
    dlist_head_init(entry);
}

void dlist_remove(struct dlist_head *entry)
{
    dlist_remove_entry(entry);

    entry->next = NULL;
    entry->previous = NULL;
}

void dlist_add_internal(struct dlist_head *new, struct dlist_head *previous, struct dlist_head *next)
{
    next->previous = new;
    new->next = next;
    new->previous = previous;
    previous->next = new;
}

void dlist_add(struct dlist_head *new, struct dlist_head *head) { dlist_add_internal(new, head, head->next); }

void dlist_add_tail(struct dlist_head *new, struct dlist_head *head) { dlist_add_internal(new, head->previous, head); }