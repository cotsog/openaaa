/*
 * (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <marvin/lib.h>
#include <marvin/kernel.h>
#include <marvin/mpm/sched.h>

#include <marvin/mem/page.h>
#include <marvin/mem/map.h>

#include <marvin/hash/fn.h>
#include <marvin/hash/table.h>

#include <errno.h>
#include <getopt.h>

const char *package_name = "map-table";

enum { 
        OPT_HELP,
        OPT_VERSION,
	OPT_CONFIG,
	OPT_MAP_TABLE,
};

static const char *opt_cmd = "h:vm:";
static struct option opt_long[] = {
        {"help",      0, 0, OPT_HELP    },
        {"version",   0, 0, OPT_VERSION },
	{"map-table", 1, 0, OPT_MAP_TABLE},
};

NONRET static void
usage(int code)
{
        printf("Usage: %s <commands-and-parameters>\n\n", package_name);
	printf("\t--version, -v\t\t\t Package version\n");
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
#define MAX_CMDS 16
        unsigned int cmd[MAX_CMDS];
        const char *arg[MAX_CMDS];
        unsigned int num_cmds;
};

static void
map_table(const char *file)
{
	struct table_attrs attrs = {
		.bucket_shift = 12,
		.page_shift   = 12,
		.bucket_total = 4U << 16,
		.page_total   = 4U << 16
	};

	u64 page_size = 1U << attrs.page_shift;
	u64 bucket_size = 1U << attrs.bucket_shift;
	u64 size = attrs.page_total * page_size;

	printf("hash table mapping size: %ld\n", size);
	printf("page_size: %ld bucket_size: %ld\n", page_size, bucket_size);
	printf("page_total: %ld bucket_total: %ld\n", attrs.page_total, attrs.bucket_total);

	struct table *table = table_open("./var/object.map", &attrs);

	table_close(table);
}

int
main(int argc, char *argv[])
{
	log_init();

	struct app *app = object_alloc_zero(sizeof(*app));

        int opt;
        while ((opt = getopt_long(argc, argv, opt_cmd, opt_long, NULL)) >= 0) {
        	switch (opt) {
        	case OPT_HELP:
        	case '?':
                	usage(0);
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
		switch(app->cmd[i]) {
		case OPT_MAP_TABLE:
			map_table(app->arg[i]);
		break;
		default:
		break;
		}
	}

	log_fini();
	return 0;
}
