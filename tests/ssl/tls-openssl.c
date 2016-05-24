/*
 * $tls-openssl.c                                   Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <dlfcn.h>

#include <ctypes/lib.h>
#include <ctypes/list.h>
#include <getopt/opt.h>
#include <mem/map.h>
#include <mem/page.h>
#include <mem/pool.h>
#include <mem/stack.h>
#include <aaa/lib.h>

#ifdef CONFIG_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <arch/lib.h>

#include <asm/dis86/types.h>
#include <asm/dis86/extern.h>
#include <asm/dis86/itab.h>
#include <asm/dis86/decode.h>

#include <sys/mman.h>

//int mprotect(void *addr, size_t len, int prot);

#define SERVER  "www.openssl.org"
#define PORT 443

int
example_call(char *test, int test2)
{
	test2++;
	printf("example_call()\n");
	return 0;
}

arch_define(int, example_call, char *test, int test2)
{
	return 0;
}

arch_sym_define (int, example_call, char *, int);
arch_sym_declare(int, example_call)(char *test, int test2)
{
	printf("arch trampoline example_call()\n");
	return arch_call_org(example_call)(test, test2);
}

arch_sym_define (SSL *, SSL_new, SSL_CTX *);
arch_sym_declare(SSL *, SSL_new)(SSL_CTX *ctx)
{
	printf("arch trampoline SSL_new()\n");
	return arch_call_org(SSL_new)(ctx);
}

arch_dso_define (openssl);
arch_dso_declare(openssl)
{
	arch_sym_link(example_call);
	arch_sym_link(SSL_new);
}

/* The maximum length of an instruction is 16 bytes */
unsigned int arch_insn_max(void) { return 16; }

unsigned int
arch_lde(byte *addr)
{
	ud_t ud_obj;
	ud_init(&ud_obj);

	ud_set_input_buffer(&ud_obj, addr, arch_insn_max());
	ud_set_mode(&ud_obj, CPU_BITS_SIZE);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	unsigned int len = 0, size = 0;

	while (ud_disassemble(&ud_obj)) {
		if (ud_insn_mnemonic(&ud_obj) == UD_Iinvalid)
			break;
		len = ud_insn_len(&ud_obj);
		printf("\taddr=%p asm=%s bytes=%d\n", addr, ud_insn_asm(&ud_obj), len);
		return len;
	}

	return 0;
}

unsigned int
arch_decompose(byte *addr, unsigned int require)
{
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, addr, require);
	ud_set_mode(&ud_obj, CPU_BITS_SIZE);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	unsigned int len = 0, size = 0;
	while (ud_disassemble(&ud_obj)) {
		if (ud_insn_mnemonic(&ud_obj) == UD_Iinvalid)
			break;
		size += len = ud_insn_len(&ud_obj);
		printf("\taddr=%p asm=%s bytes=%d\n", 
		        addr, ud_insn_asm(&ud_obj), len);
	}
	printf("decompose=%d\n", size);
	return size;
}

int
arch_jmp_direct(byte *from, byte *to)
{
	//unconditional jump opcode
	*from = 0xe9;
	// store the relative address from this opcode to our hook function
	*(unsigned long *)(from + 1) = (byte *)to - from - 5;
	return 1;
}

