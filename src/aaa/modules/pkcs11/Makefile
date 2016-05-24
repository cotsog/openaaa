DIRS+=src/aaa/modules/pkcs11

LIBAAAPKCS_API_VERSION=1.0.0
LIBAAAPKCS=$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.pc
LIBAAAPKCS_INCLUDES=lib.h
LIBAAAPKCS_OBJECTS+=$(addprefix $(o)/src/aaa/modules/pkcs11/, module)

ifdef CONFIG_SHARED
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: SO_RUNDIR=lib
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: SONAME_SUFFIX=.$(LIBAAAPKCS_API_VERSION)

$(o)/src/aaa/modules/pkcs11/module.o: CFLAGS+=-I./include/nspr
$(o)/src/aaa/modules/pkcs11/module.oo: CFLAGS+=-I./include/nspr

ifndef CONFIG_WINDOWS

#WINDRES=windres
#RCAAAPKCS=$(s)/src/aaa/modules/pkcs11/resource.rc
#$(o)/src/aaa/modules/pkcs11/res.o: $(RCAAAPKCS)
#	$(Q)$(WINDRES) -i $(RCAAAPKCS) -o $(o)/src/aaa/modules/pkcs11/res.o

$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: $(addsuffix .$(OO),$(LIBAAAPKCS_OBJECTS)) $(LIBAAA) $(LIBAAATLS)
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: SO_RUNDIR=lib
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: SONAME_SUFFIX=.$(LIBAAAPKCS_API_VERSION)
ifndef CONFIG_DARWIN
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: LDFLAGS+=-L$(o)/src/aaa -L$(o)/src/aaa/net/tls
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: LDFLAGS+=-laaa -Wl,-rpath=$(PREFIX)/lib
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: LDFLAGS+=-laaa_tls -Wl,-rpath=$(PREFIX)/lib	
#$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so: LSHARED+=-Wl,--version-script=$(s)/src/aaa/modules/pkcs11/libaaa_pkcs11.ver
endif
endif

ifdef CONFIG_WINDOWS

RCAAAPKCS=$(s)/src/aaa/modules/pkcs11/resource.rc
$(o)/src/aaa/modules/pkcs11/res.o: $(RCAAAPKCS)
	@echo "RES $(RCAAAPKCS)"
	$(Q)$(WINDRES) -i $(RCAAAPKCS) -o $(o)/src/aaa/modules/pkcs11/res.o

PROGS+=$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll run/lib/libaaa_pkcs11.dll $(o)/src/aaa/modules/pkcs11/res.o
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll: $(o)/src/aaa/libaaa.dll $(o)/src/aaa/net/tls/libaaa_tls.dll
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll: $(addsuffix .o,$(LIBAAAPKCS_OBJECTS)) \
$(o)/src/libaaa_dll.a $(o)/src/aaa/net/tls/libaaa_tls_dll.a
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll: SO_RUNDIR=lib
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll: SONAME_SUFFIX=.$(LIBAAAPKCS_API_VERSION)
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll: LSHARED+=-Wl,--version-script=$(s)/src/aaa/modules/pkcs11/libaaa_pkcs11.ver

#$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll: LSHARED+=-Wl,--subsystem,windows,--out-implib,$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll.a

run/lib/libaaa_pkcs11.dll: $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll
	$(Q)$(shell cp $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll run/lib/)
endif
endif

ifdef CONFIG_DARWIN
$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dylib: $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so
	@cp $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dylib
run/lib/libaaa_pkcs11.dylib: $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dylib
	$(call symlink,$(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dylib,run/lib)
PROGS+=run/lib/libaaa_pkcs11.dylib
endif

API_LIBS+=libaaa_pkcs11
run/lib/pkgconfig/libaaa_pkcs11.pc: $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.pc

API_INCLUDES+=$(o)/src/aaa/modules/pkcs11/.include-stamp
$(o)/src/aaa/modules/pkcs11/.include-stamp: $(addprefix $(s)/src/aaa/modules/pkcs11/,$(LIBAAAPKCS_INCLUDES))
$(o)/src/aaa/modules/pkcs11/.include-stamp: IDST=aaa/pkcs11

INSTALL_TARGETS+=install-aaa_pkcs
install-aaa_pkcs:
ifdef CONFIG_WINDOWS
	cp $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.dll /cygdrive/c/aaa/
endif	
	install -m 755 $(o)/src/aaa/modules/pkcs11/libaaa_pkcs11.so $(PREFIX)/lib/libaaa_pkcs11.so
.PHONY: install-aaa_pkcs
