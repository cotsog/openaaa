#ifndef __WINDOWS_API_H__
#define __WINDOWS_API_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include <sys/mman.h>

#define strsignal

#define WEXITSTATUS(s) (s)

#if !defined(__CYGWIN__)
# define SIGHUP -1
# define SIGPIPE -1
# define SIGRTMIN -1

typedef struct {
	int unused;
} siginfo_t;

#endif

struct tm *
localtime_r (const time_t *timer, struct tm *result);

void bzero(void *B, size_t LENGTH);

#if defined(WIN32) || defined(WIN64) 
  #define snprintf _snprintf 
  #define vsnprintf _vsnprintf 
#ifndef strcasecmp
  #define strcasecmp stricmp 
#endif
#endif

#endif
