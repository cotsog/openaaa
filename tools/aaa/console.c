/*
 * $benchmark.c                                    Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <core/lib.h>
#include <core/mem/types.h>

#include <aaa/lib.h>
#include <aaa/private.h>

#include <editline/readline.h>

#include "private.h"

void
console_input(void)
{
	using_history();

	/* setup vi editing mode by default */
//	rl_editing_mode = 0;

	const char *line;
	while ((line = readline("aaa> "))) {
		add_history(line);
		cmd_do(line);
	}
}
