/*
 * $id: mem/missing.h                                Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __MEM_MISSING_H__
#define __MEM_MISSING_H__

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>

__MEM_BEGIN_DECLS

#ifndef MIN
# define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
#endif

/** Clip a number @x to interval [@min,@max] **/
#define CLAMP(x,min,max) ({ int _t=x; (_t < min) ? min:(_t > max) ? max : _t; })
#define ABS(x) ((x) < 0 ? -(x) : (x))                   
/** Convert macro parameter to a string **/
#define STRINGIFY(x) #x                                 
/* Convert an expanded macro parameter to a string */
#define STRINGIFY_EXPANDED(x) STRINGIFY(x)
/** Glue two tokens together **/
#define GLUE(x,y) x##y
/** Glue two tokens together, separating them by an underscore **/
#define GLUE_(x,y) x##_##y

#define ROL(x, bits) (((x) << (bits)) | ((unsigned int)(x) >> \
                     (sizeof(unsigned int)*8 - (bits))))
#define ROR(x, bits) (((unsigned int)(x) >> (bits)) | ((x) << \
                     (sizeof(unsigned int)*8 - (bits))))

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof(a)/sizeof(*(a)))
#endif

#ifndef O_NOATIME
#define O_NOATIME 0
#endif

#ifndef assert
#define assert(x)
#endif

void
die(const char *fmt, ...);

void *
xmalloc(size_t size);

void *
xmalloc_zero(size_t size);

void *
xrealloc(void *addr, size_t size);

void
xfree(void *ptr);

#ifdef CONFIG_DARWIN
void *mremap(void *addr, size_t size, size_t nsize, int mode);
#endif

__MEM_END_DECLS

#endif/*__MEM_MISSING_H__*/
