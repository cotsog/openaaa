void
#ifdef __STDC__
setproctitle(const char *fmt, ...)
#else /* !__STDC__ */
setproctitle(fmt, va_alist)
	const char *fmt;
va_dcl
#endif /* __STDC__ */
{
#if defined(SETPROCTITLE) && !defined(SYSV)
	va_list	args;
	register char *p;
	register int i;
#if (defined(hpux) || defined(__hpux) || defined(__hpux__)) && defined(PSTAT_SETCMD)
	union pstun un;
#else
	extern char **Argv;
	extern char *LastArgv;
#endif
	char buf[MAXLINE];

# ifdef __STDC__
	va_start(args, fmt);
# else /* !__STDC__ */
	va_start(args);
# endif /* __STDC__ */
	(void) vsprintf(buf, fmt, args);
	va_end(args);

#if (defined(hpux) || defined(__hpux) || defined(__hpux__)) && defined(PSTAT_SETCMD)
	(void) sprintf(buf + strlen(buf), " (sendmail)");
	un.pst_command = buf;
	pstat(PSTAT_SETCMD, un, strlen(buf), 0, 0);
#else
	/* make ps print "(sendmail)" */
	p = Argv[0];
	*p++ = '-';

	i = strlen(buf);
	if (i > LastArgv - p - 2)
	{
		i = LastArgv - p - 2;
		buf[i] = '\0';
	}
	(void) strcpy(p, buf);
	p += i;
	while (p < LastArgv)
		*p++ = ' ';
#endif
#endif /* SETPROCTITLE && !SYSV */
}
