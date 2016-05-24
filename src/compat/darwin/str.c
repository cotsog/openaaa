#include <stdlib.h>

unsigned int
strnlen(const char *str, unsigned int n)
{
	const char *end = str + n, *c;
	for (c = str; *c && c < end; c++);
	return c - str;
}
