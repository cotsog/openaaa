#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
/*
/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -dump | grep -B6 bindings:.*:
*/

const char *sys_get_progname(void)
{
	return getprogname();
}

unsigned int
strnlen(const char *str, unsigned int n)
{
        const char *end = str + n, *c;
        for (c = str; *c && c < end; c++);
        return c - str;
}

void
sys_uri_scheme_dump(const char *name)
{
//	system("/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -dump | grep -B6 bindings:.*:");

}

int
sys_uri_scheme_register(const char *id, const char *handler, const char *about)
{
	return -EINVAL;
}

int
sys_uri_scheme_unregister(const char *id)
{
	return -EINVAL;
}
