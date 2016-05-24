/*
 * $id: mem/stack.h                                 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __SYS_MEM_STACK_H__
#define __SYS_MEM_STACK_H__

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>
#include <mem/alloca.h>

__BEGIN_DECLS

unsigned int
stk_printf_internal(const char *x, ...);

unsigned int
stk_vprintf_internal(const char *fmt, va_list args);

int
printf_asynch_safe(const char *fmt, ... );

#define stk_strdup(s) \
({\
	const char *_s = (s); unsigned int _l = strlen(_s) + 1; \
	char *_x = alloca(_l); memcpy(_x, _s, _l); _x; \
})

#define stk_strndup(s, n) \
({\
	const char *_s = (s); unsigned int _l = strnlen(_s,(n)); \
	char *_x = alloca(_l+1); \
	memcpy(_x, _s, _l); _x[_l] = 0; _x; \
})

#define stk_strcat(s1, s2) \
({\
	const char *_s1 = (s1); const char *_s2 = (s2); \
	unsigned int _l1 = strlen(_s1); \
	unsigned int _l2 = strlen(_s2); \
	char *_x = alloca(_l1+_l2+1); memcpy(_x,_s1,_l1); \
	memcpy(_x + _l1,_s2,_l2+1); _x; \
})

#define stk_printf(...) \
({\
	unsigned int _l = stk_printf_internal(__VA_ARGS__); \
	char *_x = (char *)alloca(_l); sprintf(_x, __VA_ARGS__); _x; \
})

#define stk_vprintf(f, args) \
({\
	unsigned int _l = stk_vprintf_internal(f, args); \
	char *_x = alloca(_l); vsprintf(_x, f, args); _x; \
})

#define stk_str_unesc(s) \
({\
	const char *_s = (const char *)(s); \
	char *_d = (char *)alloca(strlen(_s)+1); str_unesc(_d, _s); _d; \
})

#define stk_decimal(addr, bytes) \
({ \
 	int __bytes = (bytes); \
	char *__d = alloca(((__bytes) * 5) + 1); \
	unsigned char *__s = (unsigned char *)addr; \
	char *_ds = __d; \
	for (unsigned int __i = 0; __i < __bytes; __i++) { \
		char __num[6]; \
		snprintf(__num, sizeof(__num) - 1, "%d", (int)*__s++); \
 		if (__i) *_ds++ = ' '; \
		for (int __x = 0; __x < strlen(__num); __x++) { \
			*_ds++ = __num[__x]; \
		} \
	} \
	*_ds++ = 0; \
	__d; \
})


#define stk_mem_to_hex(src, bytes) \
({\
	char *__d = alloca((bytes * 2) + 1 );\
	char *_ds = __d;\
	static const char hex_table[] = "0123456789abcdef";\
	const byte *in = (const byte*)src;\
	for (unsigned int i = 0; i < bytes; i++) {\
		*_ds++ = hex_table[in[i] >> 4];\
		*_ds++ = hex_table[in[i] & 0xf];\
	} \
	*_ds = '\0'; __d;\
})

#define stk_hex_to_mem(src, len) \
({\
	char *__d = alloca((len * 2) + 1 );\
	typeof(len) b = bytes; \
	typeof(__d) dest = __d; \
	while (bytes--) { \
		*dest++ = (hex_parse(src[0]) << 4) | hex_parse(src[1]);\
		src += 2; } __d[(len * 2) +1] = 0; __d; \
})

#define stk_inet_ntop(af, addr) \
({\
 	size_t __len = af == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;\
	char *__str = alloca(__len);\
 	const char *__dst;\
 	__dst = inet_ntop(af, addr, __str, __len);\
 	__dst;\
})

static unsigned int
hex_make(unsigned int x)
{
	return (x < 10) ? (x + '0') : (x - 10 + 'a');
}

#define stk_hex_enc(addr, bytes) \
({\
 	byte *src = (byte *)addr; \
 	int __bytes = (bytes); \
	byte *__d, *dest = alloca(((__bytes) * 2) + 1); __d = dest;\
	while (__bytes--) { \
		dest[0] = hex_make(*src >> 4); \
		dest[1] = hex_make(*src & 0x0f); \
		dest += 2; src++; \
	} \
	*dest = 0; \
	__d; \
})

static unsigned int 
hex_parse(unsigned int c)
{
	c = toupper(c);
	c -= '0';
	return (c < 10) ? c : (c - 7);
}

#define stk_hex_dec(addr, bytes) \
({\
 	byte *src = addr; \
 	int __bytes = (bytes); \
 	byte *__d, *dest = alloca((__bytes) / 2); __d = dest;\
 	memset(dest, 0, (__bytes) / 2); \
	while (__bytes--) { \
		*dest++ = (hex_parse(src[0]) << 4) | hex_parse(src[1]); \
 		src += 2; \
	} \
	__d; \
})

__END_DECLS

#endif/*__SYS_MEM_STACK_H__*/
