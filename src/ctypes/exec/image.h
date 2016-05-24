/*
 * $lib.h                                      Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __SYS_LIB_EXEC_SYM_H__
#define __SYS_LIB_EXEC_SYM_H__

#include <sys/compiler.h>
#include <sys/cpu.h>

#include <core/lib.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>

extern const char *__exec_image_file;

void
exec_init(void);

void
exec_fini(void);

const char *
get_process_file(void);

#endif
