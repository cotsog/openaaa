#include <sys/compiler.h>
#include <sys/cpu.h>
#include <ctypes/lib.h>
#include <mem/alloc.h>
#include <stdlib.h>
#include <string.h>
	
char *
strndupa(const char *s, size_t n)
{
	char *str;
	size_t len = strlen (s);

	if (n < len)
		len = n;

	if (!(str = (char *)xmalloc (len + 1)))
		return 0;

	str[len] = '\0';
	return (char *)memcpy(str, s, len);
}
