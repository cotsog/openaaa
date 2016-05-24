/*
 * $id: openssl/init.c                              Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <sys/missing.h>
#include <sys/compiler.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

#include <aaa/lib.h>
#include <aaa/private.h>
#include <aaa/net/tls/lib.h>
#include <aaa/net/tls/openssl/sym.h>
#include <aaa/net/tls/openssl/ssl.h>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#ifdef CONFIG_LINUX
#include <link.h>
#endif

#include <unistd.h>
#include <dlwrap.h>

SSL *
openssl_get_ssl_from_store_ctx(X509_STORE_CTX *ctx)
{
	int index = sym_SSL_get_ex_data_X509_STORE_CTX_idx();
	return (SSL *)sym_X509_STORE_CTX_get_ex_data(ctx, index);
}

_noinline SSL_CTX *
sym_SSL_CTX_new_wrap(const SSL_METHOD *method)
{
	SSL_CTX *ctx = sym_SSL_CTX_new(method);

	if (!sym_SSL_CTX_add_server_custom_ext)
		return ctx;

	if (!sym_SSL_CTX_add_client_custom_ext)
		return ctx;	

	sym_SSL_CTX_add_client_custom_ext(ctx, TLSEXT_TYPE_supplemental_data, 
	                                  ssl_ext_cli_add, NULL, NULL,
	                                  ssl_ext_cli_parse, NULL);

	sym_SSL_CTX_add_server_custom_ext(ctx, TLSEXT_TYPE_supplemental_data, 
	                                  ssl_ext_srv_add, NULL, NULL, 
	                                  ssl_ext_srv_parse, NULL);
	return ctx;
}

_noinline void *
sym_SSL_get_ex_data_wrap(SSL *ssl, int idx)
{
        int i = idx == (int)~0U ? 0 : idx + 1;
	return sym_SSL_get_ex_data(ssl, i);
}

_noinline int
sym_SSL_set_ex_data_wrap(SSL *ssl, int idx, void *user)
{
        int i = idx == (int)~0U ? 0 : idx + 1;
        return sym_SSL_set_ex_data(ssl, i, user);
}

_noinline SSL *
sym_SSL_new_wrap(SSL_CTX *ctx)
{
	SSL *ssl = sym_SSL_new(ctx);
	int rv;
	struct tls_session *t = malloc(sizeof(*t));
	memset(t, 0, sizeof(*t));
	t->ssl = (void*)ssl;
	sym_SSL_set_ex_data_wrap(ssl, (int)~0U, (void  *)t);

	sym_SSL_callback_ctrl(ssl, SSL_CTRL_SET_TLSEXT_DEBUG_CB, openssl_tlsext_cb);

	//sym_SSL_set_info_callback(ssl, openssl_info_handler);
	//
	return ssl;
}

_noinline void
sym_SSL_free_wrap(SSL *s)
{
	struct tls_session *t = (struct tls_session*)sym_SSL_get_ex_data_wrap(s, (int)~0U);
	free(t);
	return sym_SSL_free(s);
}

_noinline void
sym_SSL_CTX_set_info_callback_wrap(SSL_CTX *ctx, void (*fn)())
{
	openssl_info_callback = fn;
	return sym_SSL_CTX_set_info_callback(ctx, openssl_info_handler);
}

_noinline void
sym_SSL_set_info_callback_wrap(SSL *ctx, void (*fn)())
{
	openssl_info_callback = fn;
	return sym_SSL_set_info_callback(ctx, openssl_info_handler);
}

static int
sym_module_callback(struct dl_phdr_info *info, size_t size, void *data)
{
/*	
	if (info->dlpi_addr && info->dlpi_name)
		sys_dbg("%p name=%s", info->dlpi_addr, info->dlpi_name);
*/
	void *dll = dlopen(info->dlpi_name, RTLD_GLOBAL);
	return 0;
}

static int
openssl_runtime_linkage(void)
{
	__openssl_sym_table_init();

	const char *v = sym_SSLeay_version ? sym_SSLeay_version(0): NULL;
	sys_dbg("checking for OpenSSL Version                        : %s", 
	        v ? v : "No");
	if (!v)
		return 1;

	int rfc_5878 = sym_SSL_CTX_add_server_custom_ext &&
	               sym_SSL_CTX_add_client_custom_ext;

	sys_dbg("checking for TLS Keying Material Exporter [RFC 5705]: %s",
	        sym_SSL_export_keying_material ? "Yes" : "No");
	sys_dbg("checking for TLS Authorization Extension  [RFC 5878]: %s",
		rfc_5878 ? "Yes" : "No");
	sys_dbg("checking for TLS Supplemental Data        [RFC 4678]: %s",
		rfc_5878 ? "Yes" : "No");

	return (int)(v == NULL);	
}

void
tls_openssl_init(void)
{
#ifndef CONFIG_DARWIN
	int e;

	dl_iterate_phdr(sym_module_callback, NULL);
	if (openssl_runtime_linkage())
		return;

        dlsym_SSL_CTX_new = (struct dlsym_struct) {
		.orig = sym_SSL_CTX_new, .wrap = sym_SSL_CTX_new_wrap
	};

	e = dlsym_wrap(&dlsym_SSL_CTX_new);


	dlsym_SSL_new = (struct dlsym_struct) {
		.orig = sym_SSL_new, .wrap = sym_SSL_new_wrap
	};

	e = dlsym_wrap(&dlsym_SSL_new);

	dlsym_SSL_free = (struct dlsym_struct) {
		.orig = sym_SSL_free, 
		.wrap = sym_SSL_free_wrap
	};
	e = dlsym_wrap(&dlsym_SSL_free);

	dlsym_SSL_set_ex_data = (struct dlsym_struct) {
		.orig = sym_SSL_set_ex_data, 
		.wrap = sym_SSL_set_ex_data_wrap
	};
	e = dlsym_wrap(&dlsym_SSL_set_ex_data);

	dlsym_SSL_get_ex_data = (struct dlsym_struct) {
		.orig = sym_SSL_get_ex_data, 
		.wrap = sym_SSL_get_ex_data_wrap
	};
	e = dlsym_wrap(&dlsym_SSL_get_ex_data);

	dlsym_SSL_CTX_set_info_callback = (struct dlsym_struct) {
		.orig = sym_SSL_CTX_set_info_callback,
		.wrap = sym_SSL_CTX_set_info_callback_wrap
	};

	e = dlsym_wrap(&dlsym_SSL_CTX_set_info_callback);
	sys_dbg("checking for TLS Handshake Signalling               : %s",
	        e ? "No" : "Yes");
/*
	dlsym_SSL_set_info_callback = (struct dlsym_struct) {
		.orig = sym_SSL_set_info_callback,
		.wrap = sym_SSL_set_info_callback_wrap};

	e = dlsym_wrap(&dlsym_SSL_set_info_callback);
*/
#endif
}

void
tls_openssl_fini(void)
{
}
