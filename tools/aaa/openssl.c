/*
 * $openssl.c                                       Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/lib.h>

#include <aaa/lib.h>
#include <aaa/net/tls/lib.h>
#include <aaa/net/vpn/lib.h>

#include <openssl/opensslv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

void
openssl_test(void)
{
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	aaa_init();
	aaa_tls_init();

	SSL_CTX *ctx = SSL_CTX_new(SSLv3_client_method);

	if (ctx)
		SSL_CTX_free(ctx);

	aaa_fini();
}
