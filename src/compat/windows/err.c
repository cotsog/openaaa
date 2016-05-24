#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#define ERROR_FMT FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS

void
sys_call_error(char *func) 
{ 
	LPTSTR err = NULL;
	DWORD dw = GetLastError(); 
	WORD lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

	if (FormatMessage(ERROR_FMT,NULL,dw, lang, err, 0, NULL )) {
		fprintf(stderr, "FormatMessage() failed");
	}

	fprintf(stderr, "%d %d %s", (int)dw, (int)err, (char *)err);

	if (err)
		LocalFree(err);
}
