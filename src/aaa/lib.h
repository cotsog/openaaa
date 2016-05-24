/*
 * $lib.h                                           Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 * AAA (Autentication, Authorisation and Accounting) Library
 *
 */

#ifndef __AAA_LIB_H__
#define __AAA_LIB_H__

#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/decls.h>

#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS
#define __END_DECLS
#endif

__BEGIN_DECLS

/* API version, they compare as integers */
#define API_VERSION PACKAGE_VERSION

/*
 * Predefined set of AAA attributes 
 *
 * user.id:    Presence of user.id attribute specifies authenticated context
 * user.name:  Name should be set because of interoperability with upper layers
 * sess.id     Session ID
 * sess.cid    Channel ID identifies sessions across (re)negotiations
 * sess.key    Keying Material Exporter RFC[5705]
 * sess.sec    SHA1(KeyingMaterialExporter,ServerPublicKey)
 */

#define AAA_ATTR_SESSION_ID              "sess.id"
#define AAA_ATTR_SESSION_CHANNEL_ID      "sess.cid"
#define AAA_ATTR_SESSION_CREATED         "sess.created"
#define AAA_ATTR_SESSION_MODIFIED        "sess.modified"
#define AAA_ATTR_SESSION_ACCESS          "sess.access"
#define AAA_ATTR_SESSION_EXPIRES         "sess.expires"
#define AAA_ATTR_SESSION_BINDING_KEY     "sess.key"
#define AAA_ATTR_SESSION_SECRET          "sess.sec"
#define AAA_ATTR_USER_ID                 "user.id"
#define AAA_ATTR_USER_UUID               "user.uuid"
#define AAA_ATTR_USER_NAME               "user.name"
#define AAA_ATTR_USER_EMAIL              "user.email"
#define AAA_ATTR_USER_GROUPS             "user.groups[]"

/* A private structures containing the aaa context */
struct aaa;

/* A private structure used for accessing binary data */
struct aaa_object;

/* 
 * Specifies available modes for bindings
 */

enum aaa_bind_e {
	AAA_BIND_OBJECT_ID        = 1, /* direct object id access  */
	AAA_BIND_SESSION_ID       = 2, /* session id               */
	AAA_BIND_KEYING_MATERIAL  = 3, /* keying material exporter */
	AAA_BIND_SECRET           = 4, /* keying material derivate */
};

enum aaa_opt_e {
	AAA_OPT_USERDATA          = 1,
	AAA_OPT_CUSTOMLOG         = 2  /* A pointer to a custom logging fn */
};

enum aaa_endpoint_e {
	AAA_ENDPOINT_CLIENT       = 1,
	AAA_ENDPOINT_SERVER       = 2
};

enum aaa_proto_e {
	AAA_PROTO_MEMORY          = 1,
	AAA_PROTO_UNIX            = 2,
	AAA_PROTO_IP              = 3,
	AAA_PROTO_UDP             = 4,
	AAA_PROTO_TCP             = 5,
	AAA_PROTO_IPSEC           = 6,
	AAA_PROTO_DTLS            = 7,
	AAA_PROTO_TLS             = 8,
};

typedef void (*aaa_custom_log_t)(struct aaa*, unsigned level, const char *msg);

void
aaa_init(void);

void
aaa_fini(void);

/* public api functions */

/*
 * NAME
 *
 * aaa_new()
 *
 * DESCRIPTION
 *
 * Creates a new aaa context. Before using it, it is necessary to initialize
 * it by calling aaa_open().
 *
 * RETURN
 *
 * A pointer to the new context or NULL is returned.
 */

struct aaa *
aaa_new(void);

/*
 * NAME
 *
 * aaa_new()
 *
 * DESCRIPTION
 *
 * Creates a new aaa context. Before using it, it is necessary to initialize
 * it by calling aaa_open().
 *
 * RETURN
 *
 * A pointer to the new context or NULL is returned.
 */

void
aaa_free(struct aaa *);

/*
 * NAME
 *
 * aaa_bind()
 *
 * DESCRIPTION
 *
 * aaa_bind() finds a session identified by @id. 
 * The meaning of @id depends on the selected @mode.
 *
 * It can also be used to switch from one session to another after the context 
 * is reset.
 *
 * Binding does not fetch the automatic attributes to the context.
 * Use aaa_select() to fetch other subtrees as needed.
 *
 * NOTE
 *
 * The authentication rules used in binding can vary between implementations 
 *
 * RETURN
 *
 * Upon successful completion, 0 is returned.  Otherwise, a negative
 * error code is returned.
 */

int
aaa_bind(struct aaa *, enum aaa_bind_e type, const char *id);

/*
 * NAME
 *
 * aaa_set()
 *
 * DESCRIPTION
 *
 * Sets the single-valued attribute identified by @key to @val. If @val
 * is NULL, the attribute is removed.
 *
 * Changing of the o.id attribute is possible, but covered by special
 * rules. It must be performed on a context which is bound
 * and the context must be destroyed afterwards.
 *
 * RETURN
 *
 * Upon successful completion, 0 is returned.  Otherwise, a negative
 * error code is returned.
 */

