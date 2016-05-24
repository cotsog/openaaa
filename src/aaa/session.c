#include <sys/compiler.h>
#include <sys/cpu.h>

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <hash/fn.h>
#include <hash/list.h>
#include <hash/table.h>

#include <mem/map.h>
#include <mem/page.h>
#include <mem/pool.h>

#define AAA_HTABLE_BITS 9

DEFINE_HASHTABLE(__aaa_map_sid, AAA_HTABLE_BITS);
DEFINE_HASHTABLE(__aaa_map_sec, AAA_HTABLE_BITS);
DEFINE_HASHTABLE(__aaa_map_key, AAA_HTABLE_BITS);

struct pagemap *__mem_map;

struct aaa_sess;
struct aaa_ctxt;

struct aaa_session {
	byte sid[40];                   /* tls session id  */
	byte cid[40];                   /* tls channel id  */
	byte sec[40];                   /* tls binding key */
	byte key[40];                   /* aaa binding key */
};

struct session {
	struct page page;
	timestamp_t created;
	timestamp_t modified;
	timestamp_t expired;
	struct hlist_node node_sid;
	struct hlist_node node_sec;
	struct hlist_node node_key;
	struct aaa_session aaa;
};

struct session *
aaa_sesson_get(struct pagemap *map, const char *id)
{
	struct page *page = page_alloc_safe(map);
	struct session *obj = (struct session *)page;

	u32 hash = hash_string(id);
	hash_add(__aaa_map_sid, &obj->node_sid, hash);

	return NULL;
}

void
aaa_session_del(struct pagemap  *map, struct session *sess)
{

}
