DIRS+=src/core/mpm
LIBSYS_OBJECTS+=$(addprefix $(o)/src/core/mpm/, cpu task thread sched)

ifndef CONFIG_WINDOWS
LIBSYS_OBJECTS+=$(addprefix $(o)/src/core/mpm/, proc signal )
endif