int
arch_tramp(void *org, void *acc)
{
	int rv;
	u32 len = 1 + sizeof(unsigned long);

	int snip = arch_lde(org);

	int size = 16;

	byte *page = _align_ptr(org, CPU_PAGE_SIZE) - CPU_PAGE_SIZE;
	rv = mprotect(page, CPU_PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
	printf("mem prottect status=%d\n", rv);

	rv = mprotect(org, 16, PROT_READ | PROT_EXEC);
}

void
arch_hook(void)
{
	byte *addr = (byte *)example_call;
	unsigned int len = arch_lde(addr);
	printf("arch_lde=%d\n", len);
}

struct tls_conn {
	int fd;
	SSL *ssl;
	SSL_CTX *ctx;
};

static void
show_version(struct opt_item *opt, const char *value, void *data)
{
	printf("TLS Utility using OpenSSL crypto library \n");
	exit(EXIT_SUCCESS);
}

static struct opt_section options = {
OPT_ITEMS {
	OPT_HELP("Options:"),
	OPT_END
	}
};

void
usage(int code)
{
	printf("Usage: tls-openssl <commands-and-parameters>\n\n");
	printf("\t--version,      -V\t package version\n");
	printf("\n");

	if (code != 0)
		exit(code);
}

_noreturn static void
nothing(void)
{
	exit(0);
}

static int
tcp_connect(void)
{
	int error, fd;
	struct hostent *host;
	struct sockaddr_in server;

	host = gethostbyname (SERVER);
	fd = socket (AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror ("socket");
	} else {
		server.sin_family = AF_INET;
		server.sin_port = htons (PORT);
		server.sin_addr = *((struct in_addr *) host->h_addr);
		memset(&(server.sin_zero), 0, 8);
		error = connect (fd, (struct sockaddr *) &server,
		                 sizeof(struct sockaddr));
		if (error == -1) {
			perror("connect");
			close(fd);
			fd = -1;
		}
	}
	return fd;
}

static struct tls_conn *
ssl_connect(void)
{
	struct tls_conn *c = malloc(sizeof(*c));
	c->ssl = NULL;
	c->ctx = NULL;

	if ((c->fd = tcp_connect()) == -1) {
		perror("connect failed");
		return NULL;
	}

	SSL_load_error_strings();
	SSL_library_init();

	c->ctx = SSL_CTX_new(SSLv23_client_method ());
	if (c->ctx == NULL)
		ERR_print_errors_fp(stderr);

	c->ssl = SSL_new(c->ctx);
	if (c->ssl == NULL)
		ERR_print_errors_fp(stderr);

	if (!SSL_set_fd(c->ssl, c->fd))
		ERR_print_errors_fp(stderr);

	if (SSL_connect (c->ssl) != 1)
		ERR_print_errors_fp(stderr);

	return c;
}

static void 
ssl_disconnect(struct tls_conn *c)
{
	if (c->fd)
		close(c->fd);
	if (c->ssl) {
		SSL_shutdown(c->ssl);
		SSL_free(c->ssl);
	}
	if (c->ctx)
		SSL_CTX_free(c->ctx);

	free(c);
}

static char *
ssl_read(struct tls_conn *c)
{
	const int sz = 8192;
	char *rc = NULL;
	int received, count = 0;
	char buffer[8192];

	while (1) {
		if (!rc)
			rc = malloc(sz * sizeof (char) + 1);
		else
			rc = realloc(rc, (count + 1) * sz * sizeof (char) + 1);

		received = SSL_read (c->ssl, buffer, sz);
		if (received <= 0)
			break;

		buffer[received] = '\0';

		if (received > 0)
			strcat (rc, buffer);

		if (received < sz)
			break;
		count++;
	}

	return rc;
}

void
ssl_write(struct tls_conn *c, char *text)
{
	SSL_write(c->ssl, text, strlen(text));
}

int
main(int argc, char *argv[])
{
#ifdef CONFIG_WINDOWS
	WSADATA wsaData;
	int rv = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (rv != 0) {
		printf("WSAStartup failed: %d\n", rv);
		return EXIT_FAILURE;
	}
#endif
	arch_hook();
	arch_dso_init(openssl);

        printf("sym_org_ssl_new: %p sym_act_ssl_new=%p\n",
	       sym_org_SSL_new, sym_act_SSL_new);

	arch_tramp(sym_org_SSL_new, sym_act_SSL_new);

	struct tls_conn *c = ssl_connect();
	if (c == NULL)
		return EXIT_FAILURE;

	ssl_write(c, "GET /\r\n\r\n");
	char *response = ssl_read (c);

	ssl_disconnect(c);
	return EXIT_SUCCESS;
}
