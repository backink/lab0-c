#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

void reverse_between(struct list_head *, struct list_head *);

void merge(struct list_head *, struct list_head *, bool);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head) {
        return NULL;
    }

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe(entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *ele = malloc(sizeof(element_t));
    if (!ele) {
        return false;
    }

    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }

    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *ele = malloc(sizeof(element_t));
    if (!ele) {
        return false;
    }

    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }

    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *ele = list_first_entry(head, element_t, list);
    if (ele->value && sp) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(head->next);
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *ele = list_last_entry(head, element_t, list);
    if (ele->value && sp) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(head->prev);
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    int ret = 0;
    struct list_head *tmp = NULL;
    list_for_each(tmp, head) {
        ret++;
    }
    return ret;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *first = head->next, *last = head->prev;
    while (first != last && first->next != last) {
        first = first->next;
        last = last->prev;
    }

    element_t *ele = list_entry(first, element_t, list);
    list_del(first);
    q_release_element(ele);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }

    element_t *entry, *safe;
    bool dup = false;

    list_for_each_entry_safe(entry, safe, head, list) {
        if (&safe->list != head && !strcmp(entry->value, safe->value)) {
            dup = true;
            list_del(&entry->list);
            q_release_element(entry);
        } else if (dup) {
            dup = false;
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *node = head->next;
    for (; node != head && node->next != head; node = node->next) {
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    const struct list_head *last = head->next;
    for (struct list_head *node = head; head->prev != last; node = node->next) {
        list_move(head->prev, node);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1) {
        return;
    }

    struct list_head *start = head->next;
    struct list_head *node;
    while (start != head) {
        int n = k - 1;
        for (node = start; n > 0 && node->next != head; node = node->next) {
            n--;
        }
        if (n == 0) {
            reverse_between(start, node);
            start = start->next;
        } else {
            break;
        }
    }
}

void reverse_between(struct list_head *start, struct list_head *end)
{
    struct list_head *head = start->prev;
    struct list_head *tmp;
    while (end != start) {
        tmp = end->prev;
        list_move(end, head);
        head = head->next;
        end = tmp;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *first = head->next, *last = head->prev;
    while (first != last) {
        last = last->prev;
        if (first == last) {
            break;
        }
        first = first->next;
    }
    LIST_HEAD(head_to);
    list_cut_position(&head_to, head, first);
    q_sort(head, descend);
    q_sort(&head_to, descend);
    merge(head, &head_to, descend);
}

void merge(struct list_head *head_main, struct list_head *head, bool descend)
{
    struct list_head *node_main = head_main->next, *node = head->next;
    struct list_head *tmp;
    int flag = descend ? 1 : -1;
    while (node_main != head_main && node != head) {
        const element_t *entry_main = list_entry(node_main, element_t, list);
        const element_t *entry = list_entry(node, element_t, list);
        while (node_main != head_main &&
               (flag * strcmp(entry_main->value, entry->value) > 0)) {
            node_main = node_main->next;
            entry_main = list_entry(node_main, element_t, list);
        }
        tmp = node->next;
        list_move(node, node_main->prev);
        node = tmp;
    }
    if (node != head)
        list_splice_tail(head, head_main);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    struct list_head *min = head->prev;
    int ret = 1;
    while (min->prev != head) {
        const element_t *ele_min = list_entry(min, element_t, list);
        element_t *ele_pre = list_entry(min->prev, element_t, list);
        if (strcmp(ele_min->value, ele_pre->value) < 0) {
            list_del(min->prev);
            q_release_element(ele_pre);
        } else {
            min = min->prev;
            ret++;
        }
    }
    return ret;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    struct list_head *max = head->prev;
    int ret = 1;
    while (max->prev != head) {
        const element_t *ele_max = list_entry(max, element_t, list);
        element_t *ele_pre = list_entry(max->prev, element_t, list);
        if (strcmp(ele_max->value, ele_pre->value) > 0) {
            list_del(max->prev);
            q_release_element(ele_pre);
        } else {
            max = max->prev;
            ret++;
        }
    }
    return ret;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    struct list_head *first = head->next, *last = head->prev;
    int ret = 0;
    bool count_size = true;
    while (last != head->next) {
        while (first != last && first->prev != last) {
            queue_contex_t *q_first = list_entry(first, queue_contex_t, chain);
            queue_contex_t *q_last = list_entry(last, queue_contex_t, chain);
            if (count_size) {
                ret += q_first->size + q_last->size;
            }
            merge(q_first->q, q_last->q, descend);
            first = first->next;
            last = last->prev;
        }
        if (count_size && first == last) {
            const queue_contex_t *q = list_entry(first, queue_contex_t, chain);
            ret += q->size;
        }
        count_size = false;
        first = head->next;
    }
    return ret;
}
