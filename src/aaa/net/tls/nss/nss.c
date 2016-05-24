/*
 *  (c) 2013                                    Daniel Kubec <niel@rtfm.cz>
 *
 *  This software may be freely distributed and used according to the terms
 *  of the GNU Lesser General Public License.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <dlfcn.h>

/* NSPR include files */
#include <prerror.h>
#include <prinit.h>
#include <keyhi.h>
#include <blapit.h>
#include <nssb64.h>
#include <keythi.h>
//#include <utilrename.h>
#include <secder.h>
#include <nss.h>
#include <pk11pub.h>
#include <ssl.h>

#include <sys/lib.h>                                                            
#include <sys/mem/map.h>                                                        
#include <sys/mem/page.h>                                                       
#include <sys/mem/stack.h>                                                      
#include <sys/hash/fn.h>
#include <sys/crypto/sha1.h>
#include "tls.h"

/* lookgs interest for TLS-A 
static PRBool
ssl3_ClientAuthTokenPresent(sslSessionID *sid) {
PK11SlotInfo *slot = NULL;
PRBool isPresent = PR_TRUE;
		 
if (!sid || !sid->u.ssl3.clAuthValid) {
return PR_TRUE;
}
		     
slot = SECMOD_LookupSlot(sid->u.ssl3.clAuthModuleID,
sid->u.ssl3.clAuthSlotID);
if (slot == NULL ||
!PK11_IsPresent(slot) ||
sid->u.ssl3.clAuthSeries     != PK11_GetSlotSeries(slot) ||
sid->u.ssl3.clAuthSlotID     != PK11_GetSlotID(slot)     ||
sid->u.ssl3.clAuthModuleID   != PK11_GetModuleID(slot)   ||
(PK11_NeedLogin(slot) && !PK11_IsLoggedIn(slot, NULL))) {
isPresent = PR_FALSE;
} 
if (slot) {
PK11_FreeSlot(slot);
}
return isPresent;
}
*/
#include <nspr.h>
#include <prtypes.h>
#include <prtime.h>
#include <prlong.h>

#define call_overload_ns nss

#define call(fn, args)
#define output printf

DECLARE_CALL(SSL_HandshakeCallback, 
             SECStatus, PRFileDesc *, void (*callback)(), void *);
DECLARE_CALL(SSL_SetNextProtoCallback,
             SECStatus, PRFileDesc *fd, SSLNextProtoCallback cb, void *arg);
DECLARE_CALL(PR_Read, 
             SECStatus, PRFileDesc *fd, void *b, PRInt32 len);

#ifndef CONFIG_WINDOWS
DEFINE_SYMBOL_TABLE_BEGIN
        DECLARE_LINK(SSL_HandshakeCallback);
	DECLARE_LINK(SSL_SetNextProtoCallback);
	DECLARE_LINK(PR_Read);
DEFINE_SYMBOL_TABLE_END
#endif

#define stk_get_ssl_session_id(fd) \
({\
	PR_SetError(0, 0);                                                 \
	SECItem *__id = SSL_GetSessionID(fd);                                \
	if (__id == NULL) {                                                  \
		const PRErrorCode err = PR_GetError();                     \
		output("msg err %d: %s\n", err, PR_ErrorToName(err));      \
	}                                                                  \
	char sess_id[1024] = {0};                                          \
	SECItemToHex(__id, sess_id);                                         \
	char *__sid = stk_strdup(sess_id); \
 	SECITEM_FreeItem(__id, PR_TRUE);\
 	__sid;\
})

#define stk_get_http_aaa_uri(p) \
({\
	char *__z, *__y , *__x = strstr(p, "X-AAA-ID: "); \
	if (__x) { __x += 10; __y = strchr(__x, ' '); } \
	__z = __x && __y && *__y ? stk_strndup(__x, __y - __x) : ""; \
	__z;\
})

static void 
tcl_stats(void)
{
	SSL3Statistics *stats = SSL_GetStatistics();
	/*
	stats->hch_sid_stateless_resumes;
	stats->hsh_sid_stateless_resumes;
	stats->sch_sid_stateless_resumes;
	*/
}

static PRFileDesc *tls_fd = NULL;

