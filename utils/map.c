/*
 * (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include <marvin/lib.h>
#include <marvin/kernel.h>
#include <marvin/crc.h>
#include <marvin/mpm/sched.h>
#include <marvin/mem/page.h>
#include <marvin/mem/map.h>
#include <marvin/mem/stack.h>
#include <marvin/hash/fn.h>
#include <marvin/hash/table.h>

#include <compat/getopt/getopt.h>

#define PAGE_HEADER_SIZE 52

const char *package_name = "map";

enum { 
        OPT_HELP,
        OPT_VERSION,
	OPT_CONFIG,
	OPT_CPU_AFFINITY,
	OPT_CPU_MODEL,
	OPT_ARCHITECTURE,
	OPT_INIT,
	OPT_FILE,
	OPT_ACCESS,
	OPT_OBJECTS,
	OPT_PAGES,
	OPT_PAGES_MISS,
	OPT_TASKS,
	OPT_PROCESSES,
	OPT_THREADS,
	OPT_BENCHMARK,
	OPT_CHECK,
	OPT_REPEAT,
	OPT_VERBOSE,
	OPT_DURATION,
};

enum access_type {
	ACCESS_RANDOM           = (1 << 1),
	ACCESS_SEQUENTIAL       = (1 << 2),
	ACCESS_IO               = (1 << 3),
	ACCESS_ZERO_COPY        = (1 << 4),
	ACCESS_ATOMIC_IO        = (1 << 5),
	ACCESS_ATOMIC_ZERO_COPY = (1 << 6),
	ACCESS_CACHE            = (1 << 7),
	ACCESS_NO_CACHE         = (1 << 8),
};

enum architecture_type {
	ARCH_SHARED              = 1,
	ARCH_DEDICATED           = 2,
};

enum table_type {
	TABLE_BINARY            = 1,
	TABLE_TEXT              = 2,
};

static const char * const access_type_names[] = {
	[ACCESS_RANDOM]    = "random",    [ACCESS_SEQUENTIAL]       = "sequential",
	[ACCESS_IO]        = "io",        [ACCESS_ZERO_COPY]        = "zero-copy",
	[ACCESS_ATOMIC_IO] = "atomic-io", [ACCESS_ATOMIC_ZERO_COPY] = "atomic-zero-copy",
	[ACCESS_CACHE]     = "cache",     [ACCESS_NO_CACHE]         = "no-cache",
};

static const char * const cpu_model_names[] = {
	[CPU_DEFAULT]      = "default",   [CPU_SCALABILITY]         = "scalability",
};

static const char * const arch_type_names[] = {
        [ARCH_SHARED]      = "shared",    [ARCH_DEDICATED]           = "dedicated",
};

static const char * const table_type_names[] = {
        [TABLE_BINARY]     = "binary",    [TABLE_TEXT]              = "text",
};

static const char *opt_cmd = "h:vm:";
static struct option opt_long[] = {
        {"help"         ,0, 0, OPT_HELP        },
        {"version"      ,0, 0, OPT_VERSION     },
	{"init"         ,0, 0, OPT_INIT        },
	{"file"         ,1, 0, OPT_FILE        },
	{"access"       ,1, 0, OPT_ACCESS      },
	{"cpu-model"    ,1, 0, OPT_CPU_MODEL   },
	{"cpu-affinity" ,1, 0, OPT_CPU_AFFINITY},
	{"architecture" ,1, 0, OPT_ARCHITECTURE},
	{"processes"    ,1, 0, OPT_PROCESSES   },
	{"threads"      ,1, 0, OPT_THREADS     },
	{"pages"        ,1, 0, OPT_PAGES       },
	{"objects"      ,1, 0, OPT_OBJECTS     },
	{"pages-miss"   ,1, 0, OPT_PAGES_MISS  },
	{"benchmark"    ,0, 0, OPT_BENCHMARK   },
	{"check"        ,0, 0, OPT_CHECK       },
	{"repeat"       ,1, 0, OPT_REPEAT      },
	{"verbose"      ,0, 0, OPT_VERBOSE     },
	{"duration"     ,1, 0, OPT_DURATION    },
};

static int 
opt_find(const char * const names[], unsigned int size, const char *val)
{
	for (int i = 0; i < size; i++)
		if (names[i] && !strcmp(names[i], val))
			return i;

	return -1;
}

static void
opt_print(const char *about, const char * const names[], unsigned int size)
{
        printf("\t%s\t\t\t", about);

        for (int i = 0; i < size; i++) { 
                if (!names[i]) 
			continue; 

		printf(" %s", names[i]);

                if ((size - i) > 1) 
			printf(" |");
        }

        printf("\n");
}

NONRET static void
usage(int code)
{
        printf("Usage: %s <commands-and-parameters>\n\n", package_name);
	printf("\t--version,      -v\t\t\t package version\n");
	printf("\t--cpu-affinity, -f\t\t\t cpu affinity\n");
	opt_print("--cpu-model     -c", cpu_model_names, ARRAY_SIZE(cpu_model_names));
	opt_print("--architecture  -a", arch_type_names, ARRAY_SIZE(arch_type_names));
        printf("\t--processes,    -x\t\t\t number of processes\n");
        printf("\t--threads,      -y\t\t\t number of threads\n");

	printf("\t--file,         -f\t\t\t table space file\n");
	opt_print("--access        -a", access_type_names, ARRAY_SIZE(access_type_names));

	printf("\t--objects,      -o\t\t\t number of inserted objects\n");
	printf("\t--pages,        -p\t\t\t pages in file based table\n");
	printf("\t--pages-miss,   -m\t\t\t pages miss\n");
	printf("\t--duration,     -t\t\t\t finish test after time-max miliseconds \n");

	printf("\n");
        exit(code);
}

NONRET static void
nothing(void)
{
	printf("Nothing to do.\n");
	exit(0);
}

struct app {
        struct object object;
	int (*handler)(struct task *);
	const char *file;
	int init;
	enum cpu_model cpu_model;
	enum table_type table_type;
	enum access_type access_type;
	unsigned int processes;
	unsigned int threads;
	unsigned int tasks;
	unsigned int objects;
	unsigned int pages;
	unsigned int pages_miss;
	unsigned int duration;
	bool verbose;
#define MAX_CMDS 16
        unsigned int cmd[MAX_CMDS];
        const char *arg[MAX_CMDS];
        unsigned int num_cmds;
};

static struct app *app;
static struct map *map;

struct sess {
        struct object object;
        char key[64];
	u32 key_size;
	u32 hash;
        u32 crc;
        timestamp_t created;
        timestamp_t expires;
        byte obj[];
};

void gen_random(char *s, const int len) {
	static const char alphanum[] = 
	                  "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) 
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

static inline void
sess_init(struct sess *sess)
{
	memset(sess, 0, sizeof(*sess));
	gen_random(sess->key, 16);
        sess->hash = hash_string(sess->key) % map->page_total;
        sess->created = time(NULL);
}

static inline void
sess_extend(struct sess *sess, unsigned int expires)
{
        unsigned int now = time(NULL);
        sess->expires = (expires <= (u32)~0U - now) ? now + expires : (u32)~0U;
}

static inline void
sess_crc(struct sess *sess)
{
        crc32_context ctx32;
        crc32_init(&ctx32, CRC_MODE_DEFAULT); /* Default algorithm (4K table) */
        crc32_update(&ctx32, (byte *)sess, CPU_PAGE_SIZE);
        sess->crc = crc32_final(&ctx32);
}

