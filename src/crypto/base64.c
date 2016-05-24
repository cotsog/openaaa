#include <ctypes/lib.h>
#include <stdlib.h>
#include <ctype.h>

static const byte base64_table[] =
{
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const byte base64_pad = '=';

unsigned int
base64_encode(byte *dst, const byte *src, unsigned int len)
{
	const byte *c = src;
	unsigned int i = 0;

	while (len > 2) { /* keep going until we have less than 24 bits */
		dst[i++] = base64_table[c[0] >> 2];
		dst[i++] = base64_table[((c[0] & 0x03) << 4) + (c[1] >> 4)];
		dst[i++] = base64_table[((c[1] & 0x0f) << 2) + (c[2] >> 6)];
		dst[i++] = base64_table[c[2] & 0x3f];
		c += 3;
		len -= 3; /* we just handle 3 octets of data */
	}
	/* now deal with the tail end of things */
	if (len != 0) {
		dst[i++] = base64_table[c[0] >> 2];
		if (len > 1) {
			dst[i++] = base64_table[((c[0] & 0x03) << 4) + (c[1] >> 4)];
			dst[i++] = base64_table[(c[1] & 0x0f) << 2];
			dst[i++] = base64_pad;
		} else {
			dst[i++] = base64_table[(c[0] & 0x03) << 4];
			dst[i++] = base64_pad;
			dst[i++] = base64_pad;
		}
	}
	return i;
}
