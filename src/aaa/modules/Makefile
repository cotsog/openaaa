DIRS+=src/aaa/modules
include $(s)/src/aaa/modules/openvpn/Makefile
include $(s)/src/aaa/modules/pkcs11/Makefile

APACHE_RUNDIR=lib/apache2/modules
EXTRA_RUNDIRS+=$(APACHE_RUNDIR)
ifdef CONFIG_APACHE2
include $(s)/src/aaa/modules/apache2/Makefile
endif
