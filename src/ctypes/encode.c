#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <mem/pool.h>


static char 
from_hex(char c) {
	return isdigit(c) ? c - '0' : tolower(c) - 'a' + 10;
}

static char
to_hex(char c) 
{
	static char h[] = "0123456789abcdef";
	return h[c & 15];
}

char *
url_encode(char *str) 
{
	char *s = str, *buf = malloc(strlen(str) * 3 + 1), *p = buf;
	while (*s) {
		if (isalnum(*s) || *s == '-' || *s == '_' || *s == '.' || *s == '~') 
			*p++ = *s;
		else if (*s == ' ') 
			*p++ = '+';
		else 
			*p++ = '%', *p++ = to_hex(*s >> 4), *p++ = to_hex(*s & 15);
		s++;
	}

	*p = '\0';
	return buf;
}

char *
url_decode(char *str) 
{
	char *s = str, *buf = malloc(strlen(str) + 1), *p = buf;
	while (*s) {
		if (*s == '%') {
			if (s[1] && s[2]) {
				*p++ = from_hex(s[1]) << 4 | from_hex(s[2]);
				s += 2;
			}
		} else if (*s == '+') { 
			*p++ = ' ';
		} else {
			*p++ = *s;
		}
		s++;
	}
	*p = '\0';

	return buf;
}

char *
mp_url_encode(struct mempool *mp, char *str)
{
	char *s = str, *buf = mp_alloc(mp, strlen(str) * 3 + 1), *p = buf;
	while (*s) {
		if (isalnum(*s) || *s == '-' || *s == '_' || *s == '.' || *s == '~')
			*p++ = *s;
		else if (*s == ' ')
			*p++ = '+';
		else
			*p++ = '%', *p++ = to_hex(*s >> 4), *p++ = to_hex(*s & 15);
		s++;
	}
	*p = '\0';
	return buf;
}

char *
mp_url_decode(struct mempool *mp, char *str)
{
	char *s = str, *buf = mp_alloc(mp, strlen(str) + 1), *p = buf;
	while (*s) {
		if (*s == '%') {
			if (s[1] && s[2]) {
				*p++ = from_hex(s[1]) << 4 | from_hex(s[2]);
				s += 2;
			}
		} else if (*s == '+') {
			*p++ = ' ';
		} else {
			*p++ = *s;
		}
		s++;
	}
	*p = '\0';
	return buf;
}
