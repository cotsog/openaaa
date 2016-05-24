/*
 * $id: mem/debug.h                                 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __SYS_MEM_TRACE_H__
#define __SYS_MEM_TRACE_H__

#ifdef DEBUG
#define mem_dbg(fmt, ...) fprintf(stdout, fmt "\n", __VA_ARGS__);
#else
#define mem_dbg(fmt, ...)
#endif

#endif
