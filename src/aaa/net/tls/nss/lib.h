#ifndef __LIBAAA_TLS_NSS_SYM_H__
#define __LIBAAA_TLS_NSS_SYM_H__

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

#define sym_define(fn, rv, args...) \
	rv ((*sym_##fn)(args)); \
        struct dlsym_struct dlsym_##fn

#define sym_link(fn) \
	sym_##fn = dlsym((void *)RTLD_DEFAULT, #fn);\

typedef enum SSLNextProtoState {
	SSL_NEXT_PROTO_NO_SUPPORT = 0, /* No peer support                */
	SSL_NEXT_PROTO_NEGOTIATED = 1, /* Mutual agreement               */
	SSL_NEXT_PROTO_NO_OVERLAP = 2,  /* No protocol overlap found      */
	SSL_NEXT_PROTO_SELECTED   = 3, /* Server selected proto (ALPN)   */
} SSLNextProtoState;

typedef SECStatus
(PR_CALLBACK *SSLNextProtoCallback)(
		void *arg,
		PRFileDesc *fd,
		const unsigned char* protos,
		unsigned int protosLen,
		unsigned char* protoOut,
		unsigned int* protoOutLen,
		unsigned int protoMaxOut);

sym_define(NSSSSL_GetVersion, char *, void);
sym_define(SSL_HandshakeCallback, SECStatus, PRFileDesc *, void (*)(), void *);
sym_define(SSL_HandshakeNegotiatedExtension, SECStatus, PRFileDesc *, int,  PRBool *);
sym_define(SSL_SetNextProtoCallback, SECStatus, PRFileDesc *, SSLNextProtoCallback, void *);
sym_define(SSL_SetNextProtoNego, SECStatus, PRFileDesc *, const unsigned char *, unsigned int);
sym_define(SSL_GetNextProto, SECStatus, PRFileDesc *, SSLNextProtoState *,
           unsigned char *, unsigned int *, unsigned int);
sym_define(SSL_PeerCertificate, CERTCertificate *, PRFileDesc *);
sym_define(CERT_ExtractPublicKey, SECKEYPublicKey *, CERTCertificate *);
sym_define(SECKEY_EncodeDERSubjectPublicKeyInfo, SECItem *, SECKEYPublicKey *);
sym_define(NSSBase64_EncodeItem, char *, PLArenaPool *, char *, unsigned int, SECItem *);
sym_define(SSL_GetChannelInfo, SECStatus, PRFileDesc *, SSLChannelInfo *, unsigned int);
sym_define(PR_GetError, const PRErrorCode, void);
sym_define(PR_ErrorToName, const char *, PRErrorCode);
sym_define(PR_SetError, void, PRErrorCode , PRInt32 );
sym_define(PR_Read, SECStatus, PRFileDesc *, void *, PRInt32);
sym_define(SECITEM_FreeItem, void, SECItem *, PRBool );

sym_define(SSL_GetSessionID, SECItem *,PRFileDesc *);
sym_define(SSL_SecurityStatus,SECStatus, PRFileDesc *, int *, char **cipher, int *keySize, int *secretKeySize, char **issuer, char **subject);
sym_define(SSL_RevealURL, char *, PRFileDesc *);
sym_define(SSL_SetURL, SECStatus, PRFileDesc *, const char *);
sym_define(SSL_ExportKeyingMaterial, SECStatus, PRFileDesc *fd, const char *, 
           unsigned int, PRBool,const unsigned char *,unsigned int,
	   unsigned char *,unsigned int );
extern void
(PR_CALLBACK *orig_SSLHandshakeCallback)(PRFileDesc *, void *);

void
hook_SSLHandshakeCallback(PRFileDesc *, void *);

int
nss_auth_handler(const char *tls_key, unsigned int tls_size);

#endif
