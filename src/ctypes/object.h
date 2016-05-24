/*
 * (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __SYS_OBJECT_H__
#define __SYS_OBJECT_H__

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <mem/pool.h>
#include <hash/fn.h>
#include <ctypes/list.h>

#define ATTR_MULTI   0x1
#define ATTR_CHANGED 0x2
#define ATTR_DELETED 0x4

#define ATTR_MAX_KEY_LEN 64
#define ATTR_MAX_VAL_LEN 2048

__BEGIN_DECLS

struct object {
	struct node node;
	struct object *parent;
	struct mempool *mp;
	struct list attrs;
	struct list childs;
	void *userdata;
};

struct attr {
	struct node node;
	const char *key;
	const char *val;
	int flags;
};

void *
object_alloc(unsigned int size);

void *
object_alloc_zero(unsigned int size);

void
object_free(void *);

void
object_merge(struct object *from, struct object *to);

void
object_flush(void *o);

int
object_write(struct object *, const char *addr, unsigned int max);

int
object_read(struct object *object, byte *pkt,  unsigned int len);

struct attr *
attr_lookup(struct object *o, const char *key, int create);

int
attr_set(struct object *o, const char *key, const char *val);

int
attr_set_fmt(struct object *, const char *key, const char *fmt, ...);

int
attr_set_num(struct object *, const char *key, int num);

int
attr_raw_set(struct object *o, const char *key, const char *val);

int
attr_add(struct object *,const char *key, const char *val);

int
attr_add_fmt(struct object *, const char *key, const char *fmt, ...);

int
attr_set_raw(struct object *o, const char *key, const char *val);

const char *
attr_get(struct object *o, const char *key);

struct attr *
attr_find(struct object *o, const char *key);

__END_DECLS

#endif
