
#include <sys/compiler.h>
#include <sys/compiler.h>
#include <sys/missing.h>

#include <ctypes/lib.h>
#include <ctypes/string.h>
#include <ctypes/b64.h>
#include <mem/stack.h>

#include <openssl/opensslv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

#include <hash/fn.h>

#include <aaa/lib.h>
#include <aaa/private.h>
#include <aaa/net/tls/lib.h>
#include <aaa/net/tls/openssl/sym.h>

#include <aaa/net/tls/lib.h>
#include <aaa/net/tls/openssl/sym.h>
#include <aaa/net/tls/openssl/ssl.h>

#include <crypto/sha1.h>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dlwrap.h>

static const char *__tls_ext_names[] = {
	[TLSEXT_TYPE_renegotiate]            = "renegotiate",
	[TLSEXT_TYPE_heartbeat]              = "heartbeat",      
	[TLSEXT_TYPE_ec_point_formats]       = "ec_point_formats",
	[TLSEXT_TYPE_server_name]            = "server_name",
	[TLSEXT_TYPE_max_fragment_length]    = "max_fragmet_length",
	[TLSEXT_TYPE_client_certificate_url] = "client_certificate_url",
	[TLSEXT_TYPE_trusted_ca_keys]        = "trusted_ca_keys",
	[TLSEXT_TYPE_truncated_hmac]         = "truncated_hmac",
	[TLSEXT_TYPE_status_request]         = "status_request",
	[TLSEXT_TYPE_user_mapping]           = "user_mapping",
	[TLSEXT_TYPE_client_authz]           = "client_authz",
	[TLSEXT_TYPE_server_authz]           = "server_authz",
	[TLSEXT_TYPE_cert_type]              = "cert_type",
	[TLSEXT_TYPE_elliptic_curves]        = "elliptic_curves",
	[TLSEXT_TYPE_srp]                    = "srp",
	[TLSEXT_TYPE_signature_algorithms]   = "signature_algorithms",
	[TLSEXT_TYPE_use_srtp]               = "use_srtp",
	[TLSEXT_TYPE_session_ticket]         = "session_ticket",
	[TLSEXT_TYPE_next_proto_neg]         = "next_proto_neg",
	[TLSEXT_TYPE_alpn]                   = "alpn",
	[TLSEXT_TYPE_channel_id]             = "channel_id",
	[TLSEXT_TYPE_extended_master_secret] = "extended_master_secret",
	[TLSEXT_TYPE_signed_certificate_timestamp] = "signed certtificate timestamp",
	[TLSEXT_TYPE_supplemental_data]      = "supplemental_data"
};

#define __ssl_get_session(ssl)\
	(struct tls_session *)sym_SSL_get_ex_data_wrap(ssl, ~0U);

void (*openssl_info_callback)  (SSL *s, int, int) = NULL;
void (*openssl_tlsext_callback)(SSL *s, int client_server, 
                                int type, unsigned char *data, 
                                int len, void *arg) = NULL;

void
openssl_tlsext_cb(SSL *ssl, int client, int type,
                  unsigned char *data, int len, void *arg)
{ 
	struct tls_session *tls = __ssl_get_session(ssl);
	tls->endpoint = client ? TLS_EP_CLIENT : TLS_EP_SERVER;

	sys_dbg("openaaa:msg:ssl:extension name=%s type=%d, len=%d endpoint=%d", 
	        __tls_ext_names[type], type, len, client);
}

#define stk_strmem(addr, size)                                                \
({                                                                            \
	char *_s = (char *)addr; unsigned int _l = size + 1;                  \
	char *_x = alloca(_l); memcpy(_x, _s, _l); _x[size] = 0; _x;          \
})

int
ssl_ext_srv_add(SSL *s, unsigned int type,
                const unsigned char **out, size_t *outlen, int *al, void *arg)
{
	const char *authority = getenv("OPENAAA_AUTHORITY");

	sys_dbg("openaaa:msg:ssl:extension name=%s type=%d send [%s]",
	        __tls_ext_names[type], type, authority);	

	*out = authority;
	*outlen = authority ? strlen(authority) : 0;
	return 1;
}

