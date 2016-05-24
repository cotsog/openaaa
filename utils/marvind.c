/*
 * marvind.c                        (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/lib.h>
#include <sys/list.h>
#include <sys/object.h>
#include <sys/crc.h>
#include <sys/mpm/sched.h>
#include <sys/mem/page.h>
#include <sys/mem/map.h>
#include <sys/getopt/getopt.h>

#include <marvin/lib.h>
#include <marvin/db.h>

static const char package_name[] = "marvind";

static int processes = 4;
static int cpu_model = 0;

enum { 
        OPT_HELP    = 'h',
        OPT_VERSION = 'V',
	OPT_VERBOSE = 'v',
	OPT_CONFIG  = 'f',
	OPT_DAEMON  = 'd',
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

static const char * const table_type_names[] = {
	[TABLE_BINARY]     = "binary",    [TABLE_TEXT]              = "text",
};

static const char *opt_cmd = "h:Vfd";
static struct option opt_long[] = {
        {"help",      0, 0, OPT_HELP    },
        {"version",   0, 0, OPT_VERSION },
	{"verbose",   0, 0, OPT_VERBOSE},
        {"config",    1, 0, OPT_CONFIG  },
	{"daemon",    0, 0, OPT_DAEMON  },
        { NULL,       0, 0, 0 }
};

struct app {
        struct object object;
        int (*handler)(struct task *);
        const char *file;
        int init;
        enum cpu_model_e cpu_model;
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
} a;

struct app *app = &a;

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
	printf("\t--processes,    -x\t\t\t number of processes\n");
	printf("\t--threads,      -y\t\t\t number of threads\n");

	printf("\t--file,         -f\t\t\t table space file\n");
	opt_print("--access        -a", access_type_names, ARRAY_SIZE(access_type_names));
	printf("\t--objects,      -o\t\t\t number of inserted objects\n");
	printf("\t--pages,        -p\t\t\t pages in file based table\n");
	printf("\n");
	exit(code);
}

static void
server(void)
{
//	wait_for_completion();

	return;
}

static void
configure(int argc, char *argv[])
{
        int opt;
        while ((opt = getopt_long(argc, argv, opt_cmd, opt_long, NULL)) >= 0) {
                switch (opt) {
                case OPT_HELP:
                case '?':
                        usage(0);
                        break;
                case OPT_VERSION:
                        fprintf(stderr, "%d\n", PACKAGE_VERSION);
                        exit(1);
		case OPT_DAEMON:
			server();
			break;
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
                usage(1);
}

int
main(int argc, char *argv[])
{
	/* disable interrupt requests */
	irq_disable();
	/* configure from default file */
	configure(argc, argv);
	return EXIT_SUCCESS;
}
