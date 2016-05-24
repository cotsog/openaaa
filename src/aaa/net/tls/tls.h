#ifndef __CRYPTO_TLS_H
#define __CRYPTO_TLS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <mem/page.h>
#include <mem/map.h>
#include <mem/stack.h>

#define TLS_SESSION_ID_MAX    64
#define TLS_SESSION_KEY_MAX   64
#define TLS_SESSION_URI_MAX   256

#define DECLARE_CALL(fn, rv, args...) \
        rv (*openssl_##fn)(args)
#define DECLARE_LINK(fn) \
        openssl_##fn = dlsym(RTLD_NEXT, #fn); \

#define TRACE_CALL prints("%s\n", __func__)
#define HOOK_CALL_INIT prints("%s init\n", __func__)
#define HOOK_CALL_FINI prints("%s fini\n", __func__)

#define prints(args...) fprintf (stdout, args)

#define DEFINE_SYMBOL_TABLE_BEGIN \
	static inline void init_symbols(void) { \
		tls_cache_init();
#define DEFINE_SYMBOL_TABLE_END }

extern struct map *map;

void                                                                     
tls_cache_init(void);

void                                                                     
tls_cache_fini(void);

#endif
