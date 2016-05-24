#ifndef __COMPAT_WINDOWS_H__
#define __COMPAT_WINDOWS_H__

#define NTDDI_VERSION           NTDDI_WIN2KSP4
#define _WIN32_WINNT            0x500
#define _WIN32_IE_              _WIN32_IE_WIN2KSP4

#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <sys/decls.h>
#include <sys/locking.h>

__BEGIN_DECLS

#define F_LOCK  1
#define F_ULOCK 0
#define F_TLOCK 2
#define F_TEST  3

#ifndef SIGKILL
#define SIGKILL 9
#endif

#ifndef SIGNONE
#define SIGNONE 0
#endif

#ifndef SIGTERM
#define SIGTERM 15
#endif

#define F_GETFL 1

int lockf(int fd, int cmd, off_t len);
int fsync(int fd);
int kill(int pid, int sig);
int fcntl(int fd, int cmd, ... /* arg */ );
int fsync(int fd);

void __compat_perror(void);

char *
strchrnul(const char *s, int c_in);

char *
strndupa(const char *s, size_t n);

__END_DECLS

#endif