/* 
 * SSLNextProtoCallback is called during the handshake for the client, when a   
 * Next Protocol Negotiation (NPN) extension has been received from the server. 
 * |protos| and |protosLen| define a buffer which contains the server's         
 * advertisement. This data is guaranteed to be well formed per the NPN spec.   
 * |protoOut| is a buffer provided by the caller, of length 255 (the maximum    
 * allowed by the protocol). On successful return, the protocol to be announced 
 * to the server will be in |protoOut| and its length in |*protoOutLen|.        
 *                                                                              
 * The callback must return SECFailure or SECSuccess (not SECWouldBlock).       
 */

SSLNextProtoCallback libnpncb;

SECStatus ssl_next_proto_cb(void *arg, PRFileDesc *fd,                                                             
const unsigned char* protos, unsigned int protosLen,
unsigned char* protoOut, unsigned int* protoOutLen,
unsigned int protoMaxOut)
{
	char npn[1024] = {0};
	memcpy(npn, protos, protosLen);
	npn[protosLen - 1] = 0;

	printf("tls.npn: %s\n", npn + 1);
	return libnpncb(arg, fd, protos, protosLen, protoOut, protoOutLen, protoMaxOut);
}
                                                                                
/* 
 * SSL_SetNextProtoCallback sets a callback function to handle Next Protocol    
 * Negotiation. It causes a client to advertise NPN. 
 */

SSLNextProtoCallback libnpncb;

