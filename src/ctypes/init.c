#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#ifdef CONFIG_HPUX
# include <sys/mpctl.h>
#endif

#include <time.h>
#include <ctypes/lib.h>

unsigned int __cpu_count;
unsigned int __cpu_page_size;
unsigned int __verbose;

static void __attribute__((constructor)) __init(void)
{
}

static void __attribute__((destructor)) __fini(void)
{

}

void
sys_init(void)
{
}

void
sys_fini(void)
{
}
