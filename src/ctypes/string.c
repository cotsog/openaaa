#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <ctypes/lib.h>
#include <ctypes/string.h>
#include <mem/map.h>
#include <mem/stack.h>
#include <mem/alloca.h>

#define MEM_TO_HEX_UPCASE 0x100

void
mem_to_hex(char *dest, const byte *src, size_t bytes, uint flags)
{
	uint sep = flags & 0xff;

	while (bytes--) {
		dest[0] = hex_make(*src >> 4);
		dest[1] = hex_make(*src & 0x0f);
		if (flags & MEM_TO_HEX_UPCASE) {
			dest[0] = toupper(dest[0]);
			dest[1] = toupper(dest[1]);
		}
		dest += 2;
		if (sep && bytes)
			*dest++ = sep;
		src++;
	}
	*dest = 0;
}

/*
static uint
hex_parse(uint c)
{
	c = toupper(c);
	c -= '0';
	return (c < 10) ? c : (c - 7);
}
*/

const char *
hex_to_mem(byte *dest, const char *src, size_t max_bytes, uint flags)
{
	while (max_bytes--) { 
		*dest++ = (hex_parse(src[0]) << 4) | hex_parse(src[1]);
		src += 2; 
	} 

	return src;
}

char *
str_unesc(char *d, const char *s)
{
  while (*s)
    {
      if (*s == '\\')
        switch (s[1])
          {
            case 'a': *d++ = '\a'; s += 2; break;
            case 'b': *d++ = '\b'; s += 2; break;
            case 'f': *d++ = '\f'; s += 2; break;
            case 'n': *d++ = '\n'; s += 2; break;
            case 'r': *d++ = '\r'; s += 2; break;
            case 't': *d++ = '\t'; s += 2; break;
            case 'v': *d++ = '\v'; s += 2; break;
            case '\?': *d++ = '\?'; s += 2; break;
            case '\'': *d++ = '\''; s += 2; break;
            case '\"': *d++ = '\"'; s += 2; break;
            case '\\': *d++ = '\\'; s += 2; break;
            case 'x':
              if (!isdigit(s[2]))
                {
                  s++;
                }
              else
                {
                  char *p;
                  uint v = strtoul(s + 2, &p, 16);
                  if (v <= 255)
                    *d++ = v;
                  else
                    sys_dbg("hex escape sequence out of range");
                  s = (char *)p;
                }
              break;
            default:
              if (s[1] >= '0' && s[1] <= '7')
                {
                  uint v = s[1] - '0';
                  s += 2;
                  for (uint i = 0; i < 2 && *s >= '0' && *s <= '7'; s++, i++)
                    v = (v << 3) + *s - '0';
                  if (v <= 255)
                    *d++ = v;
                  else
                    sys_dbg("octal escape sequence out of range");
                }
              else
                *d++ = *s++;
              break;
          }
      else
        *d++ = *s++;
    }
  *d = 0;
  return d;
}

size_t
value_units(const char *s)
{
	char m;
	double v = atof(s);
	m = s[strlen(s) - 1];
	switch (m) {
	case 'G':
	case 'g':
	default:
		v *= 1024*1024;
		break;
	case 'M':
	case 'm':
		v *= 1024;
		break;
	case 'K':
	case 'k':
		v *= 1;
		break;
	}
	return (size_t)v;
}

int
printf_asynch_safe(const char *fmt, ...)
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
			l = vsnprintf(b, len, fmt, args);
			int rv = write(0, b, l);
			va_end(args);
			return l + 1;
		}
		b = alloca(len);
	}
}
