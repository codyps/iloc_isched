#ifndef LIST_H_
#define LIST_H_

struct list_head { struct list_head *prev, *next; };

#define LIST_HEAD(lhead) struct list_head lhead = \
	{ .prev = &lhead, .next = &lhead }

#define container_of(item, type, member) \
		((type *)((char *)(item) - offsetof(type, member)))
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_entry_prev(pos, head, member)                  \
	for (pos = list_entry((head)->prev, typeof(*pos), member);    \
	     &pos->member != (head);                                  \
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

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