int
ssl_ext_srv_parse(SSL *s, unsigned int type,
                  const unsigned char *in, size_t inlen, int *al, void *arg)
{
	const char *v = stk_strmem(in, inlen);
	sys_dbg("openaaa:msg:ssl:extension name=%s type=%d recv [%s]",
	        __tls_ext_names[type], type, v);
	return 1;
}

int
ssl_ext_cli_add(SSL *s, unsigned int type,
                const unsigned char **out, size_t *outlen, int *al, void *arg)
{
	const char *v = "protocol=aaa";
	sys_dbg("openaaa:msg:ssl:extension name=%s type=%d send [%s]",
	        __tls_ext_names[type], type,v); 

	*out = v;
	*outlen = strlen(v);
	return 1;
}

int
ssl_ext_cli_parse(SSL *s, unsigned int type,
                  const unsigned char *in, size_t inlen, int *al, void *arg)
{
	const char *v = stk_strmem(in, inlen);
	sys_dbg("openaaa:msg:ssl:extension name=%s type=%d recv [%s]",
		__tls_ext_names[type], type, v);

	if (type==1000)
		tls_set_authority(v);
	return 1;
}

static inline void
pubkey_fixups(char *str, unsigned int len)
{
	char *p = str;
	for (unsigned int i = 0; i < len && *p; i++, p++)
		if (*p == '\r' || *p == ' ' || *p == '\t')
			continue;
		else
			*str++ = *p;
	*str = 0;
}

static inline void
pubkey_derive_key(struct tls_session *tls, X509 *x)
{
	byte pub[8192];
        EVP_PKEY *key = sym_X509_get_pubkey(x);
        if (!key)
		return;

	int size = sym_EVP_PKEY_size(key);
	int bits = sym_EVP_PKEY_bits(key);

	BIO *bio = sym_BIO_new(sym_BIO_s_mem());
	sym_PEM_write_bio_PUBKEY(bio, key);

	BUF_MEM *bptr;
	sym_BIO_ctrl(bio, BIO_C_GET_BUF_MEM_PTR, 0,(char *)&bptr);

	if (bptr->length > (sizeof(pub) - 1))
		goto cleanup;

	memcpy(pub, bptr->data, bptr->length);
	pub[bptr->length] = 0;

	pubkey_fixups((char *)pub, bptr->length);
	int hash = hash_string((char *)pub);

        sha1_context sha1;
        sha1_init(&sha1);
        sha1_update(&sha1, (byte *)pub, bptr->length);
        sha1_update(&sha1, (byte *)tls->key, tls->key_size);

        memcpy(tls->sec, (char *)sha1_final(&sha1), SHA1_SIZE);
	tls->sec_size = SHA1_SIZE;
        char *sec = stk_mem_to_hex(tls->sec, SHA1_SIZE);	

	sys_dbg("checking for server public key: len=%d size=%d bits=%d hash=%d", 
	        bptr->length, size, bits, hash);

	sys_dbg("checking for derived binding key: aaa_binding_key=%s", sec);

cleanup:
	sym_BIO_free(bio);
	sym_EVP_PKEY_free(key);
}

static void
certificate_info(X509 *x)
{
	char *subject = sym_X509_NAME_oneline(sym_X509_get_subject_name(x), NULL, 0);
	char *issuer  = sym_X509_NAME_oneline(sym_X509_get_issuer_name(x), NULL, 0);
	sys_dbg("checking for subject: %s", subject);
	sys_dbg("checking for issuer:  %s", issuer);
}

static inline int
derive_client_binding_key(struct tls_session *tls)
{
	X509 *x = sym_SSL_get_peer_certificate((SSL *)tls->ssl);
	sys_dbg("checking for server certificate: %s", x ? "Yes" : "No");
	if (x == NULL)
		return 1;

	certificate_info(x);
	pubkey_derive_key(tls, x);
	return 0;
}	

