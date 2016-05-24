#ifndef __ALLOCA_H__
#define __ALLOCA_H__

#if defined __GNUC__
#ifndef alloca
# define alloca __builtin_alloca
#endif
#else
# include <alloca.h>
#endif

#endif
