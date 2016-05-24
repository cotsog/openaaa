/*
 * $id: compat/windows/missing.h               Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __COMPAT_WINDOWS_MISSING_H__
#define __COMPAT_WINDOWS_MISSING_H__

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>
#include <sys/types.h>
#include <sys/mman.h>

__BEGIN_DECLS

#define F_LOCK  1
#define F_ULOCK 0
#define F_TLOCK 2
#define F_TEST  3

#ifndef SIGKILL
#define SIGKILL 9
#endif

#ifndef WEXITSTATUS
#define WEXITSTATUS(s) (s)
#endif

#ifndef SIGNONE
#define SIGNONE 0
#endif

#ifndef SIGTERM
#define SIGTERM 15
#endif

#if defined(WIN32) || defined(WIN64)
  #define snprintf _snprintf
  #define vsnprintf _vsnprintf
#ifndef strcasecmp
  #define strcasecmp stricmp
#endif
#endif

#ifndef F_GETFL
#define F_GETFL 1
#endif

int lockf(int fd, int cmd, off_t len);
int fsync(int fd);
int kill(int pid, int sig);
int fcntl(int fd, int cmd, ... /* arg */ );
int fsync(int fd);

void __compat_perror(void);

char *
strchrnul(const char *s, int c_in);

/*
char *
strndupa(const char *s, size_t n);
*/

__END_DECLS

#endif