static inline int
derive_server_binding_key(struct tls_session *tls)
{
	X509 *x = sym_SSL_get_certificate((SSL *)tls->ssl);
	sys_dbg("checking for server certificate: %s", x ? "Yes" : "No");
	if (x == NULL)
		return 1;

	certificate_info(x);
	pubkey_derive_key(tls, x);
	return 0;
}

static inline int
tls_export_keying_material(struct tls_session *tls)
{
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

	struct tls_ekm *ekm = tls_get_ekm_attrs();
	tls->key_size = ekm->size;

        /*
	 * The output is a pseudorandom bit string of length bytes generated
	 * from the master_secret.  (This construction allows for
	 * interoperability with older exporter-type constructions which do not
	 * use context values, e.g., [RFC5281]).
	 * unsigned char key[4096] = {0};
	 */

        if (sym_SSL_export_keying_material((SSL *)tls->ssl, 
	                                   tls->key, tls->key_size, 
	                                   ekm->label, ekm->label_size, 
					   NULL, 0, 0) == 0)
		return 1;

	const char *key = stk_mem_to_hex(tls->key, tls->key_size);
	sys_dbg("checking for exported key: tls_binding_key=%s", key);
	return 0;
}

static inline const char *
openssl_endpoint_str(int type)
{
	return type == TLS_EP_CLIENT ? "client":
	       type == TLS_EP_SERVER ? "server": "undefined";
}

void
openssl_handshake_handler(SSL *ssl)
{
	struct tls_session *tls = __ssl_get_session(ssl);
	const char *authority = tls_get_authority();
	const char *handler   = tls_get_handler();
        const char *endpoint  = openssl_endpoint_str(tls->endpoint);

	sys_dbg("openaaa:msg:ssl:handshake done endpoint type=%s", endpoint);

	if (!authority || !handler)
		return;

	if (tls_export_keying_material(tls))
		return;

	switch (tls->endpoint) {
	case TLS_EP_CLIENT:
		if (derive_client_binding_key(tls))
			return;
		break;
	case TLS_EP_SERVER:
		if (derive_server_binding_key(tls))
			return;
		break;
	default:
		return;
	}

	sha1_context sha1;
	byte *hash;

	char *key = stk_mem_to_hex(tls->key, tls->key_size);

	byte enkey[(tls->key_size * 3) + 1];
	memset(enkey, 0, sizeof(enkey));
	b64_enc(enkey, (byte *)tls->key, tls->key_size);

	sha1_init(&sha1);
	sha1_update(&sha1, (byte *)key, strlen(key));
	hash = sha1_final(&sha1);
	char *id = stk_mem_to_hex((char *)hash, SHA1_SIZE / 2);

        byte enid[(tls->key_size * 3) + 1];
        memset(enid, 0, sizeof(enid));
        b64_enc(enid, (byte *)hash, SHA1_SIZE / 2);

        sha1_init(&sha1);
        sha1_update(&sha1, (byte *)hash, SHA1_SIZE / 2);
        hash = sha1_final(&sha1);

        byte entype[(tls->key_size * 3) + 1];
        memset(entype, 0, sizeof(entype));
	b64_enc(entype, (byte *)hash, SHA1_SIZE / 2);

	char *uri_type= url_encode((char *)entype);
	char *uri_id  = url_encode((char *)enid);
	char *uri_key = url_encode((char *)enkey);

	sys_dbg("checking for tls_binding_id:  %s", id);
	sys_dbg("checking for tls_binding_key: %s", key);
	sys_dbg("checking for authority: %s", authority);

	sys_dbg("auth_id: %s", enid);
        sys_dbg("binding_id: %s", entype);
        sys_dbg("binding_key: %s", enkey);

	char *r3 = stk_printf("http%%3A%%2F%%2F%s%%2FAIM%%2Fservices%%2FR3", authority);

	char *uri = stk_printf("aducid://callback?authId=%s&r3Url=%s&bindingId=%s&bindingKey=%s",
	                             uri_id, r3, uri_type, uri_key);

	free(uri_id);
	free(uri_key);
	free(uri_type);

	if (tls->endpoint == TLS_EP_CLIENT) {
		int status;

 		const char *msg;
		msg = stk_printf("aducid.exe -k%s -i%s -prx -a%s -c", 
		                 key, id, authority);
		status = system(msg);
		sys_dbg("[%.4d] %s", WEXITSTATUS(status), msg);

		if (WEXITSTATUS(status) == 0) {
			msg = stk_printf("START /B aducid.exe -k%s -i%s -prx -a%s", key, id, authority);
			status = system(msg);
			sys_dbg("[%.4d] %s", WEXITSTATUS(status), msg);
			return;
		}
#ifdef CONFIG_WINDOWS
		status = system(stk_printf("START /B %s -t20 \"%s\"", handler, uri));
#else
		status = system(stk_printf("%s -t20 \"%s\" &", handler, uri));
#endif
		sys_dbg("[%.4d] %s %s", WEXITSTATUS(status), handler, uri);
	}

cleanup:
	return;
}