static inline u32
page_crc(struct page *page, unsigned long offset)
{
        crc32_context ctx32;
        crc32_init(&ctx32, CRC_MODE_DEFAULT); /* Default algorithm (4K table) */
        crc32_update(&ctx32, (byte *)page + offset, CPU_PAGE_SIZE - offset);
        return crc32_final(&ctx32);
}

static void
page_crc_update(struct page *page, u32 crc)
{
	unsigned char *p = (unsigned char *)page;
	snprintf(p + 36, 24, "%.16u", crc);
	p[PAGE_HEADER_SIZE] = '\n';
}

static int
handler(struct task *p)
{
        struct object *object = object_alloc_zero(sizeof(*object));

	unsigned int ms = 0;
	u32 err = 0;
	u32 ins = 0;

	timestamp_t now, expires, start = get_timestamp();

	do { 
	for (unsigned int i = 0; i < app->objects; i++) {
		char page_cache[CPU_PAGE_SIZE];

		now = get_timestamp();
		if (app->duration && ((now - start) > app->duration ))
			goto hup;

		expires = now + 360000;

		struct sess sess;
		sess_init(&sess);

		object_flush(object);

		/* setting up object attributes */
		attr_set_fmt(object, "page.id", "%.16u", sess.hash);
		attr_set_fmt(object, "page.crc", "%.16u", 0);
		attr_set(object, "page.key", sess.key);
		attr_set_fmt(object, "page.created", "%ju", (intmax_t)now);
		attr_set_fmt(object, "page.expires", "%ju", (intmax_t)expires);

		/* initialize page and put object into */
		memset(page_cache, '\n', CPU_PAGE_SIZE);	
		object_write(object, (const char *)page_cache, CPU_PAGE_SIZE);

		/* exclude page.id and page.crc header from crc */
		u32 crc = page_crc((struct page *)page_cache, PAGE_HEADER_SIZE);
		page_crc_update((struct page *)page_cache, crc);

		struct page *page = (struct page *)get_page(map, (unsigned int)sess.hash);
		memcpy(page, page_cache, CPU_PAGE_SIZE);

                list_for_each(struct attr *, a, object->attrs)
                        log_debug("%s: %s", a->key, a->val);

		log_debug("page_crc: %u", crc);

		ins++;

                if (sig_request_hup)
                        goto hup;
	}

	} while(app->duration);

hup:
	ms = get_timer(&start);

	log_info("inserted %d object(s) in %u ms", ins, ms);

	struct task *c = (struct task *)p->object.parent;

	/* exit status 0 decrements number of tasks in idle process 
	 * otherwise sub task will be started again */
	p->status = 0;
	sig_request_hup = 1;
}

