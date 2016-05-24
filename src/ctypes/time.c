/*
 * (c) 1997-2013 Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <ctypes/lib.h>

timestamp_t
get_timestamp(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (timestamp_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

unsigned int
get_timer(timestamp_t *timer)
{ 
	timestamp_t t = *timer;
	*timer = get_timestamp();
	return _min(*timer - t, ~0U);
}
