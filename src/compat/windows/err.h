#ifndef __WINDOWS_ERR_H__
#define __WINDOWS_ERR_H__

#define ERROR_FMT FORMAT_MESSAGE_ALLOCATE_BUFFER |\
                  FORMAT_MESSAGE_FROM_SYSTEM |\
                  FORMAT_MESSAGE_IGNORE_INSERTS

#define sys_call_printf printf

#define sys_call(fn, rv, args...) \
	if (fn(args) != rv) { goto sys_error; }

#define sys_call_trace(fn, rv, args...) \
        if (fn(args) != rv) { sys_call_printf("%s\n", #fn); sys_call_error(#fn); goto sys_error; }

void sys_call_error(char *func);

#endif
