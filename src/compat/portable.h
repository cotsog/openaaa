#ifndef __COMPAT_PORTABLE_H__
#define __COMPAT_PORTABLE_H__

/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <sys/decls.h>
*/
#include "autoconf.h"

#ifdef CONFIG_WINDOWS
#include <compat/windows.h>
#endif

#ifndef CONFIG_WINDOWS
#include <sys/mman.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#endif

#define CPU_AFFINITY_DOMAIN   1
#define CPU_AFFINITY_ID       2
#define CPU_AFFINITY_INDEX    3

int cpu_set_affinity(int id, int mode, int cpu);

int sys_uri_scheme_register(const char *id, const char *handler, const char *about);
int sys_uri_scheme_unregister(const char *id);
void sys_uri_scheme_dump(const char *id);

const char *sys_get_progname(void);

int
open_uri(const char *uri);

unsigned long find_window_pid(char *c, char *window);
unsigned long find_proc_pid(const char *name);
unsigned long find_proc_tid(unsigned long pid);
unsigned long find_module(unsigned long pid, const char *name);

#ifndef strdupa
#define strdupa(s)                                                        \
(__extension__                                                            \
({                                                                        \
	const char *__old = (s);                                          \
	size_t __len = strlen (__old) + 1;                                \
	char *__new = (char *) __builtin_alloca (__len);                  \
	(char *) memcpy (__new, __old, __len);                            \
}))
#endif

#ifndef strndupa
#define strndupa(s, n)                                                   \
(__extension__                                                            \
({                                                                        \
	const char *__old = (s);                                          \
	size_t __len = strnlen (__old, (n));                              \
	char *__new = (char *) __builtin_alloca (__len + 1);              \
	__new[__len] = '\0';                                              \
	(char *) memcpy (__new, __old, __len);                            \
}))                                                                         
#endif

#ifdef CONFIG_LINUX
#include <compat/linux/sys.h>
#endif

#ifdef CONFIG_HPUX
#include <compat/hpux/sys.h>
#endif

#ifdef CONFIG_DARWIN
#include <compat/darwin/sys.h>
#include <compat/darwin/dlfcn-add.h>
#endif

#ifdef CONFIG_WINDOWS
#include <compat/windows/api.h>
#endif

#endif
