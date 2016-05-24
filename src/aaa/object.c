
#include <sys/lib.h>
#include <sys/mem/map.h>
#include <sys/mem/page.h>
#include <sys/mem/stack.h>
#include <aaa/lib.h>

struct aaa_object {
	struct page page;
	timestamp_t created;
	timestamp_t modified;
	timestamp_t expires;
	byte obj[];
};

