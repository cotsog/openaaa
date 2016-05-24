
/*
 * $sysconf.c                                       Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 *
 */

#include <sys/compiler.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctypes/lib.h>
#include <ctypes/list.h>

#include <getopt/getopt.h>
#include <getopt/opt.h>
#include <getopt/conf.h>

#include <mem/map.h>
#include <mem/page.h>
#include <mem/pool.h>
#include <mem/stack.h>

#include <dlfcn.h>

static char short_opts[] = "vV";
static const struct option long_opts[] = {
	{ "verbose", 0, 0, 'v' },
	{ NULL, 0, 0, 0 }
};

static void
version(struct opt_item *opt, const char *value, void *data)
{
	printf("Portable sysconf utility v0.1\n");
	exit(EXIT_SUCCESS);
}

static struct opt_section options = {
OPT_ITEMS {
	OPT_HELP(""),
	OPT_HELP("Options:"),
	//OPT_CALL('V', "version", version, NULL, OPT_NO_VALUE, "\tShow the version"),
	OPT_END
}
};

static char *hw_text = "Hello world";
static int hw_count = 1;
static int hw_wait_answer = 0;
static struct cf_section hw_config = {
CF_ITEMS {
	CF_STRING("Text", &hw_text),
	CF_INT("Count", &hw_count),
	CF_INT("WaitAnswer", &hw_wait_answer),
	CF_END
}
};

static void _constructor hw_init(void) {
	cf_declare_section("HelloWorld", &hw_config, 0);
}

static inline void
str_collapse(char *str)
{
	char *a = str, *b = str;
	do while (*b == ' ') b++;
		while ((*a++ = *b++));
}

static inline void
rm_newline_char(const char *str)
{
	for(char *p = (char *)str; *p; p++)
		if (*p == '\n' || *p == '\r')
			*p = ' ';
}

static void 
parse_session(const char *file)
{
	char n[4096] = {0}, v[4096] = {0};

	FILE *f = fopen(file, "r");
	if (f == NULL || feof(f))
		return;

	do {
		*n = 0;
		*v = 0;
		
		fscanf(f, "%s %s", n, v);
		rm_newline_char(n);
		rm_newline_char(v);
		str_collapse(n);
		str_collapse(v);

		if (!*n || !*v)
			break;

		printf("%s=%s\n", n, v);
	} while(!feof(f));

	fclose(f);
}

int
main(int argc, char *argv[])
{
	char *cf_def_file = "etc/sysconf";
	opt_parse(&options, argv + 1);

	const char *file = "/tmp/aaa-db4d382e63d6ae733e10";
	parse_session(file);

	return EXIT_SUCCESS;
}
