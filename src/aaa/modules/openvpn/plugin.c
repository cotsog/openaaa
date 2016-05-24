/*
 *  (c) 2014                                        Daniel Kubec <niel@rtfm.cz>
 *
 *  This software may be freely distributed and used according to the terms
 *  of the GNU Lesser General Public License.
 */

#define _GNU_SOURCE 1
#define ENABLE_SSL

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ctypes/lib.h>
#include <ctypes/string.h>
#include <mem/stack.h>
#include <mem/pool.h>
#include <hash/fn.h>
#include <crypto/sha1.h>
#include <ctypes/b64.h>

#include <aaa/lib.h>
#include <aaa/net/tls/lib.h>
#include <aaa/modules/openvpn/plugin.h>

#include <dlfcn.h>

const char *aaa_authority = NULL;
const char *aaa_handler = NULL;
#define SSL_EKM "SSL_export_keying_material"

int (*export_keying_material)(SSL*, unsigned char *, size_t, 
                              const char*, size_t,
                              const unsigned char*, size_t , int) = NULL;

struct ovpn_ctxt {
	struct mempool *mp;
	struct mempool *mp_api;
	enum {
		VPN_CLIENT = 1,
		VPN_SERVER = 2,
	} type;
	const char *exe;
	const char *aaa_authority;
	const char *aaa_handler;
	const char *aaa_group;
	const char *aaa_role;
	plugin_log_t log;
	int mask;
	u16 management_port;
};

struct ovpn_sess {
	struct mempool *mp;
	struct mempool *mp_api;
};

static inline void
rm_newline_char(const char *str)
{
	for(char *p = (char *)str; *p; p++)
		if (*p == '\n' || *p == '\r')
			*p = ' ';
}

static void
openplugin_sys_log(void *usr, unsigned level, const char *msg)
{
	struct ovpn_ctxt *ovpn = (struct ovpn_ctxt *)usr;
	rm_newline_char(msg);
	ovpn->log(PLOG_NOTE, "openaaa", "%s", msg);
}

static const char *
envp_get(const char *name, const char *envp[])
{
	const int namelen = strlen (name);
	for (int i = 0; envp[i]; ++i) {
		if (!strncmp(envp[i], name, namelen)) {
			const char *cp = envp[i] + namelen;
			if (*cp == '=')
				return cp + 1;
		}
	}

	return NULL;
}

static void
envp_dbg(const char *envp[])
{
	for (int i = 0; envp[i]; ++i) {
		const char *cp = envp[i] + strlen(envp[i]);
		sys_dbg("envp: %s", envp[i]);
	}
	
}

OPENVPN_EXPORT int 
openvpn_plugin_select_initialization_point_v1(void)
{
	return OPENVPN_PLUGIN_INIT_PRE_DAEMON;
}

OPENVPN_EXPORT int
openvpn_plugin_min_version_required_v1(void)
{
	return OPENVPN_PLUGIN_VERSION;
}

OPENVPN_EXPORT void 
openvpn_plugin_close_v1(openvpn_plugin_handle_t handle)
{
	sys_dbg("plugin close");
}

OPENVPN_EXPORT int
openvpn_plugin_open_v3(const int version,
                       struct openvpn_plugin_args_open_in const *args,
                       struct openvpn_plugin_args_open_return *ret)
{
	struct mempool *mp = mp_new(CPU_PAGE_SIZE);
	struct ovpn_ctxt *ovpn = mp_alloc_zero(mp, sizeof(*ovpn));

	ovpn->mp_api = mp_new(CPU_PAGE_SIZE);

	ovpn->mp = mp;
	ovpn->log = args->callbacks->plugin_log;
	ovpn->type = envp_get("remote_1", args->envp) ? VPN_CLIENT : VPN_SERVER;
	sys_log_custom(openplugin_sys_log, (void *)ovpn);

	ovpn->aaa_handler = mp_strdup(mp, envp_get("aaa_handler", args->envp));
	ovpn->aaa_authority = mp_strdup(mp, envp_get("aaa_authority", args->envp));

	aaa_handler = ovpn->aaa_handler;
	aaa_authority = ovpn->aaa_authority;

	ovpn->aaa_group = mp_strdup(mp, envp_get("aaa_group", args->envp));
	ovpn->aaa_role  = mp_strdup(mp, envp_get("aaa_role", args->envp));

	sys_dbg("aaa.handler=%s", ovpn->aaa_handler);
	sys_dbg("aaa.authority=%s", ovpn->aaa_authority);

	tls_set_authority(ovpn->aaa_authority);
	tls_set_handler(ovpn->aaa_handler);

	struct tls_ekm tls_ekm = (struct tls_ekm) {
		.label_size = strlen(TLS_RFC5705_LABEL),
		.size = 16
	};

	snprintf(tls_ekm.label, sizeof(tls_ekm.label) - 1, "%s", TLS_RFC5705_LABEL);
	tls_set_ekm_attrs(&tls_ekm);