static int
handler_check(struct task *p)
{
	struct object *object = object_alloc_zero(sizeof(*object));

        for (unsigned int i = 0; i < app->objects; i++) {
		struct page *page = (struct page *)get_page(map, i);

		char page_cache[CPU_PAGE_SIZE];
		memcpy(page_cache, page, CPU_PAGE_SIZE);

		u32 crc_page = page_crc((struct page *)page_cache, PAGE_HEADER_SIZE);

		object_flush(object);
		if (object_read(object, (byte*)page_cache, CPU_PAGE_SIZE) < 1)
			continue;

		const char *v; 
		if (!(v = attr_get(object, "page.crc")))
			continue;
	
		u32 crc_attr = atoi(v);

		log_debug("attr_crc: %u page_crc: %u", (unsigned int)crc_attr, (unsigned int)crc_page);

		list_for_each(struct attr *, a, object->attrs)
			log_debug("%s: %s", a->key, a->val);

	}

	object_free(object);

	p->status = 0;
	sig_request_hup = 1;
}

int
main(int argc, char *argv[])
{
	unsigned int ms;

	log_init();

	app = object_alloc_zero(sizeof(*app));
	app->file       = "./session.map";
	app->pages      = 1000;
	app->objects    = 1000;
	app->processes  = 1;
	app->threads    = 1;
	app->pages_miss = 3;
	app->tasks      = 1;
	app->handler    = handler;

        int opt;
        while ((opt = getopt_long(argc, argv, opt_cmd, opt_long, NULL)) >= 0) {
        	switch (opt) {
        	case OPT_HELP:
        	case '?':
                	usage(0);
			break;
		case OPT_INIT:
			app->init = 1;
			break;
        	case OPT_VERSION:
			fprintf(stderr, "%d\n", PACKAGE_VERSION);
			exit(1);
        	break;
        	default:
                	if (app->num_cmds >= MAX_CMDS)
                        	giveup("Too many commands specified");
                	app->cmd[app->num_cmds] = opt;
                	app->arg[app->num_cmds] = optarg;
                	app->num_cmds++;
        	}

	}

        if (optind < argc)
                usage(2);

        if (!app->num_cmds)
                nothing();

        for (unsigned int i = 0; i < app->num_cmds; i++) {
		int type;
                switch(app->cmd[i]) {
		case OPT_PROCESSES: 
			app->processes = atoi(app->arg[i]); 
			break;
		case OPT_THREADS: 
			app->threads = atoi(app->arg[i]);
			break;
		case OPT_CPU_MODEL:
                        if ((type = opt_find(cpu_model_names, ARRAY_SIZE(cpu_model_names), app->arg[i])) != -1)
                                app->cpu_model |= type;
                        else giveup("unknown access options");
			break;
		case OPT_DURATION:
			app->duration = atoi(app->arg[i]);
			break;
                case OPT_OBJECTS: app->objects = atoi(app->arg[i]); break;
		case OPT_PAGES: app->pages = atoi(app->arg[i]); break;
		case OPT_FILE:  app->file = mp_strdup(app->object.mp, app->arg[i]); break;
		case OPT_ACCESS: 
			if ((type = opt_find(access_type_names, ARRAY_SIZE(access_type_names), app->arg[i])) != -1)
				app->access_type |= type;
			else giveup("unknown access options"); 
		break;
		case OPT_CHECK:
			app->handler = handler_check;
			break;
                default:
                	break;
                }
        }

	log_info("setting up mapped table space");

	struct sched sched = {
		.cpu_model      = app->cpu_model,
		.task           = proc_self(),
		.vm             = TASK_VM_CLONE,
		.tasks          = app->processes,
	};

	sched_init(&sched);

        struct task_attrs attrs = {
                .name           = "map",
                .argv           = argv,
                .argc           = argc,
                .vm             = TASK_VM_CLONE,
                .cpu_affinity   = 1,
                .per_cpu_proc   = 1,
                .per_cpu_thread = 1,
                .max_procs      = 1,//app->tasks,
                .main           = app->handler
        };

        task_global_init(&attrs);

        timestamp_t now = get_timestamp();

        if ((map = mmap_open(app->file, PROT_WRITE | PROT_READ, app->pages, '\0', '\0')) == NULL)
                giveup("can not map %d pages in %s file", app->pages, app->file);

        ms = get_timer(&now);
        log_info("table space open %s %uM (%u pages) in %u ms ", app->file, pages2mb(map, app->pages), app->pages, ms);

	if (app->access_type & ACCESS_RANDOM)
		log_info("table space optimizing for random access");

	if (app->access_type & ACCESS_SEQUENTIAL)
		log_info("table space optimizing for sequential access");

	madvise(map->pages, map->size, app->access_type & ACCESS_RANDOM ? MADV_RANDOM : MADV_SEQUENTIAL);

	if (app->init) {
		now = get_timestamp();
		for (unsigned int i = 0; i < map->page_total; i++)
			memset(get_page(map, i), '\n', CPU_PAGE_SIZE);

		ms = get_timer(&now);
		log_info("table space init in %u ms ", ms);
	}

        task_global_wait();
        task_global_fini();

	now = get_timestamp();
	mmap_close(map);
	ms = get_timer(&now);
	log_info("table space commited in %u ms ", ms);

	log_fini();
	return 0;
}
