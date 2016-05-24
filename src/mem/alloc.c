#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <mem/alloc.h>
#include <mem/debug.h>

void *
xmalloc(size_t size)
{
	void *x = malloc(size);
	if (!x)
		mem_dbg("Cannot allocate %zu bytes of memory", size);
	return x;
}

void *
xmalloc_zero(size_t size)
{
	void *x = xmalloc(size);
	memset(x, 0, size);
	return x;
}

void *
xrealloc(void *addr, size_t size)
{
	return realloc(addr, size);
}

void
xfree(void *ptr)
{
	free(ptr);
}

char *
xstrdup(void *str)
{
	return strdup(str);
}

/* Default allocator */
static void *
std_alloc(struct allocator *a, size_t size)
{
	return xmalloc(size);
}

static void *
std_realloc(struct allocator *a , void *ptr, size_t old_size , size_t new_size)
{
	  return xrealloc(ptr, new_size);
}

static void std_free(struct allocator *a, void *ptr)
{
	xfree(ptr);
}

struct allocator allocator_std = {
	.alloc = std_alloc,
	.realloc = std_realloc,
	.free = std_free,
};

/* Zeroing allocator */
static void *
zeroed_alloc(struct allocator *a, size_t size)
{
	return xmalloc_zero(size);
}

static void *
zeroed_realloc(struct allocator *a , void *ptr, size_t osize, size_t nsize)
{
	ptr = xrealloc(ptr, nsize);
	if (osize < nsize)
		memset((byte *) ptr + osize, 0, nsize - osize);
	return ptr;
}

struct allocator allocator_zeroed = {
	.alloc = zeroed_alloc,
	.realloc = zeroed_realloc,
	.free = std_free,
};
