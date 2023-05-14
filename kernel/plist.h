#pragma once

#include <kernel/dlist.h>

#define plist_first_entry(head, type, member) dlist_container_of(plist_first(head), type, member)

#define plist_last_entry(head, type, member) dlist_container_of(plist_last(head), type, member)

#define plist_for_each_entry(pos, head, mem) dlist_for_each_entry(pos, &(head)->node_list, mem.node_list)

struct plist_head
{
    struct dlist_head node_list;
};

struct plist_node
{
    int priority;
    struct dlist_head priority_list;
    struct dlist_head node_list;
};

void plist_head_init(struct plist_head *head);
void plist_node_init(struct plist_node *node, int priority);
bool plist_head_empty(const struct plist_head *head);
struct plist_node *plist_first(const struct plist_head *head);
struct plist_node *plist_last(const struct plist_head *head);
void plist_add(struct plist_node *node, struct plist_head *head);
void plist_remove(struct plist_node *node, struct plist_head *head);