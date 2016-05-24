/*
 * $lib.h                                      Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __SYS_LIB_H__
#define __SYS_LIB_H__

#include <sys/missing.h>
#include <sys/compiler.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/missing.h>
#include <mem/unaligned.h>
#include <sys/types.h>
#include <sys/decls.h>

/* API version, they compare as integers */                                     
#define API_VERSION PACKAGE_VERSION

__BEGIN_DECLS

extern unsigned int __cpu_count;
extern unsigned int __cpu_page_size;
extern unsigned int __verbose;

void sys_init(void);
void sys_fini(void);

/* signal API */
void sig_init(void);
void sig_fork(void);
void sig_fini(void);
void sig_ignore(int sig);
void sig_disable(int sig);
void sig_enable(int sig);
void irq_enable(void);
void irq_disable(void);

/* system report */
void
log_init(void);

void
log_fini(void);

void
log_file(const char *file);

void
log_name(const char *name);

void                                                                            
log_writev(unsigned int flags, const char *fmt, va_list args);

void                                                                            
log_write(unsigned int flags, const char *fmt, ...);

#define sys_dbg(fmt, ...) \
	log_write(0, fmt "\r\n", ## __VA_ARGS__)
#define sys_info(fmt, ...) \
        log_write(0, fmt "\n", ## __VA_ARGS__)
#define sys_msg(fmt, ...) \
	log_write(0, fmt "\n", ## __VA_ARGS__)
#define sys_err(fmt, ...) \
	log_write(0, fmt "\n", ## __VA_ARGS__)

typedef void (*sys_log_custom_t)(void *, unsigned level, const char *msg);

void
sys_log_custom(sys_log_custom_t fn, void *data);

void *xmalloc(size_t size);
void *xmalloc_zero(size_t size);
void xfree(void *ptr);
char *xstrdup(void *str);

void
noop(void);

timestamp_t
get_timestamp(void);

unsigned int
get_timer(timestamp_t *timer);

int
random_range(unsigned const low, unsigned const high);

void
gen_random(char *s, int len);

unsigned int                                                                    
base64_encode(byte *dest, const byte *src, unsigned int len);

timestamp_t
get_timestamp(void);

unsigned int
get_timer(timestamp_t *timer);

#ifndef LIKE_MALLOC
#define LIKE_MALLOC
#endif

#ifndef SENTINEL_CHECK
#define SENTINEL_CHECK
#endif

__END_DECLS

#endif/*__mH__*/