	const char *remote = envp_get("remote_1", args->envp);
	sys_dbg("checking for end-point type: %s", remote ? "client" : "server");

	ovpn->exe = args->argv && args->argv[0] ? args->argv[1] : NULL;

	switch(ovpn->type) {
	case VPN_SERVER:
		sys_dbg("setting up aaa tls server");
		ovpn->mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_TLS_FINAL);
		ovpn->mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);
		break;
	case VPN_CLIENT:
		sys_dbg("setting up aaa tls client");
		ovpn->mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_CLIENT_CONNECT);
		ovpn->mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_CLIENT_DISCONNECT);
		ovpn->mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_ROUTE_UP);
		break;
	default:
		sys_err("endpoint detection type failed");
		break;
	}
#ifdef CONFIG_DARWIN
	void *dll = dlopen("/opt/local/lib/libssl.1.0.0.dylib", RTLD_NOW);
#else
	void *dll = dlopen("/usr/local/ssl/lib/libssl.so", RTLD_NOW);	
#endif
	sys_dbg("ssl dll=%p", dll);
	export_keying_material = dlsym(dll, SSL_EKM);
	sys_dbg("export_keying_material=%p", export_keying_material);

	ret->type_mask = ovpn->mask;
	ret->handle = (void *)ovpn;

	aaa_init();
	tls_init();

	return OPENVPN_PLUGIN_FUNC_SUCCESS;
}


OPENVPN_EXPORT void *
openvpn_plugin_client_constructor_v1(openvpn_plugin_handle_t handle)
{
	struct mempool *mp = mp_new(CPU_PAGE_SIZE);

	struct ovpn_ctxt *ovpn = (struct ovpn_ctxt *)handle;
        struct ovpn_sess *sess = mp_alloc_zero(mp, sizeof(*sess));

	sess->mp = mp;
        sess->mp_api = mp_new(CPU_PAGE_SIZE);
	sys_dbg("client constructor");
	return (void *)sess;
}

OPENVPN_EXPORT void
openvpn_plugin_client_destructor_v1(openvpn_plugin_handle_t handle, void *ctx)
{
	struct ovpn_ctxt *ovpn = (struct ovpn_ctxt *)handle;
	struct ovpn_sess *sess = (struct ovpn_sess *)ctx;

	mp_delete(sess->mp_api);
	mp_delete(sess->mp);
	sys_dbg("client destructor");
}

static inline void
openvpn_auth_user_verify(const int version,
                         struct openvpn_plugin_args_func_in const *args,
                         struct openvpn_plugin_args_func_return *ret)
{
	const char *user = envp_get("username", args->envp);
	const char *pass = envp_get("password", args->envp);

	sys_dbg("auth user=%s pass=%s", user, pass);
}

void
aaa_auth(const char *key)
{
	byte *hash;
	byte *enkey = alloca(512);                                              
	memset(enkey, 0, 511);                                                  
	base64_encode(enkey, (byte *)key, strlen(key));                         

	sha1_context sha1;                                                      
	sha1_init(&sha1);                                                       
	sha1_update(&sha1, (byte *)key, strlen(key));                           
	hash = sha1_final(&sha1);
	char *id = stk_hex_enc(hash, SHA1_SIZE / 2);    

	byte *enid = alloca(512);                                               
	memset(enid, 0, 511);                                                   
	base64_encode(enid, (byte *)id, strlen(id));                            

	char *bind_id = "MQ%3D%3D";                                             
	char *uri_id  = url_encode((char *)enid);                               
	char *uri_key = url_encode((char *)enkey);                              

	sys_dbg("checking for tls_binding_id:  %s", id);                        
	sys_dbg("checking for tls_binding_key: %s", key);                       

	char *r3 = stk_printf("http%%3A%%2F%%2F%s%%2FAIM%%2Fservices%%2FR3", aaa_authority);

	char *uri_win32 = stk_printf("aducid://callback?authId=%s^&r3Url=%s^&bindingId=%s^&bindingKey=%s",
	                             uri_id, r3, bind_id, uri_key);             

	char *uri_unix = stk_printf("aducid://callback?authId=%s\\&r3Url=%s\\&bindingId=%s\\&bindingKey=%s",
	                             uri_id, r3, bind_id, uri_key);             

	free(uri_id);                                                           
	free(uri_key);                                                          

	char *uri = uri_unix;                                                   

	int status = system(stk_printf("%s -t20 %s &", aaa_handler, uri));
	sys_dbg("[%.4d] %s %s", WEXITSTATUS(status), aaa_handler, uri);
}

void
openvpn_handshake_handler(const SSL *ssl)
{
	unsigned char k[4096];                                                  
	int size = 16;                                                          
	char *l = "EXPORTER_AAA";                                               

	if (export_keying_material((SSL*)ssl, k, size,l, strlen(l), NULL, 0, 0) == 0)
		return;                                                         

	const char *key = stk_hex_enc(k, size);                              
	aaa_auth(key);
}

