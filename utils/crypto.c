#include <marvin/lib.h>
#include <marvin/kernel.h>
#include <marvin/mpm/sched.h>
#include <errno.h>
#include <getopt.h>

const char *package_name = "crypto";

enum { 
        OPT_HELP,
        OPT_VERSION,
	OPT_CONFIG,
	OPT_FIND_PROC,
	OPT_MODULE,
	OPT_LIB,
	OPT_HIJACK
};

static const char *opt_cmd = "h:vm:";
static struct option opt_long[] = {
        {"help",      0, 0, OPT_HELP    },
        {"version",   0, 0, OPT_VERSION },
	{"find-proc", 1, 0, OPT_FIND_PROC},
	{"module",    1, 0, OPT_MODULE},
        { NULL,       0, 0, 0 }
};

static void
usage(int code)
{
        printf("Usage: %s <commands-and-parameters>\n\n", package_name);
	printf("\t--version, -v\t\t\t Package version\n");
	printf("\t--module, -m\t\t\t Module in specified process\n");
	printf("\t--find-proc\t\t\t Find pid of specified process name\n");
        exit(code);
}

struct app {
        struct object object;
	const char *lib;
	const char *module;
	unsigned int pid;
#define MAX_CMDS 16
        unsigned int cmd[MAX_CMDS];
        const char *arg[MAX_CMDS];
        unsigned int num_cmds;
};

int
cmd_hijack(void)
{
}

int
cmd_find_proc_pid(const char *name)
{
	unsigned long pid = find_proc_pid(name);
	printf("checking for %s pid: %ld\n", name, pid);

	if (pid == 0)
		return -EINVAL;

	

	int fd = find_module(pid, "ssl3.dll");
	return 0;
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

        if (!app->num_cmds) {
                printf("Nothing to do.\n");
                exit(0);
        }

	for (unsigned int i = 0; i < app->num_cmds; i++) {
		switch(app->cmd[i]) {
		case OPT_FIND_PROC:
			cmd_find_proc_pid(app->arg[i]);
		break;
		case OPT_MODULE:
			app->module = mp_strdup(app->object.mp, optarg);
		break;
		case OPT_LIB:
			app->lib = mp_strdup(app->object.mp, optarg);
		break;
		default:
		break;
		}
	}

	log_fini();
	return 0;
}
