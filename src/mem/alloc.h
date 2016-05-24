/*
 * $id: mem/alloc.h                                 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __MEM_ALLOCATOR_H__
#define __MEM_ALLOCATOR_H__

void *xmalloc(size_t size);
void *xmalloc_zero(size_t size);
void *xrealloc(void *addr, size_t size);
void xfree(void *ptr);

struct allocator {
	void *(*alloc)(struct allocator *alloc, size_t size);
	void *(*realloc)(struct allocator *alloc, void *ptr, 
	                 size_t old_size, size_t new_size);
	void (*free)(struct allocator *alloc, void *ptr);
};

extern struct allocator allocator_std;
extern struct allocator allocator_zeroed;

#endif
