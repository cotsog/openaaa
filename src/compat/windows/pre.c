#include <marvin/kernel.h>
#include <marvin/mem/alloca.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <compat/portable.h>

struct object *modules;
struct module {
	struct object object;
	HMODULE handle;
	unsigned long pid;
	char *file;
	char *fullpath;
	
};

unsigned long
find_window_pid(char *class, char *window)
{
	unsigned long pid;
	HWND hwnd = FindWindow(class, window);
	GetWindowThreadProcessId(hwnd, &pid);
    
	return pid;
}

unsigned long
find_proc_pid(const char *name)
{
	HANDLE th = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(th == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 pe = { .dwSize = sizeof(PROCESSENTRY32) };

	unsigned long pid = 0;
	for (Process32First(th, &pe); Process32Next(th, &pe); ) {
		if (!strncasecmp(pe.szExeFile, name, strlen(name)))
			pid = pe.th32ProcessID;
	}

exit:
	CloseHandle(th);
	return pid;

}

unsigned long
find_proc_tid(unsigned long pid)
{
	unsigned long pTID = 0, tid = 0;

#if CPU_ARCH==1
	__asm__ (
		"mov %%eax, fs:[0x18]"
		"add %%eax, 36"
		"mov %0, %%eax;"
		: /* no output */
		:"r"(pTID)
		:"%eax"
	);
#endif

	HANDLE handle = OpenProcess(PROCESS_VM_READ, 0, pid);
	ReadProcessMemory(handle, (void *)pTID, &tid, 4, NULL);
	CloseHandle(handle);

	return tid;
}

void
modules_init(unsigned long pid)
{
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (handle == NULL)
		return;

	DWORD need = 0;
	if (!EnumProcessModules(handle, NULL, 0, &need))
		return;

	HMODULE *mods = alloca(need * sizeof(HMODULE));
	if (!EnumProcessModules(handle, mods, need, &need))
		return;

	for (unsigned int i = 0; i < (need / sizeof(HMODULE)); i++) {
		struct module *module = mp_alloc_zero(modules->mp, sizeof(*module));
		module->fullpath = mp_alloc(modules->mp, MAX_PATH);
		module->handle   = mods[i];

		GetModuleFileNameEx(handle, mods[i], module->fullpath, MAX_PATH - 1);
		list_add_tail(&modules->childs, &module->object.node);
	}

	CloseHandle(handle);
}

void
modules_fini(void)
{
}

void
preload_init(void)
{
	modules = object_alloc_zero(sizeof(*modules));
	modules_init(getpid());
}

void
preload_fini(void)
{
	object_free(modules);
}

unsigned long
find_module(unsigned long pid, const char *name)
{
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (handle == NULL)
		return 0;

	DWORD need = 0;
	if (!EnumProcessModules(handle, NULL, 0, &need))
		return 0;

	HMODULE *mods = alloca(need * sizeof(HMODULE));
	if (!EnumProcessModules(handle, mods, need, &need))
		return 0;

	for (unsigned int i = 0; i < (need / sizeof(HMODULE)); i++) {
		char fullpath[MAX_PATH];
		if (!GetModuleFileNameEx(handle, mods[i], fullpath, MAX_PATH - 1))
			continue;

		if (!strstr(fullpath, name))
			continue;

		printf("module: %s\n", fullpath);
		goto exit;
        }

exit:
	CloseHandle(handle);
	return 0;
}

/*
int
inject_dll(unsigned long pid, const char *dll)
{
   char buf[50]={0};

   HANDLE handle = OpenProcess(CREATE_THREAD_ACCESS, FALSE, pid);

   void *lib  = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
   void *data = (LPVOID)VirtualAllocEx(handle, NULL, strlen(dll), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
   WriteProcessMemory(handle, (LPVOID)data, dll,strlen(dll), NULL);
   CreateRemoteThread(handle, NULL, NULL, (LPTHREAD_START_ROUTINE)lib, (LPVOID)RemoteString, NULL, NULL);   
   CloseHandle(handle);

   return 0;
}

*/

