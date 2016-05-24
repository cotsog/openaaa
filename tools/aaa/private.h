/*
 * $private.h                                       Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 */

#ifndef __AAA_PRIVATE_H_H__
#define __AAA_PRIVATE_H_H__

#include <sys/compiler.h>
#include <sys/decls.h>

#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>

#include <ctypes/lib.h>
#include <sys/types.h>
#include <ctypes/object.h>

__BEGIN_DECLS

enum opt_long_e {
	OPT_HELP         = 'h',
	OPT_VERSION      = 'V',
	OPT_CPU_AFFINITY = 'A',
	OPT_CPU_MODEL    = 'M',
	OPT_PROCESSES    = 'T',
	OPT_THREADS      = 't',
	OPT_SOURCE       = 'S',
	OPT_PAGES        = 'p',
	OPT_BENCHMARK    = 'B',
	OPT_VERBOSE      = 'v',
	OPT_BIND         = 'b',
	OPT_COMMIT       = 'c',
	OPT_SET          = 's',
	OPT_GET          = 'g',
	OPT_DUMP         = 'd',
	OPT_ID           = 'i',
	OPT_KEY          = 'k',
	OPT_VAL          = 'a',
	OPT_TEST_OPENSSL = '1',
};

struct cmd {
	const char *name;
	s16 args;
	int (*handler)(struct cmd *cmd, const char *arg);
	char opt;
	const char *help;
};

struct app {
	struct object object;
	struct map *map;
	const char *id;
	const char *key;
	const char *val;
	bool verbose;
#define MAX_CMDS 16
	unsigned int cmd[MAX_CMDS];
	const char *arg[MAX_CMDS];
	unsigned int num_cmds;
};

extern const char *opt_cmd;
extern struct option opt_long[];
extern struct app *app;

int                                                                             
benchmark_map(void);

void
benchmark(void);

void
openssl_test(void);

void
cmd_do(const char *line);

void
usage(int code);

void
console_input(void);

__END_DECLS

#endif/*__AAA_PRIVATE_H_H__*/
