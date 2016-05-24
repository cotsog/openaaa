DIRS+=tests/ssl
ifdef CONFIG_HAVE_OPENSSL1
TLS_OPENSSL=$(o)/tests/ssl/tls-openssl
TLS_OPENSSL_OBJECTS=$(addprefix $(o)/tests/ssl/, tls-openssl)

$(TLS_OPENSSL): $(addsuffix .o, $(TLS_OPENSSL_OBJECTS)) $(LIBSYS_STATIC) \
$(LIBAAA_STATIC)
$(o)/tests/ssl/tls-openssl: LIBS+=`pkg-config --libs libssl`
ifdef CONFIG_WINDOWS
endif

PROGS+=$(TLS_OPENSSL)
ASM+=$(addsuffix .s, $(TLS_OPENSSL_OBJECTS))
endif
