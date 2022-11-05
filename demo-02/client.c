#include <stdlib.h>
#include <memory.h> /* for memset */
#include <assert.h>
#include "add.h"

bool_t xdr_add_1_argument(XDR *xdrs, add_1_argument *objp)
{
	if (!xdr_int(xdrs, &objp->arg1))
		return FALSE;
	if (!xdr_int(xdrs, &objp->arg2))
		return FALSE;
	return TRUE;
}

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = {25, 0};

int *add_1(int arg1, int arg2, CLIENT *clnt)
{
	add_1_argument arg;
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	arg.arg1 = arg1;
	arg.arg2 = arg2;
	if (clnt_call(clnt, add, (xdrproc_t)xdr_add_1_argument, (caddr_t)&arg,
				  (xdrproc_t)xdr_int, (caddr_t)&clnt_res,
				  TIMEOUT) != RPC_SUCCESS)
	{
		return (NULL);
	}
	return (&clnt_res);
}

void addprog_1(char *host)
{
	CLIENT *clnt = clnt_create(host, ADDPROG, ADDVER, "tcp");

	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}

	int add_1_arg1 = 10;
	int add_1_arg2 = 20;
	int *result_1 = add_1(add_1_arg1, add_1_arg2, clnt);
	if (result_1 == NULL)
	{
		clnt_perror(clnt, "call failed");
	}

	printf("result: %d + %d = %d\n", add_1_arg1, add_1_arg2, *result_1);

	clnt_destroy(clnt);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s server_host\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *host = argv[1];
	addprog_1(host);

	return EXIT_SUCCESS;
}
