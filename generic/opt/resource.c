/*
 *	The UCW Library -- Resource Pools
 *
 *	(c) 2008--2011 Martin Mares <mj@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include <sys/compiler.h>
#include <sys/log.h>
#include <mem/list.h>
#include <mem/pool.h>
#include <opt/resource.h>
#include <stdio.h>

struct respool *
rp_new(const char *name, struct mempool *mp)
{
  struct respool *rp;

  if (mp)
    {
      rp = mp_alloc_zero(mp, sizeof(*rp));
      rp->mpool = mp;
    }
  else
    rp = xmalloc_zero(sizeof(*rp));
  list_init(&rp->resources);
  rp->name = name;
  rp->default_res_flags = RES_FLAG_TEMP;
  return rp;
}

static void
rp_free(struct respool *rp)
{
  if (rp->subpool_of)
    res_detach(rp->subpool_of);
  if (!rp->mpool)
    xfree(rp);
  if (rp_current() == rp)
    rp_switch(NULL);
}

void
rp_delete(struct respool *rp)
{
  struct resource *r;
  while ((r = list_tail(&rp->resources)))
    {
      assert(r->rpool == rp);
      res_free(r);
    }
  rp_free(rp);
}

void
rp_detach(struct respool *rp)
{
  struct resource *r;
  while ((r = list_head(&rp->resources)))
    {
      assert(r->rpool == rp);
      res_detach(r);
    }
  rp_free(rp);
}

void
rp_commit(struct respool *rp)
{
  struct resource *r;
  while ((r = list_head(&rp->resources)))
    {
      assert(r->rpool == rp);
      if (r->flags & RES_FLAG_TEMP)
	res_free(r);
      else
	res_detach(r);
    }
  rp_free(rp);
}

void
rp_dump(struct respool *rp, uint indent)
{
  printf("%*sResource pool %s at %p (%s)%s:\n",
	 indent, "",
	 (rp->name ? : "(noname)"),
	 rp,
	 (rp->mpool ? "mempool-based" : "freestanding"),
	 (rp->subpool_of ? " (subpool)" : "")
	 );
  list_for_each(struct resource *, r, rp->resources)
    res_dump(r, indent+4);
}

struct resource *
res_alloc(const struct res_class *rc)
{
  struct respool *rp = rp_current();
  assert(rp);
  uint size = (rc->res_size ? : sizeof(struct resource));
  struct resource *r;
  if (rp->mpool)
    {
      r = mp_alloc_fast(rp->mpool, size);
      r->flags = 0;
    }
  else
    {
      r = xmalloc(size);
      r->flags = RES_FLAG_XFREE;
    }
  res_add(r);
  return r;
}

void
res_add(struct resource *r)
{
  struct respool *rp = rp_current();
  assert(rp);
  r->rpool = rp;
  list_add_tail(&rp->resources, &r->n);
  r->flags &= ~RES_FLAG_TEMP;
  r->flags |= rp->default_res_flags & RES_FLAG_TEMP;
}

void
res_drop(struct resource *r)
{
  list_remove(&r->n);
  if (r->flags & RES_FLAG_XFREE)
    xfree(r);
}

void
res_detach(struct resource *r)
{
  if (!r)
    return;
  if (r->rclass->detach)
    r->rclass->detach(r);
  res_drop(r);
}

void
res_free(struct resource *r)
{
  if (!r)
    return;
  if (r->rclass->free)
    r->rclass->free(r);
  res_drop(r);
}

void
res_dump(struct resource *r, uint indent)
{
	printf("%*s%p %s %s", indent, "", r, ((r->flags & RES_FLAG_TEMP) ? "TEMP":
	       "PERM"), r->rclass->name);
	if (r->rclass->dump)
		r->rclass->dump(r, indent+4);
	else
		putchar('\n');
}
