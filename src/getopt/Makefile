DIRS+=src/getopt

LIBOPT_STATIC=$(o)/src/getopt/libopt.a
LIBOPT_PICSTATIC=$(o)/src/getopt/libopt-pic.a

LIBOPT_API_VERSION=1.0.0
LIBOPT_INCLUDES=conf.h config.h getopt.h getopt_int.h getopt-sh.h opt.h

PROGS+=$(LIBOPT_STATIC)

LIBOPT_OBJECTS+=$(addprefix $(o)/src/getopt/, getopt getopt1 opt opt-conf opt-help \
conf-alloc conf-context conf-dump conf-getopt conf-input conf-intr conf-journal \
conf-parse conf-section)

ifdef CONFIG_DARWIN
LIBOPT_OBJECT+=$(ddprefix $(o)/src/getopt/, strchrnul)
endif

$(LIBOPT_STATIC):    $(addsuffix .o, $(LIBOPT_OBJECTS) $(LIBSYS_OBJECTS)) $(LIBSYS_STATIC)
$(LIBOPT_PICSTATIC): $(addsuffix .$(OO), $(LIBOPT_OBJECTS) $(LIBSYS_OBJECTS)) $(LIBSYS_PICSTATIC)
