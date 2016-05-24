/*
 * $id: mem/page.h                                 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __MEM_PAGE_H__
#define __MEM_PAGE_H__

#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>
#include <mem/alloca.h>
#include <mem/debug.h>

#define PAGEMAP_SHIFT  9

#define PAGE_HDR_MAGIC 0x40000000
#define PAGE_HDR_FREE  0xffffffff
#define PAGE_HDR_PART  0x80000000 /* bit mask for page parts */

__BEGIN_DECLS

struct page;

struct pagemap {
	u64 size;
	u32 magic;
	u32 list;             /* list of free pages */
	u32 avail;            /* number of free pages in list */
	u32 parts;            /* number of large object parts              */
	u32 total;            /* number of pages in map                    */
	u32 shift;            /* page size aligned to power of 2           */
	u16 parts_max;        /* maximum number of object pages            */
	u16 init;
	u16 align;
	struct page *page;
} __attribute__ ((aligned (CPU_STRUCT_ALIGN)));

struct page {
	u32 hdr;              /* page header                               */
	u32 chk;              /* checksum                                  */
	u64 rcu;              /* read copy update lock                     */
	u32 avail;            /* linked list of available pages            */
	u32 hash;             /* linked list of hash collisions            */
	u32 part;             /* linked list of object parts               */
} __attribute__ ((aligned (CPU_STRUCT_ALIGN)));

static void
__page_build_bug_on(void)
{
	__build_bug_on(sizeof(struct pagemap) > CPU_PAGE_SIZE);
	__build_bug_on(sizeof(struct pagemap) > (1 << PAGEMAP_SHIFT));
}

static inline unsigned int
get_page_size(struct pagemap *map)
{
	return 1 << map->shift;
}


static inline struct page *
get_page(struct pagemap *map, unsigned int index)
{
	return (struct page*)((byte*)map->page + ((size_t)index << map->shift));
}

static inline struct page *
get_page_safe(struct pagemap *map, unsigned int index)
{
	if (unlikely(index > map->total || index == (unsigned int)~0U))
		return NULL;

	return (struct page*)((byte*)map->page + ((size_t)index << map->shift));
}

static inline unsigned int
page_index(struct pagemap *map, struct page *page)
{
	return (size_t)((byte*)page - (byte *)map->page) >> map->shift;
}

static inline unsigned long
page_offset(struct pagemap *map, struct page *page)
{
	return (unsigned long)((byte *)page - (byte *)map->page) >> map->shift;
}

static inline bool
page_avail(struct pagemap *map)
{
	return map->list != (u32)~0U;
}

static inline struct page *
page_alloc(struct pagemap *map)
{
	struct page *page = get_page(map, map->list);
	page->hdr = (u32)0U;
	map->list = page->avail;
	map->avail--;
	return page;
}

static inline struct page *
page_alloc_rcu(struct pagemap *map)
{
	struct page *page = get_page(map, map->list);
	page->hdr = (u32)0U;
	map->list = page->avail;
	map->avail--;
	return page;
}

static inline struct page *
page_alloc_safe_rcu(struct pagemap *map)
{
	struct page *page;
	if ((page = get_page_safe(map, map->list)) == NULL)
		return page;

	page->hdr = (u32)0U;
	map->list = page->avail;
	map->avail--;
	return page;
}

static inline void
page_free_rcu(struct pagemap *map, struct page *page)
{
	page->hdr = (u32)~0U;
	page->avail = map->list;
	map->list = page_index(map, page);
	map->avail++;
}

static inline struct page *
page_alloc_safe(struct pagemap *map)
{
	struct page *page;
	if ((page = get_page_safe(map, map->list)) == NULL)
		return page;

	page->hdr = (u32)0U;
	map->list = page->avail;
	map->avail--;
	return page;
}

static inline void
page_free(struct pagemap *map, struct page *page)
{
	page->hdr = (u32)~0U;
	page->avail = map->list;
	map->list = page_index(map, page);
	map->avail++;
}

static inline void
page_lock(struct page *page)
{
}

static inline void
page_unlock(struct page *page)
{
}

static inline void
page_prefetch(struct page *page, u32 shift, u32 pages)
{
	u32 size = 1U << shift;
	byte addr[size], *ptr = (byte *)page;
	for (unsigned int i = 0; i < pages; i++, ptr += size)
		memcpy(addr, ptr, size);
}

static inline void
pages_range_init(struct pagemap *map, u32 base, u32 range)
{
	struct page *page = NULL;
	for (unsigned int i = base; i < range; i++) {
		page = get_page(map, i);
		page->hdr = page->part = page->hash = (u32)~0U;
		page->avail = i + 1;
	}

	if (page)
		page->avail = (u32)~0U;
}

static unsigned long
pages2mb(uint32_t shift, unsigned long pages)
{
	return (pages * (1 << shift)) >> 20;
}

static unsigned long
mb2pages(uint32_t shift, unsigned long mb)
{
	return (mb << (20 - shift));
}

#define pagemap_walk(map, n, list) 

#define page_for_each(map, type, p) \
	for (type p   = (type)get_page(map, 0), \
	       *__x   = (type)get_page(map, map->total); \
	    p < __x;p = (type)((byte*)p + (1U << map->shift)))

__END_DECLS

#endif/*__MEM_PAGE_H__*/
