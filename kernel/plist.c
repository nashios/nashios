#include <kernel/plist.h>
#include <stddef.h>

void plist_head_init(struct plist_head *head) { dlist_head_init(&head->node_list); }

void plist_node_init(struct plist_node *node, int priority)
{
    node->priority = priority;
    dlist_head_init(&node->priority_list);
    dlist_head_init(&node->node_list);
}

bool plist_head_empty(const struct plist_head *head) { return dlist_empty(&head->node_list); }

struct plist_node *plist_first(const struct plist_head *head)
{
    return dlist_entry(head->node_list.next, struct plist_node, node_list);
}

struct plist_node *plist_last(const struct plist_head *head)
{
    return dlist_entry(head->node_list.previous, struct plist_node, node_list);
}

void plist_add(struct plist_node *node, struct plist_head *head)
{
    struct dlist_head *node_next = &head->node_list;
    if (plist_head_empty(head))
        goto ins_node;

    struct plist_node *first = plist_first(head);
    struct plist_node *iter = plist_first(head);
    struct plist_node *previous = NULL;
    do
    {
        if (node->priority < iter->priority)
        {
            node_next = &iter->node_list;
            break;
        }

        previous = iter;
        iter = dlist_entry(iter->priority_list.next, struct plist_node, priority_list);
    } while (iter != first);

    if (!previous || previous->priority != node->priority)
        dlist_add_tail(&node->priority_list, &iter->priority_list);
ins_node:
    dlist_add_tail(&node->node_list, node_next);
}

void plist_remove(struct plist_node *node, struct plist_head *head)
{
    if (!dlist_empty(&node->priority_list))
    {
        if (node->node_list.next != &head->node_list)
        {
            struct plist_node *next = dlist_entry(node->node_list.next, struct plist_node, node_list);
            if (dlist_empty(&next->priority_list))
                dlist_add(&next->priority_list, &node->priority_list);
        }
        dlist_remove_init(&node->priority_list);
    }
    dlist_remove_init(&node->node_list);
}