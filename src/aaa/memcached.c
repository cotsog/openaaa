/*
 * memcached.c                        (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */


#include <ctypes/list.h>
#include <ctypes/lib.h>
#include <mem/map.h>
#include <mem/page.h>
#include <mem/pool.h>
#include <mem/stack.h>
#include <mpm/sched.h>
#include <mpm/task.h>

#include <libmemcached/memcached.h>

#define st_ret_on(store, cond, err, fmt...)
#define mc_check(rv) \
	if (rv != MEMCACHED_SUCCESS) WATCHPOINT_ERROR(rv);

/* a private structures containing the context */
struct store {
	struct mempool *mpc;
	memcached_st *mcd;
	memcached_return mcr;
	struct memcache *mc;
	struct object *object;
	const char *id;
	unsigned int id_len;
	time_t expires;

};

/* public api functions */
struct store *
store_new(struct mempool *p, struct object *object)
{
	struct store *st = mp_alloc(p, sizeof(*st));
	const char *v = memcached_lib_version();

	st->mcd = memcached_create(NULL);
	st->mcr = memcached_server_add(st->mcd, "127.0.0.1", 11211);
	st->object = object;

	return st;
}

void
store_free(struct store *store)
{
	memcached_free(store->mcd);
}

static void                                                                     
set_time_t(struct object *m, const char *key, time_t val)                          
{                                                                               
	//raw_touch(attr_set_fmt(m, key, "%jd", (intmax_t)val), AT_CHANGED);      
}                                                                               
                                                                                
static time_t                                                                   
get_time_t(struct object *m, const char *key)                                      
{                                                                               
	intmax_t t;
	const char *text = attr_get(m, key);
	if (!text)
		return 0;
	else if (sscanf(text, "%jd", &t) != 1)
		return 0;
	else
	return t;
}

int
store_bind(struct store *st, const char *id)
{
	object_flush(st->object);
	st->id = mp_strdup(st->object->mp, id);
	st->id_len = strlen(id);

	time_t now = time(NULL);

	attr_raw_set(st->object, "sess.id", id);
	attr_raw_set(st->object, "sess.created", stk_printf("%jd", (intmax_t)now));
	attr_raw_set(st->object, "sess.modified", stk_printf("%jd", (intmax_t)now));
	attr_raw_set(st->object, "sess.access", stk_printf("%jd", (intmax_t)now));

	return 0;
}

int
store_set(struct store *st, const char *attr, char *value)
{
	return attr_set(st->object, attr, value);
}

const char *
store_get(struct store *st, const char *attr)
{
	return attr_get(st->object, attr);
}

int
store_select(struct store *st, const char *path)
{
	if (!st->id || !st->id_len)
		return -EINVAL;

	size_t len = 0;
	const char *v = memcached_get(st->mcd, st->id, st->id_len, 
	                              &len, 0, &st->mcr);
	if (!v || !len)
		return -EINVAL;

	object_read(st->object, (byte *)v, len);
	return 0;
}

int
store_commit(struct store *st)
{
	int updated = 0;
	list_for_each(struct attr *, a, st->object->attrs) {
		if (!(a->flags & ATTR_CHANGED))
			continue;

		a->flags &= ~ATTR_CHANGED;
		updated++;
	}

	if (!updated)
		return 0;

	char pkt[CPU_PAGE_SIZE] = {0};
	int size = object_write(st->object, pkt, CPU_PAGE_SIZE);
	pkt[size] = 0;

	time_t now = time(NULL);
	time_t expires = get_time_t(st->object, "sess.expires");
	size_t len = strlen(pkt);

	st->mcr = memcached_set(st->mcd, st->id, st->id_len, 
	                        pkt, len, now + expires, 0);
	return 0;
}
