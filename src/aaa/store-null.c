#include <stdlib.h>
#include <errno.h>

struct object;
struct mempool;

struct store *
store_new(struct mempool *p, struct object *object)
{
        return NULL;
}

void
store_free(struct store *store)
{
        
}

int
store_bind(struct store *st, const char *id)
{
        return -EINVAL;
}

int
store_set(struct store *st, const char *attr, char *value)
{
        return -EINVAL;
}

const char *
store_get(struct store *st, const char *attr)
{
        return NULL;
}

int
store_select(struct store *st, const char *path)
{
        return -EINVAL;
}

int
store_commit(struct store *st)
{
        return -EINVAL;
}
