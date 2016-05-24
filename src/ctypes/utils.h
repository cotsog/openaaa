#ifndef __MEM_UTILS_H__
#define __MEM_UTILS_H__

#include <stdlib.h>
#include <stdarg.h>
#include <ctypes/lib.h>
#include <unistd.h>                                                             
#include <ctype.h>                                                              
//#include <core/string/chartype.h>                                                
#include <mem/map.h>
#include <mem/page.h>
#include <mem/pool.h>
#include <mem/alloca.h>

__BEGIN_DECLS
/*
void mem_to_hex(char *dest, const char *src, unsigned int bytes);

const char *
hex_to_mem(byte *dest, const char *src, unsigned int bytes, unsigned int flags);
*/
char *
str_unesc(char *d, const char *s);

char *
url_encode(char *str);

char *
url_decode(char *str);

char *
mp_url_encode(struct mempool *mp, char *str);

char *
mp_url_decode(struct mempool *mp, char *str);

__END_DECLS

#endif/*__M_MAP_H__*/
