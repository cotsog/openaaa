/*
 * $id: nss/sig.c                              Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#define PRFileDesc char
#define CERTCertificate char

#include <dlwrap.h>

#include <sys/missing.h>
#include <sys/compiler.h>

/* NSPR include files */
#include <prerror.h>
#include <prinit.h>
#include <prtypes.h>
#include <blapit.h>
#include <nssb64.h>
#include <keythi.h>
#include <utilrename.h>
#include <secder.h>
#include <sslt.h>

#include <ctypes/lib.h>
#include <ctypes/string.h>
#include <mem/map.h>
#include <mem/page.h>
#include <mem/stack.h>
#include <hash/fn.h>
#include <crypto/sha1.h>

#include <aaa/lib.h>
#include <aaa/private.h>
#include <aaa/net/tls/lib.h>
#include <aaa/net/tls/nss/lib.h>
#include <dlfcn.h>
#include <dlwrap.h>

static char exported_key[4096];
static int exported_key_size;

void (PR_CALLBACK *orig_SSLHandshakeCallback)(PRFileDesc *, void *) = NULL;

static inline void
str_fix_printable(char *str, unsigned int len)
{
	char *p = str;
	for (unsigned int i = 0; i < len && *p; i++, p++) {
		if (*p == '\r' || *p == ' ' || *p == '\t')
			continue;
		*str = *p;
		str++;
	}
	*str = 0;
}

/* Caller ensures that dst is at least item->len*2+1 bytes long */
static void
SECItemToHex(const SECItem *item, char *dst)
{
	if (dst && item && item->data) {
		unsigned char * src = item->data;
		unsigned int len = item->len;
		for (; len > 0; --len, dst += 2)
			sprintf(dst, "%02X", *src++);
		*dst = '\0';
	}
}

int
nss_auth_handler(const char *tls_key, unsigned int tls_size)
{
        const char *authority = tls_get_authority();
        const char *handler   = tls_get_handler();

	struct tls_ekm *ekm = tls_get_ekm_attrs();

	char *key = stk_mem_to_hex(exported_key, exported_key_size);

	byte *enkey = alloca(512);
	memset(enkey, 0, 511);
	base64_encode(enkey, (byte *)key, strlen(key));

	sha1_context sha1;
	sha1_init(&sha1);
	sha1_update(&sha1, (byte *)key, strlen(key));
	char *id = stk_mem_to_hex((char *)sha1_final(&sha1), SHA1_SIZE / 2);

	byte *enid = alloca(512);
	memset(enid, 0, 511);
	base64_encode(enid, (byte *)id, strlen(id));

	char *bind_id = "MQ%3D%3D";
	char *uri_id  = url_encode((char *)enid);
	char *uri_key = url_encode((char *)enkey);

        char *r3 = stk_printf("http%%3A%%2F%%2F%s%%2FAIM%%2Fservices%%2FR3", authority);

        char *uri_win32 = stk_printf("alucid://callback?authId=%s^&r3Url=%s^&bindingId=%s^&bindingKey=%s",
	                            uri_id, r3, bind_id, uri_key);

        char *uri_unix = stk_printf("alucid://callback?authId=%s\\&r3Url=%s\\&bindingId=%s\\&bindingKey=%s",
	                            uri_id, r3, bind_id, uri_key);

	free(uri_key);
	free(uri_id);

#ifdef CONFIG_WINDOWS
        char *uri = uri_win32;
#else
        char *uri = uri_unix;
#endif

	int status = system(stk_printf("START /B %s -t20 %s", handler, uri));
	return 0;
}

