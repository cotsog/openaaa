#include <stdlib.h>
#include <stdio.h>
#include <compat/portable.h>
#include "uri-scheme-opt.h"

static struct scheme_args_info args;
static struct scheme_params params;

int
main(int argc, char *argv[])
{
	scheme_init(&args);
	scheme_params_init(&params);

	if (argc <= 1)
		scheme_print_help();

	if (scheme(argc, argv, &args) != 0)
		exit(1);

	if (args.list_given)
		sys_uri_scheme_dump(args.name_arg);

	if (args.register_given)
		sys_uri_scheme_register(args.name_arg, args.handler_arg, args.about_arg);

	if (args.unregister_given)
		sys_uri_scheme_unregister(args.name_arg);

	return EXIT_SUCCESS;
}