const char *
ssl_get_value_desc(SSL *s, int code)
{
	return NULL;
}

static void
info_handler(SSL *s, const char *str)
{
	char *desc = stk_printf("%s:%s", str, sym_SSL_state_string_long(s));
	sys_dbg("openaaa:msg:ssl:%s", desc);
}

static void
info_handler_alert(int where, int rv)
{
	char *desc = stk_printf("alert %s:%s:%s", (where & SSL_CB_READ) ?
	                        "read" : "write",
	                        sym_SSL_alert_type_string_long(rv),
	                        sym_SSL_alert_desc_string_long(rv));

	sys_dbg("openaaa:msg:ssl:%s", desc);		
}

static void
info_handler_exit_failed(SSL *s, const char *str, int rv)
{
	char *err = stk_printf("%s:failed in %s", str,
	                       sym_SSL_state_string_long(s));

	const char *desc = ssl_get_value_desc(s, rv);
	sys_dbg("openaaa:msg:ssl:%s %s", err, desc);
}

static void
info_handler_default(SSL *s, const char *str, int rv)
{
	char *err = stk_printf("%s:error in %s", str, 
	                       sym_SSL_state_string_long(s));

	const char *desc = ssl_get_value_desc(s, rv);
	sys_dbg("openaaa:msg:ssl:%s %s", err, desc);
}

static void
info_handler_exit_error(SSL *s, const char *str, int rv)
{
	switch(sym_SSL_get_error(s, rv)) {
	case SSL_ERROR_WANT_READ:
	case SSL_ERROR_WANT_WRITE:
	break;
	default:
		info_handler_default(s, str, rv);
	break;
	}
}


void
openssl_info_handler(SSL *s, int where, int ret)
{
        const char *str, *err, *cb, *desc;
        int w = where & ~SSL_ST_MASK;
        int rv = ret;

        str = (w & SSL_ST_CONNECT)     ? "connect" :
	      (w & SSL_ST_ACCEPT)      ? "accept"  :
	      (w & SSL_ST_INIT)        ? "init" :
	      (w & SSL_ST_BEFORE)      ? "before" :
	      (w & SSL_ST_OK)          ? "ok" :
	      (w & SSL_ST_RENEGOTIATE) ? "renegotiate" : "negotiate";

	if (where & SSL_CB_HANDSHAKE_DONE) {
		info_handler(s, str);
		goto next;
	}

        if (where & SSL_CB_LOOP) {
		info_handler(s, str);
		goto next;
	} else if (where & SSL_CB_ALERT) {
		info_handler_alert(where, rv);
		goto next;
	} else if (where & SSL_CB_EXIT) {
		if (rv == 0) {
			info_handler_exit_failed(s, str, rv);
			goto next;
		} else if (rv < 0)
			info_handler_exit_error(s, str, rv);
	}

next:	

	if (where & SSL_CB_HANDSHAKE_DONE)
		openssl_handshake_handler(s);

	/* workarround recursive bug */
	if (openssl_info_callback == openssl_info_handler)
		return;

	if (!openssl_info_callback)
		return;

	openssl_info_callback(s, where, ret);
}
