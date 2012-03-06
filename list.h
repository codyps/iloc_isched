#ifndef LIST_H_
#define LIST_H_

#include <stddef.h>
#include <stdbool.h>

struct list_head { struct list_head *prev, *next; };

#define LIST_HEAD(lhead) struct list_head lhead = \
	{ .prev = &lhead, .next = &lhead }

static inline void list_init(struct list_head *head)
{
	head->prev = head->next = head;
}

#define container_of(item, type, member) \
		((type *)((char *)(item) - offsetof(type, member)))
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->prev)

#define list_for_each_safe(pos, tmp, head)				\
	for (pos = (head)->next, tmp = pos->next; pos != (head); pos = tmp, tmp = pos->next)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     (&pos->member) != (head);					\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_prev(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

/* typeof(pos) == typeof(tmp) */
#define list_for_each_entry_safe(pos, tmp, head, member)					\
	for (pos = list_entry((head)->next, typeof(*pos), member),				\
			tmp = list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);								\
	     pos = tmp,	tmp = list_entry(pos->member.next, typeof(*pos), member))

static inline unsigned list_length(struct list_head *head)
{
	struct list_head *pos;
	unsigned i = 0;
	list_for_each(pos, head) {
		i++;
	}

	return i;
}

static inline bool list_is_empty(struct list_head *head)
{
	return head == head->next;
}

static inline void list_del(struct list_head *elem)
{
	struct list_head *n = elem->next, *p = elem->prev;
	p->next = n;
	n->prev = p;
	list_init(elem);
}

/* @head - a 'head', which may already have other nodes attached to it
 * @n    - a new node which does not have any attached nodes. */
static inline void list_add(struct list_head *head, struct list_head *n)
{
	n->next = head->next;
	n->prev = head;

	n->next->prev = n;
	head->next = n;
}

static inline void list_add_prev(struct list_head *head, struct list_head *n)
{
	n->next = head;
	n->prev = head->prev;

	n->prev->next = n;
	head->prev = n;
}

static inline void list_head_init(struct list_head *list)
{
	list->next = list->prev = list;
}

/* @a - a list of things, lacks a head
 * @b - a list of things, lacks a head */
static inline void list_join(struct list_head *a, struct list_head *b)
{
	/*
	b->next = a->next;
	a->next = b;
	*/

	struct list_head *a_tail = a->prev;
	struct list_head *b_tail = b->prev;

	a_tail->next = b;
	b_tail->next = a;
	a->prev = b_tail;
	b->prev = a_tail;
}

/* @h  - a new head.
 * @ls - the start node of a headless list */
static inline void list_attach_head(struct list_head *h,
		struct list_head *ls)
{
	h->next = ls;
	h->prev = ls->prev;

	ls->prev->next = h;
	ls->prev = h;
}

#endif
