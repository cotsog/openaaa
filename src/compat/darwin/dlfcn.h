/*
 * dlfcn-win32
 * Copyright (c) 2007 Ramiro Polla
 *
 * Adaptation AAA Library and few fixes
 * Copyright (c) 2014 Daniel Kubec <niel@rtfm.cz>
 *
 * dlfcn-win32 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * dlfcn-win32 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with dlfcn-win32; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DLFCN_H
#define DLFCN_H

#ifdef __cplusplus
extern "C" {
#endif

/* POSIX says these are implementation-defined.
 * To simplify use with Windows API, we treat them the same way.
 */

#define RTLD_LAZY   0
#define RTLD_NOW    0

#define RTLD_GLOBAL (1 << 1)
#define RTLD_LOCAL  (1 << 2)

/* These two were added in The Open Group Base Specifications Issue 6.
 * Note: All other RTLD_* flags in any dlfcn.h are not standard compliant.
 */

#define RTLD_DEFAULT    0
#define RTLD_NEXT       0

/* these are extensions for _GNU_SOURCE */
typedef struct {
	const char *dli_fname;/* Filename of defining object */
	void *dli_fbase;      /* Load address of that object */
	const char *dli_sname;/* Name of nearest lower symbol */
	void *dli_saddr;      /* Exact value of nearest symbol */
} Dl_info;

struct dl_phdr_info {
	void *dlpi_addr;  /* Base address of object */
	const char *dlpi_name;  /* (Null-terminated) name of object */
};

void *
dlopen ( const char *file, int mode );

int
dlclose( void *handle );

void *
dlsym  ( void *handle, const char *name );

char *
dlerror( void );

int
dladdr(void *addr, Dl_info *info);

int
dl_iterate_phdr(int (*cb)(struct dl_phdr_info *info, size_t size, void *data), 
                void *data);

#ifdef __cplusplus
}
#endif

#endif /* DLFCN_H */
