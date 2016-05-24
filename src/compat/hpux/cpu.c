#include <stdlib.h>
#include <errno.h>
#include <sys/pset.h>

int
cpu_set_affinity(int id, int type, int cpu)
{
	psetid_t set;

	if (pset_create(&set) < 0)
		return -EINVAL;
	if (pset_assign(set, (spu_t)cpu, NULL) < 0)
		return -EINVAL;
	if (pset_bind(set, P_PID, id, NULL) < 0)
        	return -EINVAL;

	return 0;
}
