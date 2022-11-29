// RPC client for test.x (without rpcbind)

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define TESTPROG 0x20000001
#define TESTVER 1

#define PIPROC 1
#define INCPROC 2
#define ADDPROC 3
#define ECHOPROC 4

static struct timeval RTIMEOUT = {1, 0};  // retry timeout
static struct timeval TTIMEOUT = {5, 0};  // total timeout

struct two_int {
  int arg1;
  int arg2;
};

bool_t xdr_two_int(XDR* xdrs, struct two_int* p)
{
  if (!xdr_int(xdrs, &p->arg1)) return FALSE;
  if (!xdr_int(xdrs, &p->arg2)) return FALSE;
  return TRUE;
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    printf("Usage: %s <hostname> <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  char* hostname = argv[1];
  int port = atoi(argv[2]);

  struct hostent* hlist = gethostbyname(hostname);
  if (!hlist) {
    printf("error: gethostbyname\n");
    exit(EXIT_FAILURE);
  }
  uint32_t ipv4 = *(uint32_t*)hlist->h_addr_list[0];  // 32 bits address

  // server address
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ipv4;

  int sock = RPC_ANYSOCK;
  CLIENT* clnt = clntudp_create(&addr, TESTPROG, TESTVER, RTIMEOUT, &sock);
  if (!clnt) {
    clnt_pcreateerror(hostname);
    exit(EXIT_FAILURE);
  }

  // call add procedure
  struct two_int add_arg;
  add_arg.arg1 = 10;
  add_arg.arg2 = 20;
  int add_res = 0;
  int add_ret = clnt_call(clnt, ADDPROC, (xdrproc_t)xdr_two_int, &add_arg, (xdrproc_t)xdr_int, &add_res, TTIMEOUT);
  if (add_ret != RPC_SUCCESS) {
    clnt_perror(clnt, "call add failed!");
    clnt_destroy(clnt);
    exit(EXIT_FAILURE);
  }
  printf("add result: %d + %d = %d\n", add_arg.arg1, add_arg.arg2, add_res);

  // call echo procedure
  char* echo_arg = "hello";
  char* echo_res = NULL;
  int echo_ret =
      clnt_call(clnt, ECHOPROC, (xdrproc_t)xdr_wrapstring, &echo_arg, (xdrproc_t)xdr_wrapstring, &echo_res, TTIMEOUT);
  if (echo_ret != RPC_SUCCESS) {
    clnt_perror(clnt, "call echo failed!");
    clnt_destroy(clnt);
    exit(EXIT_FAILURE);
  }
  printf("echo result: %s\n", echo_res);

  clnt_destroy(clnt);

  return EXIT_SUCCESS;
}