static inline int
derive_server_binding_key(PRFileDesc *fd, struct tls_session *tls)
{
	SSLChannelInfo info;
	sym_SSL_GetChannelInfo(fd, &info, sizeof(info));

        char pub_key_a[4096] = {0};
	char key_id[32] = "123456789";

        CERTCertificate *cert = sym_SSL_PeerCertificate(fd);

	sys_dbg("checking for server certificate: %s", cert ? "Yes" : "No");
	if (cert == NULL)
		return 1;

        SECKEYPublicKey *pub = sym_CERT_ExtractPublicKey(cert);
        SECItem *pubKey;
        SECItem *item = sym_SECKEY_EncodeDERSubjectPublicKeyInfo(pub);
        sym_NSSBase64_EncodeItem(NULL, pub_key_a, sizeof(pub_key_a), item);

        str_fix_printable(pub_key_a, strlen(pub_key_a));

	char *key = stk_mem_to_hex(exported_key, exported_key_size);

        sha1_context sha1;
        sha1_init(&sha1);
        sha1_update(&sha1, (byte *)key, strlen(key));
        //sha1_update(&sha1, (byte *)pub_key_a, strlen(pub_key_a));

        byte *hash = sha1_final(&sha1);
        char *sec = stk_mem_to_hex(hash, SHA1_SIZE);

	int h = hash_string((char *)pub_key_a);

	sys_dbg("checking for server public key: bits=%d hash=%u",
	        info.authKeyBits, h);

	sys_dbg("checking for derived binding key: aaa_binding_key=%s", sec);

/*
	certificate_info(x);
	pubkey_derive_key(tls, x);
*/	
	return 0;
}

static inline int
tls_export_keying_material(PRFileDesc *fd, struct tls_session *tls)
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

	/*
	 * The output is a pseudorandom bit string of length bytes generated
	 * from the master_secret.  (This construction allows for
	 * interoperability with older exporter-type constructions which do not
	 * use context values, e.g., [RFC5281]).
	 * unsigned char key[4096] = {0};
	*/

	unsigned char exp[ekm->size];

	SECStatus state;
	state = sym_SSL_ExportKeyingMaterial(fd, ekm->label, ekm->label_size, 0, NULL, 0, exp, ekm->size);
	if (state != SECSuccess)
		return -1;

	const char *key = stk_mem_to_hex(exp, ekm->size);
	sys_dbg("checking for exported keying material: tls_binding_key=%s", key);

	memcpy(exported_key, exp, ekm->size);
	exported_key_size = ekm->size;

	return 0;
}

static bool
tls_check_safe_reneg(PRFileDesc *fd)
{
	PRBool site;
	return (sym_SSL_HandshakeNegotiatedExtension(fd, 
                ssl_renegotiation_info_xtn, &site) != SECSuccess || !site) ?
		false : true;

}

static bool
nss_session_init(PRFileDesc *fd)
{
	SECStatus rv;
	SSLChannelInfo info;

	if ((rv = sym_SSL_GetChannelInfo(fd, &info, sizeof(info))))
		return false;

	const char *sid = stk_mem_to_hex(info.sessionID, info.sessionIDLength);

	//struct page *page = get_page(map, hash_string(sid) % map->total);

	sys_dbg("checking for TLS session.id: %s", sid);
	return true;
}

void PR_CALLBACK
hook_SSLHandshakeCallback(PRFileDesc *fd, void *data)
{
	SECStatus rv;
        SSLChannelInfo info;
        sym_SSL_GetChannelInfo(fd, &info, sizeof(info));

	char npn[4096];
	unsigned int size = 0;
	enum SSLNextProtoState state;
	sym_SSL_GetNextProto(fd, &state, (unsigned char *)npn, &size, 1024);
	npn[size] = 0;

	switch(state) {
	case SSL_NEXT_PROTO_NO_SUPPORT:
		sys_dbg("NPN no support");
		break;
	case SSL_NEXT_PROTO_NEGOTIATED:
		sys_dbg("NPN NEGOTIATED");
		break;
	case SSL_NEXT_PROTO_NO_OVERLAP:
		sys_dbg("NPN NO OVERLAP");
		break;
	case SSL_NEXT_PROTO_SELECTED:
		sys_dbg("NPN SELECTED");
	break;
	}

	if (*npn == 0)
		strcpy(npn, "tls");

        sys_dbg("tls handshake done endpoint type=client proto=%s", npn);

	sys_dbg("checking for TLS Safe Renegotiation       [RFC-5746]: %s", 
                 tls_check_safe_reneg(fd) ? "Yes" : "No");

	sys_dbg("checking for TLS Protocol Version         [RFC-5246]: %d", 
                info.protocolVersion);

        char *sid = stk_mem_to_hex(info.sessionID, info.sessionIDLength);
        sys_dbg("checking for TLS Session ID: %s", sid);

        tls_export_keying_material(fd, NULL);
	derive_server_binding_key(fd, NULL);

failed:
	if (!orig_SSLHandshakeCallback)
		return;

	orig_SSLHandshakeCallback(fd, data);
}