int 
aaa_attr_set(struct aaa *, const char *attr, char *value);

/*
 * NAME
 *
 * aaa_get()
 *
 * DESCRIPTION
 *
 * Gets the current value of a single-valued attribute identified by @key.
 *
 * RETURN
 *
 * Upon successful completion, the value of the attribute is returned.  
 * Otherwise, NULL is returned and you can call aaa_last_error() to determine 
 * the cause of the error.
 */

const char *
aaa_attr_get(struct aaa *, const char *attr);

/*
 * NAME
 *
 * aaa_object_set()
 *
 * DESCRIPTION
 *
 * Sets the binary object
 *                                                                              
 * RETURN
 *
 * Upon successful completion, 0 is returned.  Otherwise, a negative            
 * error code is returned.                                                      
 */

int
aaa_object_set(struct aaa *, void *addr, unsigned int size);

/*
 * NAME                                                                         
 *                                                                              
 * aaa_object_get()
 *                                                                              
 * DESCRIPTION                                                                  
 *                                                                              
 * Gets the current binary object associated with context
 *                                                                              
 * RETURN                                                                       
 *                                                                              
 * Upon successful completion, the value of the attribute is returned.  
 * Otherwise,
 * NULL is returned and you can call aaa_last_error() to determine the cause    
 * of the error.                                                                
 */                                                                             

void *
aaa_object_get(struct aaa *, unsigned int *size);

/*
 * NAME
 *
 * aaa_del_value()
 *
 * DESCRIPTION
 *
 * This function removes the specified value from a multi-valued attribute.
 * It removes all values when @val is NULL.
 *
 * RETURN
 *
 * Upon successful completion, 0 is returned.  Otherwise, a negative
 * error code is returned. Deletion of a non-existent value
 * is not an error.
 */

int
aaa_attr_del_value(struct aaa*, const char *key, const char *val);

/*
 * NAME
 *
 * aaa_has_value()
 *
 * DESCRIPTION
 *
 * This function checks if @val is present in the set of values of the given
 * multi-valued attribute.
 *
 * RETURN
 *
 * When the value is present, a value of 1 is returned. If it is not present,
 * the function returns 0. If any error occurs, a negative error code is returned.
 */

int
aaa_attr_has_value(struct aaa*, const char *key, const char *val);

/*
 * NAME
 *
 * aaa_find_first()
 *
 * DESCRIPTION
 *
 * Finds the first attribute in the subtree of attributes whose names start w
 * ith @path.
 * Further attributes in the subtree can be retrieved by calling 
 * aaa_find_next().
 *
 * The attributes are enumerated in no particular order, but the library 
 * guarantees that every attribute will be listed exactly once. When the 
 * enumeration is in progress, no attributes should be added nor removed.
 *
 * The @recurse parameter controls whether to enumerate only the immediate 
 * descendants (if set to 0), or the whole subtree (if set to 1).
 *
 * RETURN
 *
 * Upon successful completion, the name of the attribute is returned and if 
 * @val is non-NULL,
 * *@val is set to the value of the attribute (in case of multi-valued 
 * attributes, it is an arbitrary chosen value from the set). Otherwise, NULL 
 * is returned and you can call aaa_last_error() to determine the cause of the
 * error.
 */

const char *
aaa_attr_find_first(struct aaa*, const char *path, unsigned recurse);

/*
 * NAME
 *
 * aaa_find_next()
 *
 * DESCRIPTION
 *
 * It finds the next attribute in the specified subtree, as initialized by a call
 * to aaa_find_first().
 *
 * RETURN
 *
 * Upon successful completion, the name of the attribute is returned and 
 * if @val is non-NULL, *@val is set to the value of the attribute (in case of 
 * multi-valued attributes, it is an arbitrary chosen value from the set). 
 * Otherwise, NULL is returned and you can call aaa_last_error() to 
 * determine the cause of the error.
 */

const char *
aaa_attr_find_next(struct aaa *);

/*
 * NAME
 *
 * aaa_select()
 *
 * DESCRIPTION
 *
 * This function fetches the up-to-date values of all attributes in given subtrees
 * from the primary storage (depending on the subtree). 
 *
 * The subtrees are specified by their names separated by ':'.
 *
 * RETURN
 *
 * Upon successful completion, 0 is returned.  Otherwise, a negative
 * error code is returned.
 */

int 
aaa_select(struct aaa *, const char *path);

int
aaa_touch(struct aaa *);

/*
 * NAME
 *
 * aaa_commit()
 *
 * DESCRIPTION
 *
 * Commits all attribute changes to persistent storage.
 *
 * RETURN
 *
 * Upon successful completion, 0 is returned.  Otherwise, a negative
 * error code is returned.
 */

int
aaa_commit(struct aaa *);

int
aaa_set_opt(struct aaa *, enum aaa_opt_e , const char *);

const char *
aaa_get_opt(struct aaa*, enum aaa_opt_e);

int
aaa_last_error(struct aaa *);

__END_DECLS

#endif/*__AAA_LIB_H__*/
