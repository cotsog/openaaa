/*
 *  (c) 2014                                        Daniel Kubec <niel@rtfm.cz>
 *
 *  This software may be freely distributed and used according to the terms
 *  of the GNU Lesser General Public License.
 */

#define _GNU_SOURCE 1

#include "nptypes.h"
#include "npfunctions.h"
#include "npapi.h"
#include "npfunctions.h"

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ctypes/lib.h>
#include <ctypes/string.h>
#include <mem/stack.h>
#include <mem/pool.h>
#include <hash/fn.h>

#include <aaa/lib.h>
#include <net/tls/lib.h>
#include <net/vpn/lib.h>

#include <windows.h>

static NPNetscapeFuncs* browser;

static void test(void)
{
	FILE *fp = fopen ("C:\\file.txt", "w");
	fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
	fclose(fp);
}

NPError OSCALL
NP_GetEntryPoints(NPPluginFuncs *pFuncs)
{
	return NPERR_NO_ERROR;
}

NPError OSCALL
NP_Initialize(NPNetscapeFuncs *aNPNFuncs)
{
	test();
	browser = aNPNFuncs;
	return NPERR_NO_ERROR;
}

NPError OSCALL
NP_Shutdown(void)
{
	return NPERR_NO_ERROR;
}

const char * 
NP_GetMIMEDescription(void) 
{
	return "application/x-aaa::OpenAAA MIME";
}

char*
NP_GetPluginVersion()
{
	return "1.0.0.0";
}

NPError 
NP_GetValue(NPP instance, NPPVariable variable, void *value)
{
	switch (variable) {
	case NPPVpluginNameString:
		*(const char **)(value) = "OpenAAA";
		return NPERR_NO_ERROR;
	case NPPVpluginDescriptionString:
		*(const char **)(value) = "OpenAAA";
		return NPERR_NO_ERROR;
	default:
		return NPERR_INVALID_PARAM;
	}
	return NPERR_NO_ERROR;
}