SSL_IMPORT SECStatus 
SSL_SetNextProtoCallback(PRFileDesc *fd, SSLNextProtoCallback cb, void *arg)
{
	init_symbols();
	libnpncb = cb;
	return openssl_SSL_SetNextProtoCallback(fd, ssl_next_proto_cb, arg);
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

static void
memToHex(unsigned char *src, unsigned int len, char * dst)
{
	for (;len > 0; --len, dst += 2) {
		sprintf(dst, "%02X", *src++);
	}
	*dst = '\0';
}

static void
__tls_handshake(PRFileDesc *fd)
{
	output("msg handshake fd: %p\n", fd);
	output("msg library version: %s\n", NSSSSL_GetVersion());

	SSLChannelInfo info;
	SSL_GetChannelInfo(fd, &info, sizeof(info));

	output("msg tls.auth.key.bits: %d\n", info.authKeyBits);
	output("msg tls.auth.len: %d\n", info.length);

	const char *label = "EXPERIMENTAL";
	unsigned char key[255] = {0};
	PR_SetError(0, 0);
	int ret = SSL_ExportKeyingMaterial(fd, label, strlen(label), 0, NULL, 0, key, 20);
	if (ret != SECSuccess) {
		const PRErrorCode err = PR_GetError();
		output("msg err %d: %s\n", err, PR_ErrorToName(err));
	}

        char key_id[255] = {0};                                                 
        memToHex(key, 20, key_id);

	char *sid = stk_get_ssl_session_id(fd);

	struct page *page = get_page(map, hash_string(sid) % map->total);
	struct tlsa *tlsa = (struct tlsa *)page;

	output("tlsa->id: %s\n", tlsa->id);

	if (!strncasecmp(tlsa->id, sid, strlen(sid)))
		return;

	memset(tlsa, 0, sizeof(*tlsa));

	strcpy(tlsa->id, sid);
	tlsa->created = tlsa->modified = time(NULL);
	tlsa->expires = 3000;

	char pub_key_a[4096] = {0};

	CERTCertificate *cert = SSL_PeerCertificate(fd);
	SECKEYPublicKey *pub = CERT_ExtractPublicKey(cert);
	SECItem *pubKey;

	SECItem *item = SECKEY_EncodeDERSubjectPublicKeyInfo(pub);
	NSSBase64_EncodeItem(NULL, pub_key_a, sizeof(pub_key_a), item);

	/*
	output("-----BEGIN PUBLIC KEY-----\n%s\n-----END PUBLIC KEY-----\n", 
	       pub_key_a);
	*/
	str_fix_printable(pub_key_a, strlen(pub_key_a));

	output("attr sess.id: %s\n", sid);                                  
	output("attr sess.uri: %s\n", SSL_RevealURL(fd));
	output("attr sess.key: %s\n", key_id);

	sha1_context sha1;
	sha1_init(&sha1);
	sha1_update(&sha1, (byte *)key_id, strlen(key_id));
	sha1_update(&sha1, (byte *)pub_key_a, strlen(pub_key_a));

	byte *hash = sha1_final(&sha1);
	char *sec = stk_mem_to_hex(hash, SHA1_SIZE);
	output("attr sess.sec: %s\n", sec);

	strcpy(tlsa->key, sec);

	//SECKEY_DestroyPublicKey(pub);

/*
 * if (data) SECITEM_FreeItem(data, PR_TRUE);
 * if (pubKey) SECKEY_DestroyPublicKey(pubKey);
 * if (keyInfo) SECKEY_DestroySubjectPublicKeyInfo(keyInfo);
 */	

	PRBool npn;
	ret = SSL_HandshakeNegotiatedExtension(fd, ssl_next_proto_nego_xtn, &npn);
	if (!npn)
		return;

	/* SECITEM_FreeItem(data, PR_TRUE); */

	unsigned char proto[1024] = {0};
	unsigned int len = 0;

	memset(proto, 0, sizeof(proto));

	SSLNextProtoState nps;
	int rv = SSL_GetNextProto(fd, &nps, proto, &len, 1023);
	if (rv != SECSuccess)
		return;

	printf("attr tlsa.scheme: %s\n", proto);

	/*
	 * SSL_SetNextProtoCallback
	 * SSL_SetNextProtoNego and SSL_GetNextProto
	 */
}

static SSLHandshakeCallback libcb;

static void PR_CALLBACK 
__tls_handshake_fn(PRFileDesc *fd, void *data)
{
	libcb(fd, data);
	__tls_handshake(fd);
}

/* this is the key routine for catching SSL handshake */
SECStatus
SSL_HandshakeCallback(PRFileDesc *fd, void (*cb)(), void *data)
{
	init_symbols();
	libcb = cb;
	tls_fd = fd;
	return openssl_SSL_HandshakeCallback(fd, (void (*)())__tls_handshake_fn, data);
	
}

SECStatus
PR_Read(PRFileDesc *fd, void *b, PRInt32 len)
{
	init_symbols();
	if (fd != tls_fd || len < 6 || strncasecmp(b, "http/", 5))
		return openssl_PR_Read(fd, b, len);

	char *p = alloca(len + 1);
	memcpy(p, b, len);
	p[len + 1] = 0;
	char *end = strstr(p, "\r\n");
	if (end) *end = 0;
	int size = len;

	for (int i = 0; i < len; i++)
		if (p[i] == '\n')
			p[i] = ' ';

	end = strstr(p, "<html>");
	if (end) *end = 0;

        end = strstr(p, "<body>");                                              
        if (end) *end = 0;

	end = strstr(p, "text/html");
	if (end) *end = 0;

	printf("%s\n", p);

	if (strstr(p, "X-AAA-ID")) {
		char *sid = stk_get_ssl_session_id(fd);
		struct page *page = get_page(map, hash_string(sid) % map->total);
		struct tlsa *tlsa = (struct tlsa *)page;

		output("\nTLS-AAA-NEGOTIATE\n");

		tlsa->uri[0] = 0;

		char *uri = stk_get_http_aaa_uri(p);
		if (*uri)
			strcpy(tlsa->uri, uri);

		output("tls.uri: %s\n", tlsa->uri);
		output("tls.sid: %s\n", tlsa->id);
		output("tls.key: %s\n", tlsa->key);

		char *url = stk_printf("alucid://callback?authId=%s&r3Url=%s&bindingId=%s&bindingKey=%s",
				tlsa->id, tlsa->uri, "", tlsa->key);

		output("\nqrcode: %s\n", url);

		char *app = stk_printf("qrencode -o /tmp/sid-%s %s", 
		                       tlsa->id, url);
		//execl("/bin/echo", "/bin/echo", "hello world");
		system(app);

		//gthumb
		app = stk_printf("gthumb /tmp/sid-%s", tlsa->id);
		system(app);

	}

	return openssl_PR_Read(fd, b, len);
}
