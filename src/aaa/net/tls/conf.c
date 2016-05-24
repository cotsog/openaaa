/*
 * $conf.c                                          Daniel Kubec <niel@rtfm.cz>
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

struct cf_tls_rfc5705 {
	char *context;
	char *label;
	unsigned int length;
};

struct cf_tls_rfc5705 cf_tls_rfc5705 = {
	.context = "OpenAAA",
	.label   = "EXPORTER_AAA",
	.length  = 16
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

static void 
_constructor _init_tls_section(void)
{
	cf_declare_section("TLS", &tls_section, 0);
}
