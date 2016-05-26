/*
 * The MIT License (MIT)                     (TLS) The Transport Layer Security 
 *                               Copyright (c) 2015 Daniel Kubec <niel@rtfm.cz>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __NET_TLS_CONF_H__
#define __NET_TLS_CONF_H__

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include <sys/compiler.h>
#include <sys/cpu.h>
#include <mem/unaligned.h>
#include <generic/opt/conf.h>

struct cf_tls_rfc5705 {
	char *context;
	char *label;
	unsigned int length;
};

extern struct cf_section cf_tls_section;
extern struct cf_tls_rfc5705 cf_tls_rfc5705;

#endif
