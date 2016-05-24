/*
 * $lib.h                                           Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __AAA_TLS_LIB_H__
#define __AAA_TLS_LIB_H__

#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/decls.h>

__BEGIN_DECLS
/* API version, they compare as integers */
#define API_VERSION PACKAGE_VERSION
/* Define TLS signals */
#define TLS_SIGNAL_NEGOTIATION                    (1<<1)
#define TLS_SIGNAL_NEGOTIATED                     (1<<2)
#define TLS_SIGNAL_RENEGOTIATION                  (1<<3)
#define TLS_SIGNAL_RENEGOTIATED                   (1<<4)
#define TLS_SIGNAL_AUTHENTICATION                 (1<<5)
#define TLS_SIGNAL_AUTHENTTICATED                 (1<<6)
/* RFC3546 / RFC4366 / RFC6066 */
#define TLS_EXTENSION_server_name                 0
#define TLS_EXTENSION_max_fragment_length         1
#define TLS_EXTENSION_client_certificate_url      2
#define TLS_EXTENSION_trusted_ca_keys             3
#define TLS_EXTENSION_truncated_hmac              4
#define TLS_EXTENSION_status_request              5
/* RFC4681 */
#define TLS_EXTENSION_user_mapping                6
/* RFC5878 */
#define TLS_EXTENSION_client_authz                7
#define TLS_EXTENSION_server_authz                8
/* RFC6091 */
#define TLS_EXTENSION_cert_type                   9
/* RFC4492 */
#define TLS_EXTENSION_elliptic_curves             10
#define TLS_EXTENSION_ec_point_formats            11
/* RFC5054 */
#define TLS_EXTENSION_srp                         12
/* RFC5246 */
#define TLS_EXTENSION_signature_algorithms        13
/* RFC5764 */
#define TLS_EXTENSION_use_srtp    14
/* RFC5620 */
#define TLS_EXTENSION_heartbeat   15
#define TLS_EXTENSION_application_layer_protocol_negotiation 16
#define TLS_EXTENSION_padding     21
/* RFC4507 */
#define TLS_EXTENSION_session_ticket              35
/* Temporary extension type */
#define TLS_EXTENSION_TYPE_renegotiate                 0xff01
#define TLS_EXTENSION_TYPE_next_proto_neg              13172
/* RFC 3546 */
#define TLS_EXTENSION_NAMETYPE_host_name 0
/* RFC 5246 */
#define TLS_EXTENSION_signature_anonymous                      0
#define TLS_EXTENSION_signature_rsa                            1
#define TLS_EXTENSION_signature_dsa                            2
#define TLS_EXTENSION_signature_ecdsa                          3
/* RFC 5878 */
#define TLS_EXTENSION_SUPPLEMENTALDATATYPE_authz_data 16386
#define TLS_EXTENSION_AUTHZDATAFORMAT_dtcp 66


#define TLS_KEY_MAX             40
#define TLS_EKM_LABEL_MAX       40
#define TLS_EKM_SIZE_MAX        40

#define TLS_KEY_SESSION         (1 << 1)
#define TLS_KEY_SECRET          (1 << 2)
#define TLS_KEY_BINDING         (1 << 3)

/* Define TLS attributes */
#define TLS_ATTR_EKM_LABEL      "tls.ekm.label"
#define TLS_ATTR_EKM_SIZE       "tls.ekm.size"
#define TLS_ATTR_AUTHORITY      "tls.authority"
#define TLS_ATTR_HANDLER        "tls.handler"

#define TLS_RFC5705_SECRET_BITS (8 * 16)
#define TLS_RFC5705_LABEL "EXPORTER_AAA"

/* a private structures containing the relevant context */
struct tls_module;
struct tls_context;
struct tls;

enum tls_endpoint_type {
	TLS_EP_CLIENT = 1,
	TLS_EP_SERVER = 2	
};

struct tls_keys {
	byte *master;
	u32 master_size;
	byte *server_random;
	u32 server_random_size;
	byte *client_random;
	u32 client_random_size;
};

struct tls_ekm {
	char label[TLS_EKM_LABEL_MAX];
	u32 label_size;
	u32 size;
};

struct tls_session {
	void *ssl;             /* Crypto layer object related to TLS channel */
	int endpoint;
	byte key[TLS_KEY_MAX]; /* TLS Exported Keying Material    [RFC 5705] */
	byte sec[TLS_KEY_MAX]; /* Channel BindingKey derived from [RFC 5705] */
	u16  key_size;
	u16  sec_size;
};

void
tls_init(void);

void
tls_fini(void);

struct tls_session *
tls_channel_init(struct tls *tls);

void
tls_channel_fini(struct tls *tls, struct tls_session *);

struct tls_session *
tls_channel_lookup(const byte *key, u16 size, int type);

struct tls_session *
tls_binding_key_chan(const byte *key, u32 size);

struct tls_session *
tls_session_key_chan(const byte *key, u32 size);

struct tls_session *
tls_exported_key_get(const byte *key, u32 size);

int
tls_touch(struct tls_session *);


int
tls_set_opt_str(int opt, const char *value);

const char *
tls_get_opt_str(int opt);

int
tls_set_ekm_attrs(struct tls_ekm *tls_ekm);

struct tls_ekm *
tls_get_ekm_attrs(void);

void
tls_set_authority(const char *authority);

const char *
tls_get_authority(void);

void
tls_set_handler(const char *handler);

const char *
tls_get_handler(void);

int
tls_get_server_pubkey(struct tls_session *tls);

int
tls_signal_inject(struct tls_session *tls);

int
tls_signal_uninject(struct tls_session *tls);

void
tls_openssl_init(void);

void
tls_openssl_fini(void);

void
tls_nss_init(void);

void
tls_nss_fini(void);

__END_DECLS
#endif/*__AAA_TLS_LIB_H__*/
