
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <ctypes/lib.h>

sys_log_custom_t sys_log_custom_cb = NULL;
static void *sys_log_custom_data = NULL;
static const char *__log_name = "init";

void
log_init(void)
{
}

void
log_fini(void)
{
}

void
log_name(const char *name)
{
}

void
log_file(const char *file)
{
}

void
sys_log_custom(sys_log_custom_t fn, void *data)
{
	sys_log_custom_cb = fn;
	sys_log_custom_data = data;
}

static inline void 
rm_nonpritable(char *str)
{
	return;
	for(char *p = str; *p; p++) 
		if (*p >= 0 && *p < 0x20 && *p != '\t')
			*p = 0x7f;
}

static inline void
rm_bslashr_char(const char *str)
{
	for(char *p = (char *)str; *p; p++)
		if (*p == '\r')
			*p = ' ';
}

void
log_writev(unsigned int flags, const char *fmt, va_list args)
{
	struct timeval tv;
	struct tm tm;
	va_list args2;
	char stime[24];
	char sutime[12];
	char msgbuf[4096];
	char line[4096];

	/* CAVEAT: These calls are not safe in signal handlers. */
/*
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);

	strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S", &tm);
	snprintf(sutime, sizeof(sutime), ".%06d", (int)tv.tv_usec);
*/
	va_copy(args2, args);
	int len = vsnprintf(msgbuf, sizeof(msgbuf), fmt, args2);
	va_end(args2);

	/* remove non-printable characters and newlines */
	rm_nonpritable(msgbuf);
	rm_bslashr_char(msgbuf);
#if 0
	len = snprintf(line, sizeof(line) - 1, "[I] %s%s [%s] %s\n", 
                       stime, sutime, __log_name, msgbuf);
#else
	len = snprintf(line, sizeof(line) - 1, "%s", msgbuf);  
#endif
	if (!sys_log_custom_cb) {
		write(0, line, len);
		return;
	}

	sys_log_custom_cb(sys_log_custom_data, 0, line);
}

void
log_write(unsigned int flags, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log_writev(flags, fmt, args);
	va_end(args);
}
