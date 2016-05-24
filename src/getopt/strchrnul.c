
#include <string.h>

char *
strchrnul(const char *s, int in)
{
	char c = in;
	while (*s && (*s != c))
		s++;

	return (char *)s;
}
