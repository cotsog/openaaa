
#include <sys/compiler.h>
#include <sys/missing.h>

#include <openssl/opensslv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>

#include <aaa/lib.h>
#include <aaa/private.h>
#include <aaa/net/tls/lib.h>
#include <aaa/net/tls/openssl/sym.h>

void SSL_TLSEXT_TEST_client_init(SSL * s) 
{
/*	
	TLSEXT_GENERAL * e;

	e = SSL_TLSEXT_GENERAL_new(s, 65000);
	SSL_TLSEXT_GENERAL_client_data(e, 8, "test-c1\0");
	SSL_TLSEXT_GENERAL_client_ext_cb(e, ssl_tlsext_test_client_ext_cb);
*/
	/* the server will send supplemental data */
/*	
	SSL_TLSEXT_GENERAL_server_supp_data_new(e, 65100);
	SSL_TLSEXT_GENERAL_server_supp_data_new(e, 65101);
	SSL_TLSEXT_GENERAL_client_finish_cb(e, ssl_tlsext_test_client_finish_cb);
*/	
}
