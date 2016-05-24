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

const char *package_name = "ssl-accept";

enum { 
        OPT_HELP,
        OPT_VERSION,
	OPT_CONFIG,
	OPT_CPU_AFFINITY,
	OPT_CPU_MODEL,
	OPT_PROCESSES,
	OPT_THREADS,
	OPT_PORT,
	OPT_VERBOSE,
};


static const char *opt_cmd = "h:vm:";
static struct option opt_long[] = {
        {"help"         ,0, 0, OPT_HELP        },
        {"version"      ,0, 0, OPT_VERSION     },
	{"cpu-model"    ,1, 0, OPT_CPU_MODEL   },
	{"cpu-affinity" ,1, 0, OPT_CPU_AFFINITY},
	{"processes"    ,1, 0, OPT_PROCESSES   },
	{"threads"      ,1, 0, OPT_THREADS     },
	{"port"         ,1, 0, OPT_PORT        },
	{"verbose"      ,0, 0, OPT_VERBOSE     },
};

static const char * const cpu_model_names[] = {
        [CPU_DEFAULT]      = "default",   [CPU_SCALABILITY]         = "scalability",
        [CPU_SHARED]       = "shared",    [CPU_DEDICATED]           = "dedicated",
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
        printf("\t--processes,    -x\t\t\t number of processes\n");
        printf("\t--threads,      -y\t\t\t number of threads\n");

	printf("\t--time-max,     -t\t\t\t finish test after time-max miliseconds \n");

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
	const char *file;
	enum cpu_model cpu_model;
	u32 processes;
	u32 threads;
	u16 port;
	bool verbose;
#define MAX_CMDS 16
        unsigned int cmd[MAX_CMDS];
        const char *arg[MAX_CMDS];
        unsigned int num_cmds;
};

static struct app *app;

int
main(int argc, char *argv[])
{
	unsigned int ms;

	log_init();

	app = object_alloc_zero(sizeof(*app));
	app->port = 3000;

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
		case OPT_PORT:
			app->port = atoi(app->arg[i]);
			break;
                default:
                break;
                }
        }

	log_fini();
	return 0;
}
