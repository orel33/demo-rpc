// RPC client for test.x

#include <stdlib.h>
#include <assert.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>

#define TESTPROG 0x20000001
#define TESTVER 1

#define PIPROC 1
#define INCPROC 2
#define ADDPROC 3
#define ECHOPROC 4

static struct timeval TIMEOUT = {25, 0};

struct two_int
{
    int arg1;
    int arg2;
};

bool_t xdr_two_int(XDR *xdrs, struct two_int *p)
{
    if (!xdr_int(xdrs, &p->arg1))
        return FALSE;
    if (!xdr_int(xdrs, &p->arg2))
        return FALSE;
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <server host>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *host = argv[1];

    CLIENT *clnt = clnt_create(host, TESTPROG, TESTVER, "udp"); /* or "tcp" */

    if (clnt == NULL)
    {
        clnt_pcreateerror(host);
        exit(EXIT_FAILURE);
    }

    // call add procedure
    struct two_int add_arg;
    add_arg.arg1 = 10;
    add_arg.arg2 = 20;
    int add_res = 0;
    int add_ret = clnt_call(clnt, ADDPROC, (xdrproc_t)xdr_two_int, &add_arg, (xdrproc_t)xdr_int, &add_res, TIMEOUT);
    if (add_ret != RPC_SUCCESS)
    {
        clnt_perror(clnt, "call add failed!");
        clnt_destroy(clnt);
        exit(EXIT_FAILURE);
    }
    printf("add result: %d + %d = %d\n", add_arg.arg1, add_arg.arg2, add_res);

     // call echo procedure
    char * echo_arg = "hello";
    char * echo_res = NULL;
    int echo_ret = clnt_call(clnt, ECHOPROC, (xdrproc_t)xdr_wrapstring, &echo_arg, (xdrproc_t)xdr_wrapstring, &echo_res, TIMEOUT);
    if (echo_ret != RPC_SUCCESS)
    {
        clnt_perror(clnt, "call echo failed!");
        clnt_destroy(clnt);
        exit(EXIT_FAILURE);
    }
    printf("echo result: %s\n", echo_res);

    clnt_destroy(clnt);

    return EXIT_SUCCESS;
}
