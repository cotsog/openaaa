/*
 * $lib.h                                           Daniel Kubec <niel@rtfm.cz> 
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 */

#ifndef __ARCH_LIB_H__
#define __ARCH_LIB_H__

#include <sys/compiler.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/decls.h>
#include <hash/fn.h>
#include <hash/list.h>
#include <hash/table.h>

__BEGIN_DECLS

/* API version, they compare as integers */
#define API_VERSION PACKAGE_VERSION

struct arch_sym {
	struct hlist_node node;
	struct hlist_node hash;
        unsigned char gate[128];
        unsigned char pre_gate[128];
        unsigned char hook_data[32];
	const char *name;
	const char *version;
	void *org;
	void *act;
};

int
arch_interpose(struct arch_sym *sym, bool enable);

#define arch_define(rv, fn, ...) \
	rv (*arch_org_##fn)(__VA_ARGS__); \
	rv (*arch_tramp_##fn)(__VA_ARGS__); \
	rv arch_act_##fn(__VA_ARGS__)
#define arch_sym_trace 1
#define arch_sym_define(rv, fn, args...) \
	rv ((*sym_org_##fn)(args)); struct arch_sym arch_sym_##fn;
#define arch_sym_declare(rv, fn) \
	rv sym_act_##fn
#define arch_dso_define(ns) \
	DECLARE_HASHTABLE(arch_dso_##ns, 12); \
        LIST_HEAD(arch_sym_list_##ns);
#define arch_dso_declare(ns) \
	void arch_dso_table##ns (void)
#define arch_dso_init(ns) \
	arch_dso_table##ns();
#define arch_sym_link(fn) \
	sym_org_##fn = dlsym((void *)RTLD_DEFAULT, #fn); \
	printf("%s=%p\n", #fn, sym_org_##fn);\
        arch_sym_##fn.org = sym_org_##fn; arch_sym_##fn.act = sym_act_##fn; \
        arch_sym_##fn.name = #fn;
#define arch_call_org(fn) \
	sym_org_##fn
#define arch_call_act(fn) \
	sym_act_##fn

__END_DECLS

#endif/*__ARCH_LIB_H__*/
