/*
 *	UCW Library -- Fast Buffered I/O on Memory Streams
 *
 *	(c) 1997--2002 Martin Mares <mj@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include <sys/compiler.h>
#include <stdlib.h>
#include <stdarg.h>
#include <opt/fastbuf.h>

#include <stdio.h>

struct memstream {
  uint blocksize;
  uint uc;
  struct msblock *first;
};

struct msblock {
  struct msblock *next;
  off_t pos;
  uint size;
  byte data[0];
};

struct fb_mem {
  struct fastbuf fb;
  struct memstream *stream;
  struct msblock *block;
};
#define FB_MEM(f) ((struct fb_mem *)(f))

static int
fbmem_refill(struct fastbuf *f)
{
  struct memstream *s = FB_MEM(f)->stream;
  struct msblock *b = FB_MEM(f)->block;

  if (!b)
    {
      b = s->first;
      if (!b)
	return 0;
    }
  else if (f->buffer == b->data && f->bstop < b->data + b->size)
    {
      f->bstop = b->data + b->size;
      f->pos = b->pos + b->size;
      return 1;
    }
  else if (!b->next)
    return 0;
  else
    b = b->next;
  if (!b->size)
    return 0;
  f->buffer = f->bptr = b->data;
  f->bufend = f->bstop = b->data + b->size;
  f->pos = b->pos + b->size;
  FB_MEM(f)->block = b;
  return 1;
}

static void
fbmem_spout(struct fastbuf *f)
{
  struct memstream *s = FB_MEM(f)->stream;
  struct msblock *b = FB_MEM(f)->block;
  struct msblock *bb;

  if (b)
    {
      b->size = f->bptr - b->data;
      if (b->size < s->blocksize)
	return;
    }
  bb = xmalloc(sizeof(struct msblock) + s->blocksize);
  if (b)
    {
      b->next = bb;
      bb->pos = b->pos + b->size;
    }
  else
    {
      s->first = bb;
      bb->pos = 0;
    }
  bb->next = NULL;
  bb->size = 0;
  f->buffer = f->bptr = f->bstop = bb->data;
  f->bufend = bb->data + s->blocksize;
  f->pos = bb->pos;
  FB_MEM(f)->block = bb;
}

static int
fbmem_seek(struct fastbuf *f, off_t pos, int whence)
{
  struct memstream *m = FB_MEM(f)->stream;
  struct msblock *b;

  assert(whence == SEEK_SET || whence == SEEK_END);
  if (whence == SEEK_END)
    {
      for (b=m->first; b; b=b->next)
	pos += b->size;
    }
  /* Yes, this is linear. But considering the average number of buckets, it doesn't matter. */
  for (b=m->first; b; b=b->next)
    {
      if (pos <= b->pos + (off_t)b->size) /* <=, because we need to be able to seek just after file end */
	{
	  f->buffer = f->bstop = b->data;
	  f->bptr = b->data + (pos - b->pos);
	  f->bufend = b->data + b->size;
	  f->pos = b->pos;
	  FB_MEM(f)->block = b;
	  return 1;
	}
    }
  if (!m->first && !pos)
    {
      /* Seeking to offset 0 in an empty file needs an exception */
      f->buffer = f->bptr = f->bstop = f->bufend = NULL;
      f->pos = 0;
      FB_MEM(f)->block = NULL;
      return 1;
    }
  bthrow(f, "seek", "fbmem_seek to an invalid offset");
}

static void
fbmem_close(struct fastbuf *f)
{
  struct memstream *m = FB_MEM(f)->stream;
  struct msblock *b;

  if (!--m->uc)
    {
      while ((b = m->first))
	{
	  m->first = b->next;
	  xfree(b);
	}
      xfree(m);
    }
  xfree(f);
}

struct fastbuf *
fbmem_create(uint blocksize)
{
  struct fastbuf *f = xmalloc_zero(sizeof(struct fb_mem));
  struct memstream *s = xmalloc_zero(sizeof(struct memstream));

  s->blocksize = blocksize;
  s->uc = 1;

  FB_MEM(f)->stream = s;
  f->name = "<fbmem-write>";
  f->spout = fbmem_spout;
  f->close = fbmem_close;
  return f;
}

struct fastbuf *
fbmem_clone_read(struct fastbuf *b)
{
  struct fastbuf *f = xmalloc_zero(sizeof(struct fb_mem));
  struct memstream *s = FB_MEM(b)->stream;

  bflush(b);
  s->uc++;

  FB_MEM(f)->stream = s;
  f->name = "<fbmem-read>";
  f->refill = fbmem_refill;
  f->seek = fbmem_seek;
  f->close = fbmem_close;
  f->can_overwrite_buffer = 1;
  return f;
}
