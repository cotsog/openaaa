#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>
CFLAGS="-pedantic -std=c99 -pedantic-errors `pkg-config --cflags openssl`"
LDFLAGS=`pkg-config --libs openssl`

cat << "END" | $@ $CFLAGS -x c - -c -o /dev/null > /dev/null 2>&1 && echo "y"
#include <stdlib.h>
#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>
int main(void)
{
	SSL_export_keying_material((SSL *)NULL, NULL, 0, NULL, 0,  NULL, 0, 0);
}
END
