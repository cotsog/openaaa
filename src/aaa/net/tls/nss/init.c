/*
 * $id: nss/init.c                              Daniel Kubec <niel@rtfm.cz>
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

#ifdef CONFIG_LINUX
#include <link.h>
#endif

void
__nss_sym_table_init(void)
{
	sym_link(NSSSSL_GetVersion);
	sym_link(SSL_HandshakeCallback);
	sym_link(SSL_SetNextProtoCallback);
	sym_link(SSL_SetNextProtoNego);
	sym_link(SSL_GetNextProto);
	sym_link(SSL_ExportKeyingMaterial);
	sym_link(SSL_SecurityStatus);
	sym_link(SSL_GetSessionID);
	sym_link(SSL_SetURL);
	sym_link(SSL_RevealURL);
	sym_link(PR_GetError);
	sym_link(PR_ErrorToName);
	sym_link(PR_SetError);
	sym_link(PR_Read);
	sym_link(SSL_PeerCertificate);
	sym_link(CERT_ExtractPublicKey);
	sym_link(SECKEY_EncodeDERSubjectPublicKeyInfo);
	sym_link(NSSBase64_EncodeItem);
	sym_link(SECITEM_FreeItem);
	sym_link(SSL_GetChannelInfo);
	sym_link(SSL_HandshakeNegotiatedExtension);
}

_noinline SECStatus
sym_SSL_HandshakeCallback_wrap(PRFileDesc *fd, void (*cb)(), void *ctx)
{
	orig_SSLHandshakeCallback = cb;
	return sym_SSL_HandshakeCallback(fd, hook_SSLHandshakeCallback, ctx);
}

SECStatus
sym_SSL_SetURL_wrap(PRFileDesc *fd, const unsigned char *url)
{
	sys_dbg("uri: %s", url);
	return sym_SSL_SetURL(fd, (char *)url);
}

SECStatus
sym_PR_Read_wrap(PRFileDesc *fd, void *b, PRInt32 len)
{
	if (len < 6 || strncasecmp(b, "http/", 5))
		goto declined;

        char *p = alloca(len + 1);
        memcpy(p, b, len);
        p[len + 1] = 0;
        char *end = strstr(p, "\r\n\r\n");
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

	if (!strstr(p, "X-AAA-ID"))
		goto declined;

        sys_dbg("%s\n", p);
	nss_auth_handler(NULL, 0);
declined:
	return sym_PR_Read(fd, b, len);
}

static int
sym_module_callback(struct dl_phdr_info *info, size_t size, void *data)
{
/*
	if (info->dlpi_addr && info->dlpi_name)
		sys_dbg("%p name=%s", info->dlpi_addr, info->dlpi_name);
*/
	void *dll = dlopen(info->dlpi_name, RTLD_GLOBAL);
	return 0;
}

static int
nss_runtime_linkage(void)
{
	__nss_sym_table_init();

	const char *v = sym_NSSSSL_GetVersion? sym_NSSSSL_GetVersion(): NULL;
	sys_dbg("checking for NSS Version                            : %s", 
                v ? v : "No");
	if (!v)
		return 1;

	sys_dbg("checking for TLS Keying Material Exporter [RFC 5705]: %s",
	        sym_SSL_ExportKeyingMaterial ? "Yes" : "No");
/*		
	sys_dbg("checking for TLS Authorization Extension  [RFC 5878]: %s",
		sym_SSL_CTX_add_client_custom_ext ? "Yes" : "No");
	sys_dbg("checking for TLS Supplemental Data        [RFC 4678]: %s",
		sym_SSL_CTX_add_client_custom_ext ? "Yes" : "No");
*/
	return (int)(sym_SSL_HandshakeCallback == NULL);	
}

void
tls_nss_init(void)
{
#ifndef CONFIG_DARWIN
	int e;

	dl_iterate_phdr(sym_module_callback, NULL);
	if (nss_runtime_linkage())
		return;

	dlsym_SSL_HandshakeCallback = (struct dlsym_struct) {
		.orig = sym_SSL_HandshakeCallback, 
		.wrap = sym_SSL_HandshakeCallback_wrap
	};

	e = dlsym_wrap(&dlsym_SSL_HandshakeCallback);
	sys_dbg("checking for TLS Handshake Signalling                : %s", 
	        e ? "No" : "Yes");

        dlsym_PR_Read = (struct dlsym_struct) {
		.orig = sym_PR_Read,
		.wrap = sym_PR_Read_wrap
	};

	e = dlsym_wrap(&dlsym_PR_Read);
	sys_dbg("checking for TLS App Signalling: %s", e ? "No" : "Yes");

	dlsym_SSL_SetURL = (struct dlsym_struct) {
		.orig = sym_SSL_SetURL,
		.wrap = sym_SSL_SetURL_wrap
	};

	e = dlsym_wrap(&dlsym_SSL_SetURL);
	sys_dbg("checking for TLS Session URI: %s", e ? "No" : "Yes");
#endif
}

void
tls_nss_fini(void)
{
}
