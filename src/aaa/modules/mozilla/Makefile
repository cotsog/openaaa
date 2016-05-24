DIRS+=src/aaa/modules/mozilla

LIBAAAMOZ_API_VERSION=1.0.0
LIBAAAMOZ=$(o)/src/aaa/modules/mozilla/libaaa_moz.pc
LIBAAAMOZ_INCLUDES=lib.h
LIBAAAMOZ_OBJECTS+=$(addprefix $(o)/src/aaa/modules/mozilla/, plugin res)

ifdef CONFIG_SHARED
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: SO_RUNDIR=lib
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: SONAME_SUFFIX=.$(LIBAAAMOZ_API_VERSION)
ifndef CONFIG_WINDOWS

WINDRES=windres
RCAAAMOZZILA=$(s)/src/aaa/modules/mozilla/resource.rc
$(o)/src/aaa/modules/mozilla/res.o: $(RCAAAMOZZILA)
	$(Q)$(WINDRES) -i $(RCAAAMOZZILA) -o $(o)/src/aaa/modules/mozilla/res.o

$(o)/src/aaa/modules/mozilla/libaaa_moz.so: $(addsuffix .$(OO),$(LIBAAAMOZ_OBJECTS)) $(LIBAAA) $(LIBAAATLS)
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: SO_RUNDIR=lib
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: SONAME_SUFFIX=.$(LIBAAAMOZ_API_VERSION)
ifndef CONFIG_DARWIN
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: LDFLAGS+=-L$(o)/src/aaa -L$(o)/src/net/tls
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: LDFLAGS+=-laaa -Wl,-rpath=$(PREFIX)/lib
$(o)/src/aaa/modules/mozilla/libaaa_moz.so: LDFLAGS+=-laaa_tls -Wl,-rpath=$(PREFIX)/lib	
#$(o)/src/aaa/modules/mozilla/libaaa_moz.so: LSHARED+=-Wl,--version-script=$(s)/src/aaa/modules/mozilla/libaaa_moz.ver
endif
endif

ifdef CONFIG_WINDOWS

WINDRES=windres
RCAAAMOZZILA=$(s)/src/aaa/modules/mozilla/resource.rc
$(o)/src/aaa/modules/mozilla/res.o: $(RCAAAMOZZILA)
	@echo "RES $(RCAAAMOZZILA)"
	$(Q)$(WINDRES) -i $(RCAAAMOZZILA) -o $(o)/src/aaa/modules/mozilla/res.o

PROGS+=$(o)/src/aaa/modules/mozilla/npaaamoz.dll run/lib/npaaamoz.dll $(o)/src/aaa/modules/mozilla/res.o
$(o)/src/aaa/modules/mozilla/npaaamoz.dll: $(o)/src/aaa/libaaa.dll $(o)/src/net/tls/libaaa_tls.dll
$(o)/src/aaa/modules/mozilla/npaaamoz.dll: $(addsuffix .o,$(LIBAAAMOZ_OBJECTS)) \
$(o)/src/libaaa_dll.a $(o)/src/net/tls/libaaa_tls_dll.a $(SYSROOT)/lib/libpthread.a
$(o)/src/aaa/modules/mozilla/npaaamoz.dll: SO_RUNDIR=lib
$(o)/src/aaa/modules/mozilla/npaaamoz.dll: SONAME_SUFFIX=.$(LIBAAAMOZ_API_VERSION)
$(o)/src/aaa/modules/mozilla/npaaamoz.dll: LSHARED+=-Wl,--subsystem,windows,--out-implib,$(o)/src/aaa/modules/mozilla/npaaamoz.dll.a


#$(Q)$(shell cp $(o)/src/aaa/modules/mozilla/npaaamoz.dll run/lib/)
endif
endif

ifdef CONFIG_DARWIN
$(o)/src/aaa/modules/mozilla/libaaa_moz.dylib: $(o)/src/aaa/modules/mozilla/libaaa_moz.so
	@cp $(o)/src/aaa/modules/mozilla/libaaa_moz.so $(o)/src/aaa/modules/mozilla/libaaa_moz.dylib
run/lib/libaaa_moz.dylib: $(o)/src/aaa/modules/mozilla/libaaa_moz.dylib
	$(call symlink,$(o)/src/aaa/modules/mozilla/libaaa_moz.dylib,run/lib)
PROGS+=run/lib/libaaa_moz.dylib
endif

API_LIBS+=libaaa_moz
run/lib/pkgconfig/libaaa_moz.pc: $(o)/src/aaa/modules/mozilla/libaaa_moz.pc

API_INCLUDES+=$(o)/src/aaa/modules/mozilla/.include-stamp
$(o)/src/aaa/modules/mozilla/.include-stamp: $(addprefix $(s)/src/aaa/modules/mozilla/,$(LIBAAAMOZ_INCLUDES))
$(o)/src/aaa/modules/mozilla/.include-stamp: IDST=aaa/vpn

INSTALL_TARGETS+=install-moz_aaa
install-moz_aaa:
ifdef CONFIG_WINDOWS
	cp ./build/arch/win-x86/asm32.dll /cygdrive/c/aaa/
	cp ./build/arch/win-x86/qrview.exe /cygdrive/c/aaa/
#	cp $(o)/src/aaa/libaaa.dll /cygdrive/c/aaa/
#	cp $(o)/src/net/tls/libaaa_tls.dll /cygdrive/c/aaa/
#	cp $(o)/src/aaa/modules/mozilla/npaaamoz.dll /cygdrive/c/aaa/
#	install -m 755 $(o)/src/aaa/modules/mozilla/npaaamoz.dll "`cygpath -u "$(APPDATA)"`/Mozilla/Plugins/"
endif	
	install -m 755 $(o)/src/aaa/modules/mozilla/libaaa_moz.so $(PREFIX)/lib/libaaa_moz.so
.PHONY: install-moz_aaa
