// RPC server for test.x (without rpcbind)

#include <assert.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TESTPROG 0x20000001
#define TESTVER 1

#define PIPROC 1
#define INCPROC 2
#define ADDPROC 3
#define ECHOPROC 4

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

static void pi(struct svc_req* rqstp, SVCXPRT* transp)
{
  static double result = 3.1415926;
  bool_t reply = svc_sendreply(transp, (xdrproc_t)xdr_double, &result);
  if (!reply) {
    svcerr_systemerr(transp);
    return;
  }
}

static void inc(struct svc_req* rqstp, SVCXPRT* transp)
{
  int arg;
  int ret = svc_getargs(transp, (xdrproc_t)xdr_int, &arg);
  if (!ret) {
    svcerr_decode(transp);
    return;
  }
  int result = arg + 1;
  bool_t reply = svc_sendreply(transp, (xdrproc_t)xdr_int, &result);
  if (!reply) {
    svcerr_systemerr(transp);
    return;
  }
  svc_freeargs(transp, (xdrproc_t)xdr_int, &arg);
}

static void add(struct svc_req* rqstp, SVCXPRT* transp)
{
  struct two_int arg;
  memset((char*)&arg, 0, sizeof(struct two_int));
  int ret = svc_getargs(transp, (xdrproc_t)xdr_two_int, &arg);
  if (!ret) {
    svcerr_decode(transp);
    return;
  }
  int result = arg.arg1 + arg.arg2;
  bool_t reply = svc_sendreply(transp, (xdrproc_t)xdr_int, &result);
  if (!reply) {
    svcerr_systemerr(transp);
    return;
  }
  svc_freeargs(transp, (xdrproc_t)xdr_two_int, (caddr_t)&arg);
}

static void echo(struct svc_req* rqstp, SVCXPRT* transp)
{
  char* arg = NULL;
  int ret = svc_getargs(transp, (xdrproc_t)xdr_wrapstring, &arg);
  if (!ret && !arg) {
    svcerr_decode(transp);
    return;
  }
  char* result = arg;  // echo
  bool_t reply = svc_sendreply(transp, (xdrproc_t)xdr_wrapstring, &result);
  if (!reply) {
    svcerr_systemerr(transp);
    return;
  }
  svc_freeargs(transp, (xdrproc_t)xdr_wrapstring, &arg);
}

static void handle(struct svc_req* rqstp, SVCXPRT* transp)
{
  switch (rqstp->rq_proc) {
    case NULLPROC:
      /* By convention, procedure zero of any protocol takes no parameters and returns no results.*/
      printf("call null proc (%d)...\n", rqstp->rq_proc);
      bool_t reply = svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
      if (!reply) {
        svcerr_systemerr(transp);
        return;
      }
      return;

    case PIPROC:
      printf("call pi proc (%d)...\n", rqstp->rq_proc);
      pi(rqstp, transp);
      return;

    case INCPROC:
      printf("call inc proc (%d)...\n", rqstp->rq_proc);
      inc(rqstp, transp);
      return;

    case ADDPROC:
      printf("call add proc (%d)...\n", rqstp->rq_proc);
      add(rqstp, transp);
      return;

    case ECHOPROC:
      printf("call echo proc (%d)...\n", rqstp->rq_proc);
      echo(rqstp, transp);
      return;

    default:
      printf("call invalid proc (%d)...\n", rqstp->rq_proc);
      svcerr_noproc(transp);
      return;
  }
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* bind server to a given port */
  int port = atoi(argv[1]);

  /* create socket server and bind it to a given port */
  printf("bind UDP/IP socket at port %d\n", port);
  int yes = 1;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_port = htons(port);
  int s_udp = socket(AF_INET, SOCK_DGRAM, 0);
  if (s_udp == -1) perror("udp socket");
  setsockopt(s_udp, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  bind(s_udp, (struct sockaddr*)&addr, sizeof(addr));

  printf("register udp service\n");
  SVCXPRT* transp = svcudp_create(s_udp);
  assert(transp);
  bool_t reg = svc_register(transp, TESTPROG, TESTVER, handle, 0);  // 0: don't register in rpcbind
  assert(reg);                                                      // fail to register service

  svc_run();

  return 0;
}
