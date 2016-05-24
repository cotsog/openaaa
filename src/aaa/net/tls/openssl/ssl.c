/*
 * (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <stdlib.h>
#include <stdbool.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

struct config_ssl_server {
	const char *ca;
	const char *cert;
	const char *key;
	const char *crl;
	bool verify;
	bool compression;
};

struct ssl_ctx {
	SSL_CTX *ctx;
};

struct ssl {
	SSL *ssl;
	int handshake;
};

struct ssl_session {
	int unused;
};

static struct config_ssl_server config_ssl_server = {
	.verify      = true,
	.compression = false
};

static void
ssl_handler_handshake(const SSL *ssl)
{
}

static void
ssl_handler_info(const SSL *s, int where, int ret)
{
        const char *str, *err, *cb;
        int w = where & ~SSL_ST_MASK;

        /* The following are the possible values for ssl->state are are
         * used to indicate where we are up to in the SSL connection establishment.
         */

        str = (w & SSL_ST_CONNECT)     ? "connect" :
              (w & SSL_ST_ACCEPT)      ? "accept"  :
              (w & SSL_ST_INIT)        ? "init" :
              (w & SSL_ST_BEFORE)      ? "before" :
              (w & SSL_ST_OK)          ? "ok" :
              (w & SSL_ST_RENEGOTIATE) ? "renegotiate" : "undefined";

        if (where & SSL_CB_HANDSHAKE_DONE)
                ssl_handler_handshake(s);
}

int ssl_context_init(struct ssl_ctx *ssl_ctx)
{
        SSL_load_error_strings();
        ERR_load_X509_strings();
        ERR_load_crypto_strings();

        SSL_library_init();

        SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
        SSL_CTX_set_info_callback(ctx, ssl_handler_info);

        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_algorithms();

	ssl_ctx->ctx = ctx;
}

void
ssl_context_fini(struct ssl_ctx *ssl_ctx)
{
}

int
ssl_accept_handshake(struct ssl *ssl, unsigned int timeout)
{
	return -1;
}

/*
 * openssl compression bug workarround by Tomas Horacek <tomas.horacek@unicorn.eu>
 */

void
ssl_disable_compression(void)
{
       STACK_OF(SSL_COMP) *ssl_comp_methods = SSL_COMP_get_compression_methods();
       for (int i = 0, n = sk_SSL_COMP_num(ssl_comp_methods); i < n; i++)
		sk_SSL_COMP_delete(ssl_comp_methods, i);
}

/*
 * CRL - What is needed:
 *
 * Retrieve CRL URL from certificate to validate from CRL Distribution Points extension. 
 * OpenSSL provides certificate parsing functions but no simple accessor to CRL distribution points
 * Download CRL from URL. OpenSSL doesn't implement this, nor any form of caching.
 * Verify CRL (signature, issuer DN, validity period, subject key identifier, etc...). 
 * OpenSSL provides the different low-level functions.
 * Verify if the serial number of the certificate to check is in the CRL.
 *
 * Some additional details that might complicate things for a completely generic implementation:
 *
 * Some certificates might use OCSP instead of CRLs.
 * Some certificates have LDAP DNs or URLs as distribution points.
 * Some CRLs are signed by delegated CRL signer.
 * Delta-CRLs or partitioned CRLs might complicate implementation (especially w.r.t. caching).
 */

int
ssl_x509_lookup(X509_STORE *store, int type, X509_NAME *name, X509_OBJECT *obj)
{
        X509_STORE_CTX ctx;
        X509_STORE_CTX_init(&ctx, store, NULL, NULL);
        int rc = X509_STORE_get_by_subject(&ctx, type, name, obj);
        X509_STORE_CTX_cleanup(&ctx);
        return rc;
}
