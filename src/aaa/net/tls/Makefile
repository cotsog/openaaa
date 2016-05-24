DIRS+=src/aaa/net/tls

LIBAAATLS_API_VERSION=1.0.0
LIBAAATLS=$(o)/src/aaa/net/tls/libaaa_tls.pc
LIBAAATLS_STATIC=$(o)/src/aaa/net/tls/libaaa_tls.a
LIBAAATLS_PICSTATIC=$(o)/src/aaa/net/tls/libaaa_tls-pic.a
LIBAAATLS_INCLUDES=lib.h
LIBAAATLS_OBJECTS=$(addprefix $(o)/src/aaa/net/tls/, init)

include $(s)/src/aaa/net/tls/openssl/Makefile
include $(s)/src/aaa/net/tls/nss/Makefile

$(LIBAAATLS_STATIC): $(addsuffix .o, $(LIBAAATLS_OBJECTS) $(LIBSYS_OBJECTS))          
$(LIBAAATLS_PICSTATIC): $(addsuffix .$(OO), $(LIBAAATLS_OBJECTS) $(LIBSYS_OBJECTS))
ifdef CONFIG_SHARED
$(o)/src/aaa/net/tls/libaaa_tls.so: $(addsuffix .$(OO),$(LIBAAATLS_OBJECTS)) $(LIBAAA) $(LIBAAATLS_PICSTATIC)
$(o)/src/aaa/net/tls/libaaa_tls.so: SO_RUNDIR=lib
$(o)/src/aaa/net/tls/libaaa_tls.so: SONAME_SUFFIX=.$(LIBAAATLS_API_VERSION)
ifndef CONFIG_WINDOWS
#$(o)/src/aaa/net/tls/libaaa_tls.so: $(o)/src/aaa/libaaa.so
ifndef CONFIG_DARWIN
$(o)/src/aaa/net/tls/libaaa_tls.so: LDFLAGS+=-L$(o)/src/aaa -laaa -Wl,-rpath=$(PREFIX)/lib
$(o)/src/aaa/net/tls/libaaa_tls.so: LSHARED+=-Wl,--version-script=$(s)/src/aaa/net/tls/libaaa_tls.ver
endif
endif
ifdef CONFIG_WINDOWS
PROGS+=$(o)/src/aaa/net/tls/libaaa_tls.dll run/lib/libaaa_tls.dll
$(o)/src/aaa/net/tls/libaaa_tls.dll: $(o)/src/aaa/libaaa.dll
$(o)/src/aaa/net/tls/libaaa_tls.dll: $(addsuffix .$(OO),$(LIBAAATLS_OBJECTS)) $(o)/src/libaaa_dll.a
$(o)/src/aaa/net/tls/libaaa_tls.dll: LSHARED+=-Wl,--out-implib,$(o)/src/aaa/net/tls/libaaa_tls_dll.a

run/lib/libaaa_tls.dll: $(o)/src/aaa/libaaa.dll $(o)/src/aaa/net/tls/libaaa_tls.dll
	$(Q)$(shell cp $(o)/src/aaa/net/tls/libaaa_tls.dll run/lib/libaaa_tls.dll)
endif

endif

ifdef CONFIG_DARWIN
$(o)/src/aaa/net/tls/libaaa_tls.dylib: $(o)/src/aaa/net/tls/libaaa_tls.so
	@cp $(o)/src/aaa/net/tls/libaaa_tls.so $(o)/src/aaa/net/tls/libaaa_tls.dylib
run/lib/libaaa_tls.dylib: $(o)/src/aaa/net/tls/libaaa_tls.dylib
	$(call symlink,$(o)/src/aaa/net/tls/libaaa_tls.dylib,run/lib)
PROGS+=run/lib/libaaa_tls.dylib
endif

API_LIBS+=libaaa_tls
run/lib/pkgconfig/libaaa_tls.pc: $(o)/src/aaa/net/tls/libaaa_tls.pc

API_INCLUDES+=$(o)/src/aaa/net/tls/.include-stamp
$(o)/src/aaa/net/tls/.include-stamp: $(addprefix $(s)/src/aaa/net/tls/,$(LIBAAATLS_INCLUDES))
$(o)/src/aaa/net/tls/.include-stamp: IDST=aaa/tls

INSTALL_TARGETS+=install-tls_aaa
install-tls_aaa:
ifdef CONFIG_WINDOWS
	cp $(o)/src/aaa/net/tls/libaaa_tls.dll /cygdrive/c/aaa/
endif	
	install -m 755 $(o)/src/aaa/net/tls/libaaa_tls.so $(PREFIX)/lib/libaaa_tls.so
.PHONY: install-tls_aaa
