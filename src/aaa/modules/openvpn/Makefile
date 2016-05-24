DIRS+=src/aaa/modules/openvpn

LIBAAAVPN_API_VERSION=1.0.0
LIBAAAVPN=$(o)/src/aaa/modules/openvpn/libaaa_vpn.pc
LIBAAAVPN_INCLUDES=lib.h
LIBAAAVPN_OBJECTS=$(addprefix $(o)/src/aaa/modules/openvpn/, plugin)

ifdef CONFIG_SHARED
$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: SO_RUNDIR=lib
$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: SONAME_SUFFIX=.$(LIBAAAVPN_API_VERSION)
ifndef CONFIG_WINDOWS
$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: $(addsuffix .$(OO),$(LIBAAAVPN_OBJECTS)) $(LIBAAA) $(LIBAAATLS)
ifndef CONFIG_DARWIN
#$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: LDFLAGS+=-L$(o)/src/aaa -L$(o)/src/aaa/net/tls
#$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: LDFLAGS+=-laaa -Wl,-rpath=$(PREFIX)/lib
#$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: LDFLAGS+=-laaa_tls -Wl,-rpath=$(PREFIX)/lib	
#$(o)/src/aaa/modules/openvpn/libaaa_vpn.so: LSHARED+=-Wl,--version-script=$(s)/src/aaa/modules/openvpn/libaaa_vpn.ver
endif
endif
ifdef CONFIG_WINDOWS
PROGS+=$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll run/lib/libaaa_vpn.dll
$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll: $(o)/src/aaa/libaaa.dll $(o)/src/aaa/net/tls/libaaa_tls.dll
$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll: $(addsuffix .o,$(LIBAAAVPN_OBJECTS)) \
$(o)/src/libaaa_dll.a $(o)/src/aaa/net/tls/libaaa_tls_dll.a 
$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll: LSHARED+=-Wl,-Bstatic -lintl -Wl,-Bdynamic
$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll: SO_RUNDIR=lib
$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll: SONAME_SUFFIX=.$(LIBAAAVPN_API_VERSION)
$(o)/src/aaa/modules/openvpn/libaaa_vpn.dll: LSHARED+=-Wl,--out-implib,$(o)/src/aaa/modules/openvpn/libaaa_vpn_dll.a

run/src/libaaa_vpn.dll: $(o)/src/aaa/libaaa.dll $(o)/src/aaa/net/tls/libaaa_tls.dll $(o)/src/aaa/modules/openvpn/libaaa_vpn.dll
	$(Q)$(shell cp $(o)/src/aaa/modules/openvpn/libaaa_vpn.dll run/lib/)
endif
endif

ifdef CONFIG_DARWIN
/opt/local/lib/libaaa_vpn.dylib: $(addsuffix .$(OO),$(LIBAAAVPN_OBJECTS)) $(LIBAAA) $(LIBAAATLS)
/opt/local/lib/libaaa_vpn.dylib: LIBS+=$(o)/src/aaa/libaaa.a $(o)/src/aaa/net/tls/libaaa_tls.a
/opt/local/lib/libaaa_vpn.dylib: LDFLAGS+=-undefined dynamic_lookup
#$(o)/src/aaa/modules/openvpn/libaaa_vpn.dylib: $(o)/src/aaa/modules/openvpn/libaaa_vpn.so
#	@cp $(o)/src/aaa/modules/openvpn/libaaa_vpn.so $(o)/src/aaa/modules/openvpn/libaaa_vpn.dylib
#run/lib/libaaa_vpn.dylib: $(o)/src/aaa/modules/openvpn/libaaa_vpn.dylib
#	$(call symlink,$(o)/src/aaa/modules/openvpn/libaaa_vpn.dylib,run/lib)
PROGS+=/opt/local/lib/libaaa_vpn.dylib

endif

API_LIBS+=libaaa_vpn
run/lib/pkgconfig/libaaa_vpn.pc: $(o)/src/aaa/modules/openvpn/libaaa_vpn.pc

API_INCLUDES+=$(o)/src/aaa/modules/openvpn/.include-stamp
$(o)/src/aaa/modules/openvpn/.include-stamp: $(addprefix $(s)/src/aaa/modules/openvpn/,$(LIBAAAVPN_INCLUDES))
$(o)/src/aaa/modules/openvpn/.include-stamp: IDST=aaa/vpn

INSTALL_TARGETS+=install-vpn_aaa
install-vpn_aaa:
	install -m 755 $(o)/src/aaa/modules/openvpn/libaaa_vpn.so $(PREFIX)/lib/libaaa_vpn.so
.PHONY: install-vpn_aaa
