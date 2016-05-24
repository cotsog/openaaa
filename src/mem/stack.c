#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <mem/alloca.h>

unsigned int
stk_printf_internal(const char *fmt, ...)
{
	unsigned int len = 256;
	char *b = alloca(len);
	va_list args, args2;
	va_start(args, fmt);
	for (;;) {
		va_copy(args2, args);
		int l = vsnprintf(b, len, fmt, args2);
		va_end(args2);
		if (l < 0)
			len *= 2;
		else {
			va_end(args);
			return l + 1;
		}
		b = alloca(len);
	}
}

unsigned int
stk_vprintf_internal(const char *fmt, va_list args)
{
	unsigned int len = 256;
	char *buf = alloca(len);
	va_list args2;
	for (;;) {
		va_copy(args2, args);
		int l = vsnprintf(buf, len, fmt, args2);
		va_end(args2);
		if (l < 0)
			len *= 2;
		else {
			va_end(args);
			return l + 1;
		}
		buf = alloca(len);
	}
}
