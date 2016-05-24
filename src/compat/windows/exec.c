#include <sys/compiler.h>
#include <sys/cpu.h>
#include <mem/pool.h>
#include <windows.h>
#include <process.h>

static struct mempool *mp;
const char *__exec_image_file;

void
exec_init(void)
{
	mp = mp_new(CPU_PAGE_SIZE);
	//__exec_image_file = get_process_file();
}

void
exec_fini(void)
{
	mp_delete(mp);
}

int
open_uri(const char *uri)
{
        ShellExecute(NULL, "open", (char *)uri, NULL, NULL, SW_SHOWNORMAL);
        return 0;
}

const char *
get_process_file(void)
{
	char file[MAX_PATH];
	GetModuleFileName(0, file, sizeof(file) - 1);
	return mp_strdup(mp, file);
}
