/*
 * $id: string.h                                    Daniel Kubec <niel@rtfm.cz>
 *                                                                              
 * This software may be freely distributed and used according to the terms      
 * of the GNU Lesser General Public License.                                    
 */

#ifndef __C_POSIX_STRING_H__
#define __C_POSIX_STRING_H__

char *
str_unesc(char *d, const char *s);

static inline void
str_collapse(char *str)
{
	char *a = str, *b = str;
	do 
		while (*b == ' ') b++;
	while ((*a++ = *b++));
}

void
mem_to_hex(char *dest, const byte *src, size_t bytes, uint flags);

const char *
hex_to_mem(byte *dest, const char *src, size_t max_bytes, uint flags);

char *
url_encode(char *str);

#endif