OPENVPN_EXPORT int
openvpn_plugin_func_v3(const int version,
                       struct openvpn_plugin_args_func_in const *args,
                       struct openvpn_plugin_args_func_return *ret)
{
	struct ovpn_ctxt *ovpn = (struct ovpn_ctxt *)args->handle;
	struct ovpn_sess *sess = (struct ovpn_sess *)args->per_client_context;

	switch(args->type) {
	case OPENVPN_PLUGIN_ENABLE_PF:
		sys_dbg("enable pf");
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	case OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY:
		sys_dbg("auth user_pass");
		openvpn_auth_user_verify(version, args, ret);
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	case OPENVPN_PLUGIN_ROUTE_UP:
		sys_dbg("route up");
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	case OPENVPN_PLUGIN_CLIENT_CONNECT:
		sys_dbg("connect");
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	case OPENVPN_PLUGIN_CLIENT_DISCONNECT:
		sys_dbg("disconnect");
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	case OPENVPN_PLUGIN_TLS_VERIFY:
		sys_dbg("tls verify");
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	case OPENVPN_PLUGIN_TLS_FINAL:
		sys_dbg("tls final");
		sys_dbg("aaa tls server authentication");
		char *key = (char *)envp_get("exported_keying_material", args->envp);
		if (!key || !strlen(key))
			return OPENVPN_PLUGIN_FUNC_ERROR;

		const char *authority = tls_get_authority();
		const char *handler   = tls_get_handler();

		if (!authority || !handler)
			return OPENVPN_PLUGIN_FUNC_ERROR;

		sys_dbg("aaa.authority=%s", authority);
		sys_dbg("aaa.handler=%s", handler);

		str_collapse(key);
		sha1_context sha1;
		byte *hash;

		int arg_key_size = strlen(key) / 2;
		byte *arg_key = stk_hex_dec(key, strlen(key));

		byte enkey[B64_ENC_LENGTH(arg_key_size) + 1];
		memset(enkey, 0, sizeof(enkey));
		b64_enc(enkey, (byte *)arg_key, arg_key_size);

		sha1_init(&sha1);
		sha1_update(&sha1, (byte *)key, strlen(key));
		hash = sha1_final(&sha1);
		char *id = stk_hex_enc(hash, SHA1_SIZE / 2);

		byte enid[B64_ENC_LENGTH(SHA1_SIZE / 2) + 1];
		memset(enid, 0, sizeof(enid));
		b64_enc(enid, (byte *)hash, SHA1_SIZE / 2);

		sha1_init(&sha1);
		sha1_update(&sha1, (byte *)hash, SHA1_SIZE / 2);
		hash = sha1_final(&sha1);

		byte entype[B64_ENC_LENGTH(SHA1_SIZE / 2) + 1];
		memset(entype, 0, sizeof(entype));
		b64_enc(entype, (byte *)hash, SHA1_SIZE / 2);

		char *uri_type= url_encode((char *)entype);
		char *uri_id  = url_encode((char *)enid);
		char *uri_key = url_encode((char *)enkey);

		sys_dbg("checking for tls_binding_id:  %s", id);
		sys_dbg("checking for tls_binding_key: %s", key);
		sys_dbg("checking for authority: %s", authority);

		sys_dbg("auth_id: %s", enid);
		sys_dbg("binding_id: %s", entype);
		sys_dbg("binding_key: %s", enkey);

		char *r3 = stk_printf("http%%3A%%2F%%2F%s%%2FAIM%%2Fservices%%2FR3", authority);

		char *uri = stk_printf("aducid://callback?authId=%s&r3Url=%s&bindingId=%s&bindingKey=%s",
		                        uri_id, r3, uri_type, uri_key);

		free(uri_id);
		free(uri_key);
		free(uri_type);

#ifdef CONFIG_DARWIN
		sys_dbg("authentication");
		if (ovpn->type == VPN_CLIENT) {
			const char *msg;
			int status;
			sys_dbg("aaa handler=%s", handler);
			status = system(stk_printf("%s -t20 \"%s\"", handler, uri));
			sys_dbg("[%.4d] %s %s", WEXITSTATUS(status), handler, uri);     
		}
#else
		sys_dbg("authentication");
		const char *cmd = stk_printf("%s -pr4 -a%s -i%s -k%s -g%s -r%s", 
		                             handler, authority, id, key, ovpn->aaa_group, ovpn->aaa_role);
		int status = system(cmd);
		int rv = WEXITSTATUS(status);
		sys_dbg("authenticated status=%d", rv);
		if (rv)
			return OPENVPN_PLUGIN_FUNC_ERROR;
#endif

		return OPENVPN_PLUGIN_FUNC_SUCCESS;
/*		
	default:
		goto failed;
*/		
	}

	return OPENVPN_PLUGIN_FUNC_SUCCESS; 

failed:	
	return OPENVPN_PLUGIN_FUNC_ERROR;
}
