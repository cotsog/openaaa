/* $list.h  Daniel Kubec <niel@rtfm.cz> */

#ifndef __CORE_LIST_H__
#define __CORE_LIST_H__

#include <sys/compiler.h>
#include <sys/decls.h>
#include <stdlib.h>

struct node {
	struct node *next, *prev;
};

struct list {
	struct node head;
};

static inline void 
list_init(struct list *l)
{
	struct node *head = &l->head;
	head->next = head->prev = head;
}

static inline void *
list_head(struct list *l)
{
	return (l->head.next != &l->head) ? l->head.next : NULL;
}

static inline void *
list_tail(struct list *l)
{
	return (l->head.prev != &l->head) ? l->head.prev : NULL;
}

static inline void *
list_next(struct list *l, struct node *n)
{
	return (n->next != &l->head) ? (void *) n->next : NULL;
}

static inline void *
list_prev(struct list *l, struct node *n)
{
	return (n->prev != &l->head) ? (void *) n->prev : NULL;
}

static inline int
list_empty(struct list *l)
{
	return (l->head.next == &l->head);
}

static inline void
list_insert_after(struct node *what, struct node *after)
{
	struct node *before = after->next;
	what->next = before;
	what->prev = after;
	before->prev = what;
	after->next = what;
}

static inline void
list_insert_before(struct node *what, struct node *before)
{
	struct node *after = before->prev;
	what->next = before;
	what->prev = after;
	before->prev = what;
	after->next = what;
}

static inline void
list_add_head(struct list *l, struct node *n)
{
	list_insert_after(n, &l->head);
}

static inline void
list_add_tail(struct list *l, struct node *n)
{
	list_insert_before(n, &l->head);
}

static inline void
list_remove(struct node *n)
{
	struct node *before = n->prev;
	struct node *after = n->next;
	before->next = after;
	after->prev = before;
}

static inline void *
list_remove_head(struct list *l)
{
	struct node *n = (struct node *)list_head(l);
	if (n)
		list_remove(n);
	return n;
}

static inline void 
list_insert_list_after(struct list *what, struct node *after)
{
	if (list_empty(what))
		return;

	struct node *w = &what->head;
	w->prev->next = after->next;
	after->next->prev = w->prev;
	w->next->prev = after;
	after->next = w->next;
	list_init(what);
}

static inline void
list_move(struct list *to, struct list *from)
{
	list_init(to);
	list_insert_list_after(from, &to->head);
	list_init(from);
}

#define list_walk(n, list) \
        for (n = (void*)(list).head.next;\
            (struct node*)(n) != &(list).head;\
            n = (void*)((struct node*)(n))->next)

#define list_walk_delsafe(n, list, tmp) \
        for (n = (void*)(list).head.next;\
            tmp = (void*)((struct node*)(n))->next, (struct node*)(n) != &(list).head;\
            n = (void*)tmp)

#define list_for_each(type, n, list) \
        for (type n = (type)(list).head.next;\
            (struct node*)(n) != &(list).head;\
            n = (type)((struct node*)(n))->next)

#define list_for_each_delsafe(type, n, list, tmp) \
        for (type n = (void*)(list).head.next; \
            tmp = (void*)((struct node*)(n))->next, (struct node*)(n) != &(list).head;\
            n = (void*)tmp)

#endif
