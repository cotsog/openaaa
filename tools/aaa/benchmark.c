/*
 * $benchmark.c                                    Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/lib.h>
#include <sys/mem/map.h>
#include <sys/mem/page.h>
#include <sys/mem/pool.h>
#include <sys/mem/object.h>
#include <sys/mem/stack.h>
#include <sys/hash/fn.h>
#include <aaa/lib.h>
#include <aaa/private.h>
#include <getopt.h>

#include "private.h"

#define OBJECT_MAP_FILE "/tmp/session.map"

struct sess {
	char id[64];
	u32 hash;
	u32 crc;
	u32 pad;
	
};

struct map_stats {
	u32 pages_free;
	u32 pages_total;
	u32 pages_miss;
	u32 pages_miss_max;
};

u32 pages       = 150000;
int page_shift  = 8;
int objects     = 100000;
int object_size = sizeof(struct sess);
byte *source    = NULL;

void
gen_sess(struct sess *s)
{
	memset(s, 0, object_size);
	gen_random((char *)s->id, 31);
	s->hash = hash_string(s->id) % pages;

	if (s->hash > pages)
		exit(1);
}

static void
source_init(void)
{
	timestamp_t now = get_timestamp();

	if (!(source = malloc(object_size * objects)))
		return;

	byte *p = source;
	srand((unsigned int)now);
	for (unsigned int i = 0; i < objects; i++, p += object_size)
		gen_sess((struct sess *)p);

	timestamp_t ms = get_timer(&now);
	sys_dbg("generated %d objects in %llu ms\n", objects, ms);
}

void
source_insert(void)
{

}

void
source_fini(void)
{
	if (source)
		free(source);
}

static struct page *
do_page_find(struct map *map, const char *key)
{
	struct page *page = get_page(map, hash_string(key) % map->total);


	return NULL;
}

int
benchmark_mmap(void)
{
	timestamp_t now = get_timestamp(), ms;

	const char *file = "/tmp/session.map";

	struct map *map = mmap_open(file, MAP_SHARED, page_shift, pages);

	ms = get_timer(&now);
	sys_dbg("map %luM memory in %llu ms\n", pages2mb(page_shift, pages), ms);

	sys_dbg("benchmark: memory mapped sessions");
	now = get_timestamp();

	byte *p = source;
	for (unsigned int i = 0; i < objects; i++, p += object_size) {
		u32 h = ((struct sess *)p)->hash;
		if (h >= pages)
			exit(1);

		struct page *page = get_page(map, h);
		memcpy(page + sizeof(*page), p, sizeof(struct sess));
	}



	ms = get_timer(&now);
	sys_dbg("mmap: inserted %d sessions in %llu ms", objects, ms);

	pages_stats(map);

cleanup:
	now = get_timestamp();
	if (map) {
		//mmap_sync(map, MS_SYNC);
		mmap_close(map);
	}

	ms = get_timer(&now);
        sys_dbg("mmap: commited %d sessions in %llu ms", objects, ms);

	now = get_timestamp();
	unlink(OBJECT_MAP_FILE);
        ms = get_timer(&now);
        sys_dbg("file: unlink %d sessions in %llu ms", objects, ms);


	return 0;
}

int
benchmark_file(void)
{
	char file[255];

	sys_dbg("\nbenchmark: session per file");

	timestamp_t now, ms;
	now = get_timestamp();

	byte *p = source;

	for (unsigned int i = 0; i < objects; i++, p += object_size) {
		struct sess *s = (struct sess *)p;

		snprintf(file, sizeof(file) - 1, "/tmp/aaa/%s", s->id);
		int fd = open(file, O_RDWR | O_CREAT | O_NOATIME, 0600);
		if (fd == -1)
			continue;

		write(fd, s, object_size);
		//fsync(fd);
		close(fd);
	}

	ms = get_timer(&now);
	sys_dbg("file: inserted %d sessions in %llu ms", objects, ms);

	now = get_timestamp();
	p = source;
	for (unsigned int i = 0; i < objects; i++, p += object_size) {
		struct sess *s = (struct sess *)p;
		snprintf(file, sizeof(file) - 1, "/tmp/aaa/%s", s->id);
		unlink(file);
	}

	ms = get_timer(&now);
	sys_dbg("file: unlink %d sessions in %llu ms", objects, ms);

	return 0;
}

static void
benchmark_vma(void)
{
	timestamp_t now = get_timestamp(), ms;

	const char *file = "/tmp/aaa.map";

	struct map *map = mmap_open(file, MAP_SHARED, page_shift, pages);
	if (!map)
		die("can not mmap file: %s", file);

	ms = get_timer(&now);
	sys_dbg("map %luM memory in %llu ms\n", pages2mb(page_shift, pages), ms);

	for (struct page *p = (struct page *)((byte*)map + CPU_PAGE_SIZE), 
	     *__x = (struct page *)((byte *)p + ((size_t)((struct map *)map)->total << ((struct map *)map)->shift));          
	     p < __x; p = (struct page *)((byte *)p + (1U << ((struct map *)map)->shift))) {
		sys_dbg("page: %p", p);
	}
/*
	page_for_each(map, struct page *, p) {
		sys_dbg("page: %p", p);
	}
*/
	/*
	printf("%d loops, with buffer size %d. Average time per loop: %f\n", 
	       nrOfLoops, bufferSize, cpuTime / (double) nrOfLoops);
*/

	mmap_close(map);
}

void                                                                            
benchmark(void)
{
	srand(time(0));

	source_init();
	benchmark_vma();
	//benchmark_mmap();
	//benchmark_file();

	source_fini();
}
