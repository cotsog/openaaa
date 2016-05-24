DIRS+=src/compat

ifeq ($(CONFIG_LINUX),1)
include $(s)/src/compat/linux/Makefile
endif
ifeq ($(CONFIG_DARWIN),1)
include $(s)/src/compat/darwin/Makefile
endif
ifeq ($(CONFIG_HPUX),1)
include $(s)/src/compat/hpux/Makefile
endif
ifeq ($(CONFIG_WINDOWS),1)
include $(s)/src/compat/windows/Makefile
endif
