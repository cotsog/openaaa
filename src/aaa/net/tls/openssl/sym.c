/*
 * $id: openssl/sym.c                              Daniel Kubec <niel@rtfm.cz>
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
#include <unistd.h>
#include <dlwrap.h>

void
__openssl_sym_table_init(void)
{
        sym_link(SSLeay);
        sym_link(SSLeay_version);
        sym_link(SSL_CTX_new);
        sym_link(SSL_CTX_free);
        sym_link(SSL_new);
        sym_link(SSL_free);
        sym_link(SSL_get_SSL_CTX);
        sym_link(SSL_set_ex_data);
        sym_link(SSL_get_ex_data);
        sym_link(SSL_CTX_set_info_callback);
	sym_link(SSL_set_info_callback);
        sym_link(SSL_export_keying_material);
        sym_link(SSL_state_string);
        sym_link(SSL_state_string_long);
        sym_link(SSL_alert_type_string);
        sym_link(SSL_alert_type_string_long);
        sym_link(SSL_alert_desc_string);
        sym_link(SSL_alert_desc_string_long);
        sym_link(SSL_get_error);
        sym_link(SSL_get_session);
        sym_link(SSL_SESSION_free);
        sym_link(SSL_SESSION_get_id);
        sym_link(SSL_SESSION_print);
        sym_link(BIO_new);
        sym_link(BIO_free);
        sym_link(BIO_s_mem);
        sym_link(BIO_pending);
        sym_link(BIO_read);
        sym_link(BIO_ctrl);
        sym_link(PEM_write_bio_PUBKEY);
        sym_link(SSL_callback_ctrl);
        sym_link(SSL_CTX_add_client_custom_ext);
        sym_link(SSL_CTX_get_cert_store);
        sym_link(SSL_get_peer_certificate);
        sym_link(SSL_get_certificate);
	sym_link(SSL_extension_supported);
	sym_link(SSL_CTX_add_server_custom_ext);
	sym_link(SSL_CTX_add_client_custom_ext);
        sym_link(X509_free);
        sym_link(OPENSSL_free);
        sym_link(X509_NAME_oneline);
        sym_link(X509_get_subject_name);
        sym_link(X509_get_issuer_name);
        sym_link(X509_get_pubkey);
        sym_link(EVP_PKEY_type);
        sym_link(EVP_PKEY_size);
        sym_link(EVP_PKEY_bits);
        sym_link(EVP_PKEY_free);	
}
