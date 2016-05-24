#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int
random_range(unsigned const low, unsigned const high)
{
	unsigned const range = high - low + 1;
	return low + (int) (((double) range) * rand () / (RAND_MAX + 1.0));
}

void
gen_random(char *s, const int len)
{
	static const char alphanum[] = "0123456789"
	                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	                               "abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}
