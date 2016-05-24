DIRS+=tools/aaa

AAA=$(o)/tools/aaa/aaa
AAA_OBJECTS=$(addprefix $(o)/tools/aaa/, aaa)
ifdef CONFIG_HAVE_LIBEDIT
AAA_OBJECTS+=$(addprefix $(o)/tools/aaa/, console)
$(o)/toos/aaa/console.o: LDFLAGS+=`pkg-config --libs libedit`
$(AAA): LIBS+=`pkg-config --libs libedit`
endif

$(AAA): $(addsuffix .o, $(AAA_OBJECTS)) $(LIBOPT_STATIC) $(LIBSYS_STATIC) $(LIBCOMPAT_STATIC) $(LIBS)

PROGS+=$(AAA)
ASM+=$(addsuffix .s, $(AAA_OBJECTS))

