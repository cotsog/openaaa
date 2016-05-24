DIRS+=src/aaa

LIBAAA=$(o)/src/aaa/libaaa.pc
LIBAAA_STATIC=$(o)/src/aaa/libaaa.a
LIBAAA_PICSTATIC=$(o)/src/aaa/libaaa-pic.a

LIBAAA_API_VERSION=1.0.0
LIBAAA_INCLUDES=lib.h
LIBAAA_OBJECTS=$(addprefix $(o)/src/aaa/, api session)

ifeq (${CONFIG_HAVE_LIBMEMCACHED},1)
LIBAAA_OBJECTS+=$(addprefix $(o)/src/aaa/, memcached)
$(o)/src/aaa/libaaa.so: LSHARED+=`pkg-config --libs libmemcached`
LDFLAGS+=-lmemcached
else
LIBAAA_OBJECTS+=$(addprefix $(o)/src/aaa/, store-null)
endif

ifeq (${CONFIG_SWIG},1)
ifdef JAVA_HOME
LIBAAA_OBJECTS+=$(o)/src/swig/java/aaa
endif
endif

$(o)/src/aaa/libaaa.pc: $(o)/src/aaa/libaaa.so $(o)/src/aaa/libaaa.a
ifdef CONFIG_DARWIN
	$(Q)$(shell cp $(s)/src/aaa/libaaa.pc $(o)/src/aaa/libaaa.pc)
endif

$(LIBAAA_STATIC): $(addsuffix .o, $(LIBAAA_OBJECTS) $(LIBSYS_OBJECTS))
$(LIBAAA_PICSTATIC): $(addsuffix .$(OO), $(LIBAAA_OBJECTS) $(LIBSYS_OBJECTS)) 

ifdef CONFIG_SHARED
PROGS+=$(o)/src/aaa/libaaa.so 
$(o)/src/aaa/libaaa.so: $(addsuffix .$(OO), $(LIBAAA_OBJECTS) $(LIBSYS_OBJECTS)) 
ifndef CONFIG_DARWIN
$(o)/src/aaa/libaaa.so: LSHARED+=-Wl,--version-script=$(s)/src/aaa/libaaa.ver
endif
$(o)/src/aaa/libaaa.so: SO_RUNDIR=lib
$(o)/src/aaa/libaaa.so: SONAME_SUFFIX=.$(LIBAAA_API_VERSION)
ifdef CONFIG_WINDOWS
PROGS+=$(o)/src/aaa/libaaa.dll run/lib/libaaa.dll
$(o)/src/aaa/libaaa.dll: $(addsuffix .o, $(LIBAAA_OBJECTS) $(LIBSYS_OBJECTS)) 
$(o)/src/aaa/libaaa.dll: SO_RUNDIR=lib
$(o)/src/aaa/libaaa.dll: LSHARED+=-Wl,-Bstatic -lintl -Wl,-Bdynamic
$(o)/src/aaa/libaaa.dll: LSHARED+=-Wl,--out-implib,$(o)/src/aaa/libaaa_dll.a
$(o)/src/aaa/libaaa.dll: SONAME_SUFFIX=.$(LIBAAA_API_VERSION)
run/lib/libaaa.dll: $(o)/src/aaa/libaaa.dll
	$(Q)$(shell cp $(o)/src/aaa/libaaa.dll run/lib/)
	$(Q)$(shell cp $(o)/build/arch/win-$(CPU_ARCH)/asm$(CPU_BITS_SIZE).dll run/lib/)
endif
endif

API_LIBS+=libaaa
run/lib/pkgconfig/libaaa.pc: $(o)/src/aaa/libaaa.pc
	$(Q)$(shell cp $(s)/src/aaa/libaaa.pc run/lib/pkgconfig/libaaa.pc)

API_INCLUDES+=$(o)/src/aaa/.include-stamp
$(o)/src/aaa/.include-stamp: $(addprefix $(s)/src/aaa/,$(LIBAAA_INCLUDES))
$(o)/src/aaa/.include-stamp: IDST=aaa

INSTALL_TARGETS+=install-aaa
install-aaa:
ifdef CONFIG_WINDOWS	
	cp $(o)/src/aaa/libaaa.dll /cygdrive/c/aaa/
endif	
	install -m 755 $(o)/src/aaa/libaaa.so $(PREFIX)/lib/libaaa.so
.PHONY: install-aaa
