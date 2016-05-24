/*
 * attr.c                        (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctypes/lib.h>
#include <ctypes/list.h>
#include <ctypes/object.h>
#include <mem/pool.h>

struct attr *
attr_lookup(struct object *object, const char *key, int create)
{
	list_for_each(struct attr *, a, object->attrs)
		if (!strcmp(a->key, key))
			return a;

	if (!create)
		return NULL;

	struct attr *a = mp_alloc_zero(object->mp, sizeof(*a));
	a->key = mp_strdup(object->mp, key);
	list_add_tail(&object->attrs, &a->node);

	return a;
}

int
attr_set(struct object *object, const char *key, const char *val)
{
	struct attr *a = attr_lookup(object, key, 1);
	a->val = val ? mp_strdup(object->mp, val) : NULL;
	a->flags |= ATTR_CHANGED;
	return 0;
}

int
attr_set_fmt(struct object *object, const char *key, const char *fmt, ... )
{
	struct attr *a = attr_lookup(object, key, 1);

        va_list args;
        va_start(args, fmt);

	a->val = mp_vprintf(object->mp, fmt, args);
	a->flags |= ATTR_CHANGED;

	va_end(args);
	return 0;
}

int
attr_set_num(struct object *object, const char *key, int num)
{
	struct attr *a = attr_lookup(object, key, 1);
	a->val = mp_printf(object->mp, "%d", num);
	a->flags |= ATTR_CHANGED;
	return 0;
}

int
attr_raw_set(struct object *object, const char *key, const char *val)
{
	struct attr *a = attr_lookup(object, key, 1);
	a->val = val ? mp_strdup(object->mp, val) : NULL;
	return 0;
}

const char *
attr_get(struct object *object, const char *key)
{
	struct attr *a = attr_lookup(object, key, 0);
	return a ? a->val : NULL;
}

struct attr *
attr_find(struct object *object, const char *key)
{
	list_for_each(struct attr *, a, object->attrs)
		if (!strcmp(a->key, key))
			return a;
	return NULL;
}
