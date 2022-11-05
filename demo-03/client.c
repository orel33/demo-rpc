// RPC client for add.x

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <rpc/rpc.h>

struct args
{
	int arg1;
	int arg2;
};

#define ADDPROG 0x20000199
#define ADDVER 1
#define ADDPROC 1

/* routine used to pack arguments in XDR stream */
bool_t xdr_args(XDR *xdrs, struct args *p)
{
	if (!xdr_int(xdrs, &p->arg1))
		return FALSE;
	if (!xdr_int(xdrs, &p->arg2))
		return FALSE;
	return TRUE;
}

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = {25, 0};

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s server_host\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *host = argv[1];

	CLIENT *clnt = clnt_create(host, ADDPROG, ADDVER, "tcp");

	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(EXIT_FAILURE);
	}

	struct args arg;
	arg.arg1 = 10;
	arg.arg2 = 20;
	int res = 0;

	int ret = clnt_call(clnt,
						ADDPROC,
						(xdrproc_t)xdr_args,
						(caddr_t)&arg,
						(xdrproc_t)xdr_int,
						(caddr_t)&res,
						TIMEOUT);

	if (ret != RPC_SUCCESS)
	{
		clnt_perror(clnt, "call failed!");
		clnt_destroy(clnt);
		exit(EXIT_FAILURE);
	}

	printf("result: %d + %d = %d\n", arg.arg1, arg.arg2, res);

	clnt_destroy(clnt);
	return EXIT_SUCCESS;
}
