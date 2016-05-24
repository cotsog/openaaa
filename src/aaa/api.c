/*
 * $init.c                                          Daniel Kubec <niel@rtfm.cz> 
 *                                                                              
 * This software may be freely distributed and used according to the terms      
 * of the GNU Lesser General Public License.                                    
 */

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>

#include <ctypes/lib.h>
#include <ctypes/list.h>
#include <ctypes/object.h>

#include <mem/page.h>
#include <mem/pool.h>

#include <aaa/lib.h>
#include <aaa/private.h>

void
aaa_init(void)
{
	sys_init();
}

void
aaa_fini(void)
{
	sys_fini();
}

struct aaa *
aaa_new(void)
{
	struct mempool *mp = mp_new(CPU_PAGE_SIZE);
	struct aaa *a = object_alloc_zero(sizeof(*a));

	a->mp = mp;
	a->store = store_new(mp, &a->object);

	return a;
}
                                                                                
void                                                                            
aaa_free(struct aaa *a)
{
	mp_delete(a->mp);
	store_free(a->store);
	object_free(a);
}

int
aaa_set_opt(struct aaa *a, enum aaa_opt_e opt, const char *v)
{
	switch (opt) {
	case AAA_OPT_USERDATA:
		a->userdata = (void *)v;
		break;
	case AAA_OPT_CUSTOMLOG:
		//log_custom((sys_custom_log_t)v, a->userdata);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

const char *
aaa_get_opt(struct aaa *a, enum aaa_opt_e opt)
{
	switch (opt) {
	case AAA_OPT_USERDATA:
		return (const char *)a->userdata;
	case AAA_OPT_CUSTOMLOG:
		return NULL;
	}

	return NULL;
}

int
aaa_bind(struct aaa *aaa, enum aaa_bind_e type, const char *id)
{
	return store_bind(aaa->store, id);
}

int
aaa_attr_set(struct aaa *aaa, const char *attr, char *value)
{
	return store_set(aaa->store, attr, value);
}

const char *
aaa_attr_get(struct aaa *aaa, const char *attr)
{
	return store_get(aaa->store, attr);
}

int                                                                             
aaa_object_set(struct aaa *aaa, void *addr, unsigned int size)
{
	return -1;
}
                                                                                
void *                                                                          
aaa_object_get(struct aaa *aaa, unsigned int *size)
{
	return NULL;
}

int                                                                             
aaa_attr_del_value(struct aaa *aaa, const char *key, const char *val)
{
	return -EINVAL;
}
                                                                                
int                                                                             
aaa_attr_has_value(struct aaa *aaa, const char *key, const char *val)
{
	return -EINVAL;
}

const char *                                                                    
aaa_attr_find_first(struct aaa *aaa, const char *path, unsigned recurse)
{
	return NULL;
}
                                                                                
const char *                                                                    
aaa_attr_find_next(struct aaa *aaa)
{
	return NULL;
}

int
aaa_select(struct aaa *aaa, const char *path)
{
	return store_select(aaa->store, path);
}

int                                                                             
aaa_touch(struct aaa *aaa)
{
	return -1;
}

int
aaa_commit(struct aaa *aaa)
{
	return store_commit(aaa->store);
}

int                                                                             
aaa_last_error(struct aaa *aaa)
{
	return -1;
}
