DIRS+=tools/sysconf

SYSCONF=$(o)/tools/sysconf/sysconf
SYSCONF_OBJECTS=$(addprefix $(o)/tools/sysconf/, sysconf)
SYSCONF_CFLAGS=`pkg-config --cflags libusb-1.0`

$(SYSCONF): $(addsuffix .o, $(SYSCONF_OBJECTS))
$(SYSCONF): $(LIBOPT_STATIC) $(LIBSYS_STATIC) $(LIBCOMPAT_STATIC) $(LIBS)

#$(o)/tools/sysconf/sysconf.o: CFLAGS+=$(CONFIG_HAVE_LIBUSB_1_0_CFLAGS)

PROGS+=$(SYSCONF)
ASM+=$(addsuffix .s, $(SYSCONF_OBJECTS))

