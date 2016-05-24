# Makefile for ALUCID Apache2 Module
# (c) 2015 Daniel Kubec <niel@rtfm.cz>
DIRS+=src/aaa/modules/apache2

#APACHE_PREFIX=`apxs -q | grep ^prefix | cut -d"=" -f2`
APACHE_CFLAGS:=-I.$(APACHE2_INCLUDES_DIR)
APACHE_CFLAGS+=-I/opt/aaa/httpd/include

mod_tls_aaa_OBJECTS=$(addprefix $(o)/src/aaa/modules/apache2/, mod_tls_aaa socache ssl)

$(o)/src/aaa/modules/apache2/mod_tls_aaa.so: $(addsuffix .oo, $(mod_tls_aaa_OBJECTS)) $(LIBAAA_PICSTATIC)
$(o)/src/aaa/modules/apache2/mod_tls_aaa.so: SO_RUNDIR=$(APACHE_RUNDIR)
$(o)/src/aaa/modules/apache2/mod_tls_aaa.so: LDFLAGS+=-L$(APACHE2_LIBRARY_DIR) `apr-2-config --link-ld`
$(o)/src/aaa/modules/apache2/mod_tls_aaa.so: LIBS+=`apr-2-config --link-ld`
#$(o)/src/aaa/modules/apache2/mod_tls_aaa.so: LSHARED+=-Wl, --version-script=$(s)/src/aaa/modules/apache2/mod_tls_aaa.ver

$(o)/src/aaa/modules/apache2/mod_tls_aaa.oo: CFLAGS+=$(APACHE_CFLAGS)
$(o)/src/aaa/modules/apache2/socache.oo: CFLAGS+=$(APACHE_CFLAGS)
$(o)/src/aaa/modules/apache2/ssl.oo: CFLAGS+=$(APACHE_CFLAGS)

$(o)/src/aaa/modules/apache2/mod_tls_aaa.o: CFLAGS+=$(APACHE_CFLAGS)
$(o)/src/aaa/modules/apache2/socache.o: CFLAGS+=$(APACHE_CFLAGS)
$(o)/src/aaa/modules/apache2/ssl.o: CFLAGS+=$(APACHE_CFLAGS)

run/$(APACHE_RUNDIR)/mod_tls_aaa.so: $(o)/src/aaa/modules/apache2/mod_tls_aaa.so
	$(Q)$(call symlink,$(o)/src/aaa/modules/apache2/mod_tls_aaa.so,run/$(APACHE_RUNDIR))

PROGS+=run/$(APACHE_RUNDIR)/mod_tls_aaa.so

INSTALL_TARGETS+=install-mod_tls_aaa
install-mod_tls_aaa:
ifdef CONFIG_WINDOWS
	cp $(o)/src/aaa/libaaa.dll $(APACHE2_MODULES_DIR)/
	cp $(o)/src/net/tls/libaaa_tls.dll $(APACHE2_MODULES_DIR)/
	cp $(o)/src/aaa/modules/apache2/mod_tls_aaa.so $(APACHE2_MODULES_DIR)/
	install -m 755 etc/apache2/tls_aaa.conf $(APACHE2_MODULES_DIR)/tls_aaa.conf
	install -m 755 etc/apache2/tls_aaa.load $(APACHE2_MODULES_DIR)/tls_aaa.load
else
	install -m 755 run/$(APACHE_RUNDIR)/mod_tls_aaa.so $(APACHE2_MODULES_DIR)/mod_tls_aaa.so
	install -m 755 etc/apache2/tls_aaa.conf $(APACHE2_MODULES_DIR)/tls_aaa.conf
	install -m 755 etc/apache2/tls_aaa.load $(APACHE2_MODULES_DIR)/tls_aaa.load
endif	
.PHONY: install-mod_tls_aaa
