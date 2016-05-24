#ifndef __DARWIN_MREMAP_H__
#define __DARWIN_MREMAP_H__

void *
mremap(void *addr, size_t size, size_t nsize, int mode);

#endif
