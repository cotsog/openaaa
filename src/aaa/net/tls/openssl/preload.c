/*
 *  (c) 2013                                    Daniel Kubec <niel@rtfm.cz>
 *  
 *  This software may be freely distributed and used according to the terms
 *  of the GNU Lesser General Public License.
 */

#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

#include <sys/lib.h>
#include <sys/mem/page.h>
#include <sys/mem/map.h>
#include <sys/mem/stack.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

#include "tls.h"

#define tls_aaa_call_enter \
	if (tls_init) goto tls_aaa_exit

#define tls_aaa_call_leave tls_aaa_exit:

#define TLS_RFC5705_SECRET_BITS (20 * 8)
#define TLS_RFC5705_LABEL "EXPERIMENTAL"

#define DECLARE_CALL(fn, rv, args...) \
	rv (*openssl_##fn)(args)
#define DECLARE_LINK(fn) \
	openssl_##fn = dlsym(RTLD_NEXT, #fn); \
//	prints("call: %s %p %p\n", #fn, openssl_##fn, fn)

#define TRACE_CALL prints("%s\n", __func__)
#define HOOK_CALL_INIT prints("%s init\n", __func__)
#define HOOK_CALL_FINI prints("%s fini\n", __func__)

#define prints(args...) fprintf (stdout, args)

/* SSL constructor */
DECLARE_CALL(SSL_library_init, int, void);
/* SSL context API */
DECLARE_CALL(SSL_CTX_new, SSL_CTX *, const SSL_METHOD *);
DECLARE_CALL(SSL_CTX_free, void, SSL_CTX *);
DECLARE_CALL(SSL_CTX_set_generate_session_id, int, SSL_CTX *ctx, GEN_SESSION_CB cb);
DECLARE_CALL(SSL_CTX_get_ex_data, void *, const SSL_CTX *, int );
DECLARE_CALL(SSL_CTX_set_ex_data, int, SSL_CTX *, int, void * );
DECLARE_CALL(SSL_CTX_set_msg_callback, void, SSL_CTX *, void (*callback)());
DECLARE_CALL(SSL_CTX_set_info_callback, void, SSL_CTX *, void (*callback)());

/* SSL session API */
DECLARE_CALL(SSL_new, SSL*, SSL_CTX *);
DECLARE_CALL(SSL_free, void, SSL *);
DECLARE_CALL(SSL_set_generate_session_id, int, SSL *, GEN_SESSION_CB cb);
DECLARE_CALL(SSL_get_ex_data, void *, const SSL *, int idx);
DECLARE_CALL(SSL_set_ex_data, int, SSL *, int idx, void *);
DECLARE_CALL(SSL_export_keying_material, int, SSL*, unsigned char *, size_t,  
             const char*, size_t ,  const unsigned char*, size_t , int);

#ifndef CONFIG_WINDOWS
DEFINE_SYMBOL_TABLE_BEGIN
	DECLARE_LINK(SSL_library_init);
	DECLARE_LINK(SSL_CTX_new);
	DECLARE_LINK(SSL_CTX_free);
	DECLARE_LINK(SSL_CTX_set_generate_session_id);
	DECLARE_LINK(SSL_CTX_get_ex_data);
	DECLARE_LINK(SSL_CTX_set_ex_data);
	DECLARE_LINK(SSL_CTX_set_msg_callback);
	DECLARE_LINK(SSL_CTX_set_info_callback);
	DECLARE_LINK(SSL_new);
	DECLARE_LINK(SSL_free);
	DECLARE_LINK(SSL_set_generate_session_id);
	DECLARE_LINK(SSL_set_ex_data);
	DECLARE_LINK(SSL_get_ex_data);
	DECLARE_LINK(SSL_export_keying_material);
DEFINE_SYMBOL_TABLE_END
#endif

int
SSL_library_init(void)
{
#ifndef CONFIG_WINDOWS
	init_symbols();
#endif
	return openssl_SSL_library_init();
}

struct ssl_ctxt {
	void (*handler_info)(const SSL *, int , int );
	int base;
	void *user;
};

struct ssl_conn {
	int base;
	void *user;
};

void
ssl_error_details(const char *method, const SSL *ssl)
{
	/*
	char buf[1024];
	int code;
	while((code = ERR_get_error())) {
		ERR_error_string_n(code, buf, sizeof(buf) - 1);
		prints("%s", buf);
	}
	*/
}

const char *
ssl_get_value_desc(const SSL *ssl, int val)
{
	/*
	switch (SSL_get_error(ssl, val)) {
	case SSL_ERROR_WANT_READ:
		return "(SSL_WANT_READ)";
	case SSL_ERROR_WANT_WRITE:
		return "(SSL_WANT_WRITE)";
	case SSL_ERROR_ZERO_RETURN:
		return "(SSL_RETURN)";
	case SSL_ERROR_WANT_X509_LOOKUP:
		return "(SSL_WANT_X509_LOOKUP)";
	default:
		ssl_error_details("CSSLManager::SSLDecodeReturnValue", ssl);
	break;
	}
*/
	return NULL;
}

static void
ssl_handler_handshake(const SSL *ssl)
{
	/*
	 * The output is a pseudorandom bit string of length bytes generated
	 * from the master_secret.  (This construction allows for
	 * interoperability with older exporter-type constructions which do not
	 * use context values, e.g., [RFC5281]).
	 * unsigned char key[4096] = {0};
	 */

	size_t size = TLS_RFC5705_SECRET_BITS / 8;
	unsigned char *sec = alloca(size);

	/*
	 * Labels here have the same definition as in TLS, i.e., an ASCII string
	 * with no terminating NULL.  Label values beginning with "EXPERIMENTAL"
	 * MAY be used for private use without registration.  All other label
	 * values MUST be registered via Specification Required as described by
	 * RFC 5226 [RFC5226].  Note that exporter labels have the potential to
	 * collide with existing PRF labels.  In order to prevent this, labels
	 * SHOULD begin with "EXPORTER".  This is not a MUST because there are
	 * existing uses that have labels which do not begin with this prefix.
	 */

	const char lab[] = TLS_RFC5705_LABEL;

	/*
	 * SSL_export_keying_material exports a value derived from the master secret,
	 * as specified in RFC 5705. It writes |olen| bytes to |out| given a label and
	 * optional context. (Since a zero length context is allowed, the |use_context|
	 * flag controls whether a context is included.)
	 *
	 * It returns 1 on success and zero otherwise.
	 */

	SSL_export_keying_material((SSL *)ssl, sec, size, lab, strlen(lab),  NULL, 0, 0);

    int reused = SSL_session_reused((SSL *)ssl);
    prints("msg ssl:%s\n", reused ? "session reused" : "handshake");

    SSL_SESSION *sess = SSL_get_session(ssl);
    unsigned int len = 0;
    const unsigned char *idb = SSL_SESSION_get_id(sess, &len);

	BIO *bio;
	char *buf;

	if (idb && len) {
	bio = BIO_new(BIO_s_mem());
	BIO_printf(bio, "attr ssl.sess.id: ");
	for (int i = 0; i < len; i++)
		BIO_printf(bio, "%02X", idb[i]);
	BIO_printf(bio, "\n");
	len = BIO_pending(bio);
	buf = (char *)alloca(len + 1);
	len = BIO_read(bio, buf, len);
	buf[len] = 0;
	prints("%s", buf);
	BIO_free(bio);
	}

	if (reused) return;

	prints("attr ssl.rfc5705.label: %s\n", lab);

    bio = BIO_new(BIO_s_mem()); 
    BIO_printf(bio, "attr ssl.rfc5705.secret: ");
    for (int i = 0; i < size; i++)
        BIO_printf(bio, "%02X", sec[i]);
    BIO_printf(bio, "\n");
    len = BIO_pending(bio);
    buf = (char *)alloca(len + 1);
    len = BIO_read(bio, buf, len);
    buf[len] = 0;	
    prints("%s", buf);
    BIO_free(bio);

    prints("attr ssl.sess.created: %ld\n", SSL_SESSION_get_time(sess));
    prints("attr ssl.sess.modified: %ld\n", SSL_SESSION_get_time(sess));
    prints("attr ssl.sess.expires: %ld\n", SSL_SESSION_get_timeout(sess));
}

static void
ssl_handler_info(const SSL *s, int where, int ret)
{
	SSL_CTX *ctx = s->ctx;
	const char *str, *err, *cb, *desc;
	int w = where & ~SSL_ST_MASK;
	int rv = ret;


	str = (w & SSL_ST_CONNECT)     ? "connect" :
	      (w & SSL_ST_ACCEPT)      ? "accept"  :
	      (w & SSL_ST_INIT)        ? "init" :
	      (w & SSL_ST_BEFORE)      ? "before" :
	      (w & SSL_ST_OK)          ? "ok" :
	      (w & SSL_ST_RENEGOTIATE) ? "renegotiate" : "report";

	if (where & SSL_CB_HANDSHAKE_DONE) {
		desc = stk_printf("%s:%s", str, SSL_state_string_long(s));
		prints("msg ssl:%s\n", desc);
		goto next;
	}

	if (where & SSL_CB_LOOP) {
		desc = stk_printf("%s:%s", str, SSL_state_string_long(s));
		prints("msg ssl:%s\n", desc);
		goto next;
	} else if (where & SSL_CB_ALERT) {  
		desc = stk_printf("alert %s:%s:%s", (where & SSL_CB_READ) ? 
				  "read" : "write",
		SSL_alert_type_string_long(rv), SSL_alert_desc_string_long(rv));
		prints("msg ssl:%s\n", desc);
		goto next;
	} else if (where & SSL_CB_EXIT) {  
		prints("msg ssl:exit %d\n", rv);
		if (rv == 0) {
			err = stk_printf("%s:failed in %s", str, 
					  SSL_state_string_long(s));
			const char *desc = ssl_get_value_desc(s, rv);
			prints("msg ssl:%s %s\n", err, desc);
			goto next;
		} else if (rv < 0) {
			switch(SSL_get_error(s, rv)) {
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				desc = stk_printf("%s:%s", str, SSL_state_string_long(s));

				prints("msg ssl:%s\n", desc);
			break;
			default:
				err = stk_printf("%s:error in %s", str, SSL_state_string_long(s));
				const char *desc = ssl_get_value_desc(s, rv);
				prints("msg ssl:%s %s\n", err, desc);
			break;
			}
		} 
	}
next:
	if (where & SSL_CB_HANDSHAKE_DONE)
		ssl_handler_handshake(s);

	struct ssl_ctxt *ssl_ctxt = openssl_SSL_CTX_get_ex_data(ctx, 0);
	if (!ssl_ctxt->handler_info)
		return;

	ssl_ctxt->handler_info(s, where, ret);
}

void *
SSL_CTX_get_ex_data(const SSL_CTX *s, int idx)
{
	init_symbols();
	return openssl_SSL_CTX_get_ex_data(s, idx + 1);
}

int
SSL_CTX_set_ex_data(SSL_CTX *ctx, int idx, void *data)
{
	init_symbols();
	return openssl_SSL_CTX_set_ex_data(ctx, idx + 1, data);
}

SSL_CTX *
SSL_CTX_new(const SSL_METHOD *method) 
{
	init_symbols();

	SSL_CTX *ctx = openssl_SSL_CTX_new(method);

	struct ssl_ctxt *ssl_ctxt = malloc(sizeof(*ssl_ctxt));
	//openssl_SSL_CTX_set_ex_data(ctx, 0, ssl_ctxt);
	//SSL_CTX_set_generate_session_id(ctx, generate_session_id);
	return ctx;
}

void
SSL_CTX_free(SSL_CTX *ctx)
{
	init_symbols();
	//struct ssl_ctxt *ssl_ctxt = openssl_SSL_CTX_get_ex_data(ctx, 0);
	//free(ssl_ctxt);
	openssl_SSL_CTX_free(ctx);
}

void *
SSL_get_ex_data(const SSL *ssl, int idx)
{
	init_symbols();
	return openssl_SSL_get_ex_data(ssl, idx + 1);
}

int
SSL_set_ex_data(SSL *ssl, int idx, void *user)
{
	init_symbols();
	return openssl_SSL_set_ex_data(ssl, idx + 1, user);
}

void
SSL_CTX_set_msg_callback(SSL_CTX *ctx, void (*cb)())
{
	init_symbols();
	openssl_SSL_CTX_set_msg_callback(ctx, cb);
}

void
SSL_CTX_set_info_callback(SSL_CTX *ctx, void (*callback)())
{
	init_symbols();

	struct ssl_ctxt *ssl_ctxt = openssl_SSL_CTX_get_ex_data(ctx, 0);
	ssl_ctxt->handler_info = callback;
	openssl_SSL_CTX_set_info_callback(ctx, ssl_handler_info);
}

SSL*
SSL_new(SSL_CTX *ctx)
{
	init_symbols();

	SSL *ssl = openssl_SSL_new(ctx);
	struct ssl_conn *ssl_conn = malloc(sizeof(*ssl_conn));
	openssl_SSL_set_ex_data(ssl, 0, (char *)ssl_conn);

	return ssl;
}

void
SSL_free(SSL *ssl)
{
	struct ssl_conn *ssl_conn = (struct ssl_conn *)
	                            openssl_SSL_get_ex_data(ssl, 0);
	free(ssl_conn);
	openssl_SSL_free(ssl);
}
