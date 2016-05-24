DIRS+=src/aaa/net/tls/nss
LIBAAATLS_OBJECTS+=$(addprefix $(o)/src/aaa/net/tls/nss/, init sig)

$(o)/src/aaa/net/tls/nss/init.o: CFLAGS+=-I./include/nspr -I./include/nss
$(o)/src/aaa/net/tls/nss/init.oo: CFLAGS+=-I./include/nspr -I./include/nss

$(o)/src/aaa/net/tls/nss/sig.o: CFLAGS+=-I./include/nspr -I./include/nss
$(o)/src/aaa/net/tls/nss/sig.oo: CFLAGS+=-I./include/nspr -I./include/nss
