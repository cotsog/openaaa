/*
 * $id: dlwrap.h                                    Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __DL_WRAP_H__
#define __DL_WRAP_H__

#include "easyhook.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

struct dlsym_struct {
	void *wrap;
	void *orig;
	HOOK_TRACE_INFO info;
};

int
dlsym_wrap(struct dlsym_struct *sym);

#endif/*__DL_WRAP_H__*/
