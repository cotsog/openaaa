/*
 * $cmd.c                                           Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 *
 */

#include <sys/missing.h>
#include <sys/compiler.h>
#include <sys/cpu.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include <mem/map.h>
#include <mem/page.h>
#include <mem/pool.h>
#include <mem/stack.h>
#include <hash/fn.h>

#include <aaa/lib.h>
#include <aaa/private.h>

#include "private.h"

const char *opt_cmd = "hVAMTtS:P:j:p:Bvbcsdi:k:a:";

static int
cmd_help(struct cmd *cmd, const char *args)
{
	usage(0);
	return 0;
}

static int
cmd_version(struct cmd *cmd, const char *args)
{
	return 0;
}

static int
cmd_source(struct cmd *cmd, const char *args)
{
	return 0;
}

static int
cmd_set(struct cmd *cmd, const char *args)
{
	return 0;
}

static int
cmd_get(struct cmd *cmd, const char *args)
{
	return 0;
}

static int
cmd_dump(struct cmd *cmd, const char *args)
{
	list_for_each(struct attr *, a, app->object.attrs)
		printf("%-15s: %.64s\n", a->key, a->val);

	return 0;
}

static const char * const def_help[] = {
	[OPT_HELP]    = "show this help",
	[OPT_VERSION] = "package version",
};

struct cmd cmd[] = {
	{"help",    no_argument,       cmd_help,    OPT_HELP,    NULL},
	{"version", no_argument,       cmd_version, OPT_VERSION, NULL},
	{"source",  required_argument, cmd_source,  OPT_SOURCE,  NULL},
	{"set",     required_argument, cmd_set,     OPT_SET,     NULL},
	{"get",     required_argument, cmd_get,     OPT_GET,     NULL},
	{"dump",    no_argument,       cmd_dump,    OPT_DUMP,    NULL},
	{0, 0, 0, 0}
};

struct option opt_long[] = {
	{"help"         ,0, 0, OPT_HELP        },
	{"version"      ,0, 0, OPT_VERSION     },
	{"source"       ,0, 0, OPT_SOURCE      },
	{"pages"        ,1, 0, OPT_PAGES       },
	{"benchmark"    ,0, 0, OPT_BENCHMARK   },
	{"verbose"      ,0, 0, OPT_VERBOSE     },
	{"bind"         ,0, 0, OPT_BIND        },
	{"commit"       ,0, 0, OPT_COMMIT      },
	{"set"          ,0, 0, OPT_SET         },
	{"get"          ,0, 0, OPT_GET         },
	{"dump"         ,1, 0, OPT_DUMP        },
	{"id"           ,1, 0, OPT_ID          },
	{"key"          ,1, 0, OPT_KEY         },
	{"val"          ,1, 0, OPT_VAL         },
	{"test-openssl" ,0, 0, OPT_TEST_OPENSSL},
	{0, 0, 0, 0}
};

void
cmd_do(const char *line)
{
	for (int i = 0; i < _array_size(cmd); i++) {
		if (!cmd[i].name || !cmd[i].handler)
			continue;

		if (strncmp(cmd[i].name, line, strlen(cmd[i].name)))
			continue;

		cmd[i].handler(&cmd[i], line);
	}
}
