DIRS+=tools/hotplug
ifndef CONFIG_WINDOWS
HOTPLUG=$(o)/tools/hotplug/hotplug
HOTPLUG_OBJECTS=$(addprefix $(o)/tools/hotplug/, hotplug)
HOTPLUG_CFLAGS=`pkg-config --cflags libusb-1.0`

$(HOTPLUG): $(addsuffix .o, $(HOTPLUG_OBJECTS))
$(HOTPLUG): $(LIBSYS_STATIC) $(LIBHOTPLUG_STATIC) $(LIBS)
$(o)/tools/hotplug/hotplug.o: CFLAGS+=$(HOTPLUG_CFLAGS) $(CONFIG_HAVE_LIBUSB_1_0_CFLAGS)

PROGS+=$(HOTPLUG)
ASM+=$(addsuffix .s, $(HOTPLUG_OBJECTS))
endif
