#ifndef __LIBAAA_TLS_OPENSSL_SYM_H__
#define __LIBAAA_TLS_OPENSSL_SYM_H__

//#include <types/lib.h>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dlwrap.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

__BEGIN_DECLS

#define sym_define(fn, rv, args...) \
	rv ((*sym_##fn)(args)); \
	struct dlsym_struct dlsym_##fn

#define sym_link(fn) \
	sym_##fn = dlsym((void *)RTLD_DEFAULT, #fn);\

#ifndef TLSEXT_TYPE_supplemental_data
#define TLSEXT_TYPE_supplemental_data       1000
#endif

#ifndef TLSEXT_TYPE_signed_certificate_timestamp
#define TLSEXT_TYPE_signed_certificate_timestamp 18
#endif

#ifndef TLSEXT_TYPE_channel_id
#define TLSEXT_TYPE_channel_id              30032
#endif

#ifndef TLSEXT_TYPE_extended_master_secret
#define TLSEXT_TYPE_extended_master_secret  23
#endif

#ifndef TLSEXT_TYPE_signature_algorithms
#define TLSEXT_TYPE_signature_algorithms    13
#endif

#ifndef TLSEXT_TYPE_alpn
#define TLSEXT_TYPE_alpn                    16
#endif

void
__openssl_sym_table_init(void);

sym_define(SSLeay, long, void);
sym_define(SSLeay_version, const char *, int);
sym_define(SSL_CTX_new, SSL_CTX *, const SSL_METHOD *);
sym_define(SSL_CTX_free, void, SSL_CTX *);
sym_define(SSL_new, SSL *, SSL_CTX *);
sym_define(SSL_free, void, SSL *);
sym_define(SSL_session_reused, int, SSL *);

sym_define(SSL_set_ex_data, int, SSL *, int, void *);
sym_define(SSL_get_ex_data, void *, const SSL *, int);

sym_define(SSL_CTX_set_info_callback, void, SSL_CTX *ctx, void (*callback)());
sym_define(SSL_set_info_callback, void, SSL *ctx, void (*callback)());
sym_define(SSL_export_keying_material, int,
           SSL*, unsigned char *, size_t, const char*, size_t,
           const unsigned char*, size_t , int);

sym_define(SSL_state_string, const char *, const SSL *);
sym_define(SSL_state_string_long, const char *, const SSL *);

sym_define(SSL_alert_type_string, const char *, int value);
sym_define(SSL_alert_type_string_long, const char *, int value);
sym_define(SSL_alert_desc_string, const char *, int value);
sym_define(SSL_alert_desc_string_long, const char *, int value);

sym_define(SSL_get_error, int, const SSL *, int);

sym_define(SSL_get_session, SSL_SESSION *, const SSL *);
sym_define(SSL_SESSION_free, void, SSL_SESSION *);
sym_define(SSL_SESSION_get_id, const unsigned char *, 
                               SSL_SESSION *, unsigned int *);

sym_define(SSL_SESSION_print, int, BIO *, SSL_SESSION *);

sym_define(BIO_new, BIO *, BIO_METHOD *);
sym_define(BIO_free, int, BIO *);
sym_define(BIO_s_mem, BIO_METHOD *, void);
sym_define(BIO_pending, int, BIO *);
sym_define(BIO_read, int, BIO *, void *, int);
//sym_define(BIO_get_mem_ptr, void, BIO *, BUF_MEM **);

sym_define(X509_NAME_oneline, char *, void *, char *, int);

sym_define(SSL_get_ex_data_X509_STORE_CTX_idx, int, void);
sym_define(X509_STORE_CTX_get_ex_data, SSL *, X509_STORE_CTX *, int);

sym_define(SSL_num_renegotiations, int, SSL *);
sym_define(SSL_callback_ctrl, void, SSL *, int, void *);

sym_define(SSL_get_peer_certificate, X509 *, const SSL *);

sym_define(SSL_get_certificate, X509 *, const SSL *);

sym_define(SSL_get_SSL_CTX, SSL_CTX *, const SSL *);

sym_define(SSL_CTX_get_cert_store, X509_STORE *, SSL_CTX *);

sym_define(SSL_extension_supported, int, unsigned int ext_type);

typedef int (*ssl_ext_add_cb)(SSL *s, unsigned int ext_type,
const unsigned char **out, size_t *outlen, int *al, void *add_arg);

typedef void (*ssl_ext_free_cb)(SSL *s, unsigned int ext_type,
const unsigned char *out,void *add_arg);

typedef int (*ssl_ext_parse_cb)(SSL *s, unsigned int ext_type,
const unsigned char *in, size_t inlen, int *al, void *parse_arg);

sym_define(SSL_CTX_add_client_custom_ext, int, SSL_CTX *ctx, unsigned int,
ssl_ext_add_cb ,ssl_ext_free_cb, void *, ssl_ext_parse_cb, void *);

sym_define(SSL_CTX_add_server_custom_ext, int, SSL_CTX *ctx, unsigned int,
ssl_ext_add_cb, ssl_ext_free_cb, void *, ssl_ext_parse_cb, void *);

sym_define(X509_free, void, X509 *);
sym_define(OPENSSL_free, void, void *);
sym_define(X509_get_subject_name, void *, X509 *);
sym_define(X509_get_issuer_name, void *, X509 *);
sym_define(X509_get_pubkey, EVP_PKEY *, X509 *);

sym_define(EVP_PKEY_type, int, EVP_PKEY *);
sym_define(EVP_PKEY_size, int, EVP_PKEY *);
sym_define(EVP_PKEY_bits, int, EVP_PKEY *);
sym_define(EVP_PKEY_free, void, EVP_PKEY *);

sym_define(BIO_s_mem, BIO_METHOD *, void);
sym_define(BIO_new, BIO *, BIO_METHOD *);
sym_define(BIO_ctrl, long, BIO*, int, long, void*);

sym_define(PEM_write_bio_PUBKEY, int, BIO *bp, EVP_PKEY *);

/*
X509 *SSL_get_peer_certificate(const SSL *ssl);
const SSL_METHOD *SSL_CTX_get_ssl_method(SSL_CTX *ctx);                         
const SSL_METHOD *SSL_get_ssl_method(SSL *s);

SSL_set_tlsext_debug_callback(con, tlsext_cb);
SSL_set_tlsext_debug_arg(con, bio_c_out);
SSL_set_tlsext_status_type(con, TLSEXT_STATUSTYPE_ocsp);
SSL_CTX_set_tlsext_status_cb(ctx, ocsp_resp_cb);
SSL_CTX_set_tlsext_status_arg(ctx, bio_c_out);
*/

int
ssl_ext_cli_add(SSL *, unsigned int , const unsigned char **, 
                size_t *, int *, void *);

int
ssl_ext_cli_parse(SSL *, unsigned int,
                  const unsigned char *, size_t , int *, void *);

int
ssl_ext_srv_add(SSL *, unsigned int , const unsigned char **,
                  size_t *, int *, void *);

int
ssl_ext_srv_parse(SSL *, unsigned int,
                  const unsigned char *, size_t , int *, void *);

extern void 
(*openssl_info_callback)(SSL *s, int, int);

void
openssl_info_handler(SSL *s, int where, int ret);

SSL *
openssl_get_ssl_from_store_ctx(X509_STORE_CTX *store);

void
tls_session_debug(SSL *ssl);

__END_DECLS

#endif
