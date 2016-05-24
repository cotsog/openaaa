#define PRIVATE

#include <stdlib.h>
#include <stdio.h>
#include <db/lib.h>
#include <db/priv.h>
#include <db/mem/pool.h>

#undef HASH_TABLE_PREFIX
#include <db/hash/fn.h>
#include <db/hash/tab.h>

int
main(int argc, char *argv[])
{
	unsigned int h = hash_string("1234567890");

	static const unsigned int table_shift = 9;
	static const unsigned int table_total = 4U << 20;

	struct table table = {
		.page_shift   = table_shift,
		.page_total   = table_total,
        	.bucket_shift = table_shift,
	        .bucket_total = table_total,
	};

/*
	table_open(&table);
	table_close(&table);
*/
	printf("Hello world!\n");

	return 0;
}
