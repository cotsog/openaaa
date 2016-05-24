#include "dlwrap.h"
#include <ctypes/lib.h>

int
dlsym_wrap(struct dlsym_struct *sym)
{
	TRACED_HOOK_HANDLE hook = &sym->info;
	DWORD acl[1] = {-1};

	if (LhInstallHook(sym->orig, sym->wrap, (void *)0, hook))
		goto failed;

	if (LhSetExclusiveACL(acl, 1, hook))
		goto failed;
/*
	if (LhSetInclusiveACL(acl, 1, hook))
		goto failed;
*/
	return 0;
failed:
	return -1;	
}
