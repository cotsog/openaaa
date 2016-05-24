#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <sys/compiler.h>
#include <sys/missing.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#include <ctypes/lib.h>
#include <mem/page.h>
#include <mem/map.h>
#include <mem/stack.h>

#include "lib.h"
#include "tls.h"

struct map *map = NULL;

static struct tls_ekm tls_ekm;
static int endpoint_type = 0;
const char *authority;
const char *handler;

void
tls_init(void)
{
	tls_openssl_init();
	tls_nss_init();
}

void
tls_fini(void)
{
	tls_nss_fini();
	tls_openssl_fini();
}
                                                                                
void                                                                     
tls_cache_init(void)                                                            
{                                                                               
	const char *file = stk_printf("/tmp/tls.map");
	map = mmap_open(file, MAP_SHARED, 12, 1000);
}                                                                               
                                                                                
void                                                                     
tls_cache_fini(void)                                                            
{                                                                               
	if (map)                                                                
		mmap_close(map);
}

struct tls_ekm *
tls_get_ekm_attrs(void)
{
	return &tls_ekm;
}

int
tls_set_ekm_attrs(struct tls_ekm *ekm)
{
	memcpy(&tls_ekm, ekm, sizeof(*ekm));
	return 0;
}

void
tls_set_authority(const char *str)
{
	if (str)
		authority = strdup(str);
}

const char *
tls_get_authority(void)
{
	return authority;
}

void
tls_set_handler(const char *str)
{
	handler = str;
}

const char *
tls_get_handler(void)
{
	return handler;
}

void
tls_set_endpoint(int id)
{
	endpoint_type = id;
}


int
tls_get_endpoint(void)
{
	return endpoint_type;
}
