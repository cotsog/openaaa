#ifndef __LIBAAA_TLS_OPENSSL_SSL_H__
#define __LIBAAA_TLS_OPENSSL_SSL_H__

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

__BEGIN_DECLS

extern void
(*openssl_info_callback)(SSL *s, int, int);

int
sym_SSL_set_ex_data_wrap(SSL *ssl, int idx, void *user);

void *
sym_SSL_get_ex_data_wrap(SSL *ssl, int idx);

void
openssl_handler_info(SSL *s, int where, int ret);

void
openssl_tlsext_cb(SSL *s, int client_server, int type,
                  unsigned char *data, int len, void *arg);

/* disable compression */
void 
ssl_disable_compression(void);

SSL *
ssl_x509_get_ssl_from_store(X509_STORE_CTX *store);

int
tls_openssl_get_keys(SSL *ssl, struct tls_keys *kets);

__END_DECLS

#endif
