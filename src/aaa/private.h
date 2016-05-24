/*
 * $private.h                                       Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 */

#ifndef __AAA_LIB_PRIVATE_H__
#define __AAA_LIB_PRIVATE_H__

#include <stdlib.h>
#include <stdarg.h>

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <mem/map.h>
#include <mem/page.h>

#include <hash/list.h>
#include <hash/table.h>

#include <ctypes/lib.h>
#include <ctypes/object.h>

__BEGIN_DECLS

#define aaa_dbg sys_dbg

/* API version, they compare as integers */
#define API_VERSION PACKAGE_VERSION

/* a private structures containing the context */
struct store;

struct aaa {
	struct object object;
	struct store *store;
	void *userdata;
	struct mempool *mp;
};

struct aaa_session_page {
	struct page page;
	s64 created;
	s64 modified;
	s64 expired;
	u32 id;
	byte key[64];
};

struct aaa_session {
	byte sid[40];                   /* tls session id  */
	byte sec[40];                   /* tls binding key */
	byte key[40];                   /* aaa binding key */
};

struct session {
	struct page page;
	timestamp_t created;
	timestamp_t modified;
	timestamp_t expired;
	struct hlist_node sid;
	struct hlist_node sec;
	struct hlist_node key;
	struct aaa_session aaa;
};

struct session *
aaa_session_create(struct pagemap *map, const char *id);

extern struct aaa_node_matrix init_matrix;

struct store *
store_new(struct mempool *p, struct object *object);

void
store_free(struct store *);

int
store_bind(struct store *store, const char *id);

int
store_set(struct store *, const char *attr, char *value);

const char *
store_get(struct store *, const char *attr);

int
store_select(struct store *, const char *path);

int
store_commit(struct store *);

__END_DECLS

#endif/*__AAA_LIB_PRIVATE_H__*/
