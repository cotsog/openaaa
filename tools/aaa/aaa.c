/*
 * $aaa.c                                          Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

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
#include <aaa/lib.h>
#include <aaa/private.h>

#include <dlfcn.h>

#ifndef CONFIG_HAVE_LIBEDIT
void console_input(void) {}
#endif

static void
show_version(struct opt_item *opt, const char *value, void *data)
{
	printf("This is a simple tea boiling console v0.1.\n");
	exit(EXIT_SUCCESS);
}

struct cf_tls_rfc5705 {
	char *context; 
	char *label; 
	unsigned int length;
};

struct cf_tls_rfc5705 cf_tls_rfc5705 = {
	.context = "OpenAAA",
	.label   = "EXPORTER_AAA",
	.length  = 8
};

static struct cf_section cf_section_rfc5705 = {
CF_ITEMS {
	CF_STRING("Label",   &cf_tls_rfc5705.label),
	CF_STRING("Context", &cf_tls_rfc5705.context),
	CF_UINT  ("Length",  &cf_tls_rfc5705.length),
	CF_END
	}
};

static struct cf_section tls_section = {
CF_ITEMS {
	CF_SECTION("KeyingMaterialExporter", NULL, &cf_section_rfc5705),
	CF_END
}
};

static void _constructor _init_tls_section(void) {
	cf_declare_section("TLS", &tls_section, 0);
}

static void
version(struct opt_item *opt, const char *value, void *data)
{
	printf("Portable sysconf utility v0.1\n");
	exit(EXIT_SUCCESS);
}

static struct opt_section options = {
OPT_ITEMS {
	OPT_HELP(""),
//	OPT_CALL('V', "version", version, NULL, OPT_NO_VALUE, "\tShow the version"),
OPT_END
}
};

_noreturn static void
nothing(void)
{
	console_input();
	exit(0);
}

int
main(int argc, char *argv[])
{
        char *cf_def_file = "etc/aaa";
	int rv = cf_load(cf_def_file);
	printf("load=%d\n", rv);
        opt_parse(&options, argv + 1);

	printf("%s %d \n", cf_tls_rfc5705.label, cf_tls_rfc5705.length);

	return EXIT_SUCCESS;
}
