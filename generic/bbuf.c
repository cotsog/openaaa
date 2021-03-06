/*
 *	UCW Library -- A simple growing buffers for byte-sized items
 *
 *	(c) 2006 Pavel Charvat <pchar@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include <ctypes/lib.h>
#include <ctypes/bbuf.h>
#include <stdio.h>

char *
bb_vprintf_at(bb_t *bb, uint ofs, const char *fmt, va_list args)
{
  bb_grow(bb, ofs + 1);
  va_list args2;
  va_copy(args2, args);
  int cnt = vsnprintf((char *)(bb->ptr + ofs), bb->len - ofs, fmt, args2);
  va_end(args2);
  if (cnt < 0)
    {
      /* Our C library doesn't support C99 return value of vsnprintf, so we need to iterate */
      do
        {
	  bb_do_grow(bb, bb->len + 1);
          va_copy(args2, args);
          cnt = vsnprintf((char *)(bb->ptr + ofs), bb->len - ofs, fmt, args2);
          va_end(args2);
	}
      while (cnt < 0);
    }
  else if ((uint)cnt >= bb->len - ofs)
    {
      bb_do_grow(bb, ofs + cnt + 1);
      va_copy(args2, args);
      int cnt2 = vsnprintf((char*)(bb->ptr + ofs), bb->len - ofs, fmt, args2);
      va_end(args2);
      assert(cnt2 == cnt);
    }
  return (char *)(bb->ptr + ofs);
}

char *
bb_printf_at(bb_t *bb, uint ofs, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char *res = bb_vprintf_at(bb, ofs, fmt, args);
  va_end(args);
  return res;
}

char *
bb_vprintf(bb_t *bb, const char *fmt, va_list args)
{
  return bb_vprintf_at(bb, 0, fmt, args);
}

char *
bb_printf(bb_t *bb, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char *res = bb_vprintf_at(bb, 0, fmt, args);
  va_end(args);
  return res;
}

#ifdef TEST

int main(void)
{
  bb_t bb;
  bb_init(&bb);
  char *x = bb_printf(&bb, "<Hello, %s!>", "World");
  fputs(x, stdout);
  x = bb_printf_at(&bb, 5, "<Hello, %50s!>\n", "World");
  fputs(x, stdout);
  bb_done(&bb);
  return 0;
}

#endif
