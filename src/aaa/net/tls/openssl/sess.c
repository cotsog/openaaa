/*
 * $id: openssl/sess.c                              Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <sys/compiler.h>
#include <sys/cpu.h>

#include <aaa/net/tls/lib.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

void
tls_session_print(struct tls_session *tls_session)
{
/*	
	SSL *ssl = (SSL *)tls_session->handle;
	BIO *bio = BIO_new(BIO_s_mem());

	SSL_SESSION *sess = SSL_get_session(ssl);
	SSL_SESSION_print(bio, sess);

	int len = BIO_pending(bio);

	char *buf = alloca(len + 1);
	BIO_read(bio, buf, len);
	buf[len] = 0;

	printf("%s", buf);

	BIO_free(bio);
*/	
}
