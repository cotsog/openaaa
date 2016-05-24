DIRS+=src/ctypes

LIBSYS_STATIC=$(o)/src/ctypes/libsys.a
LIBSYS_PICSTATIC=$(o)/src/ctypes/libsys-pic.a

LIBSYS_OBJECTS+=$(addprefix $(o)/src/ctypes/, object attr init exit time rand \
	       	encode log exit gary string bbuf b64 \
                fastbuf resource trans threads \
ff-printf ff-string fb-buffer fb-file fb-mmap fb-param res-mem res-subpool)

$(LIBSYS_STATIC): $(addsuffix .o, $(LIBSYS_OBJECTS))
$(LIBSYS_PICSTATIC): $(addsuffix .$(OO), $(LIBSYS_OBJECTS)) 

