DIRS+=src/compat/windows
LIBSYS_OBJECTS+=$(addprefix $(o)/src/compat/windows/, exec proc mman shell\
	       	dlfcn dlwrap lockf kill file strchrnul strndupa fcntl fsync)
NOHOOK=1
ifndef NOHOOK
LIBSYS_OBJECTS+=$(addprefix $(o)/src/compat/windows/, \
                alloc error string barrier install uninstall thread \
                caller reloc file memory acl dllmain debug)

ifdef __x86_64
LIBSYS_OBJECTS+=$(addprefix $(o)/src/compat/windows/, HookSpecific_x64)
else
LIBSYS_OBJECTS+=$(addprefix $(o)/src/compat/windows/, HookSpecific_x86)
LIBSYS_OBJECTS+=$(addprefix $(o)/src/compat/windows/, ILD_x86)
$(o)/src/compat/windows/HookSpecific_x86.o: \
$(s)/src/compat/windows/HookSpecific_x86.asm
	$(M)ASM $<
	$(Q)ml /Fo $(o)/src/compat/windows/HookSpecific_x86.o /coff /c \
	           $(s)/src/compat/windows/HookSpecific_x86.asm 2>/dev/null
$(o)/src/compat/windows/ILD_x86.o: \
$(s)/src/compat/windows/ILD_x86.asm
	$(M)ASM $<
	$(Q)ml /Fo $(o)/src/compat/windows/ILD_x86.o /coff /c \
                   $(s)/src/compat/windows/ILD_x86.asm 2>/dev/null
endif

endif
