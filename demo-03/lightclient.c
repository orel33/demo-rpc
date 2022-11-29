// RPC light client for add.x

#include <assert.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <stdlib.h>

#define ADDPROG ((u_long)0x20000199)
#define ADDVER ((u_long)1)
#define ADDPROC ((u_long)1)

struct args {
  int arg1;
  int arg2;
};

/* routine used to pack arguments in XDR stream */
bool_t xdr_args(XDR* xdrs, struct args* p)
{
  if (!xdr_int(xdrs, &p->arg1)) return FALSE;
  if (!xdr_int(xdrs, &p->arg2)) return FALSE;
  return TRUE;
}

int main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s server_host\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  char* host = argv[1];

  struct args arg;
  arg.arg1 = 10;
  arg.arg2 = 20;
  int res = 0;

  // Nota Bene: callrpc() uses only UDP/IP as a transport.
  int error = callrpc(host, ADDPROG, ADDVER, ADDPROC, (xdrproc_t)xdr_args, &arg, (xdrproc_t)xdr_int, &res);
  if (error != 0) {
    clnt_perrno(error);
    exit(EXIT_FAILURE);
  }

  printf("result: %d + %d = %d\n", arg.arg1, arg.arg2, res);

  return EXIT_SUCCESS;
}