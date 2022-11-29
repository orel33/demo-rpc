#include <memory.h>
#include <netinet/in.h>
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "add.h"

#ifndef SIG_PF
#define SIG_PF void (*)(int)
#endif

bool_t xdr_add_1_argument(XDR* xdrs, add_1_argument* objp)
{
  if (!xdr_int(xdrs, &objp->arg1)) return FALSE;
  if (!xdr_int(xdrs, &objp->arg2)) return FALSE;
  return TRUE;
}

int* add_1_svc(int arg1, int arg2, struct svc_req* rqstp)
{
  static int result;
  result = arg1 + arg2;
  return &result;
}

static int* _add_1(add_1_argument* argp, struct svc_req* rqstp) { return (add_1_svc(argp->arg1, argp->arg2, rqstp)); }

static void addprog_1(struct svc_req* rqstp, register SVCXPRT* transp)
{
  union {
    add_1_argument add_1_arg;
  } argument;
  char* result;
  xdrproc_t _xdr_argument, _xdr_result;
  char* (*local)(char*, struct svc_req*);

  switch (rqstp->rq_proc) {
    case NULLPROC:
      (void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char*)NULL);
      return;

    case add:
      _xdr_argument = (xdrproc_t)xdr_add_1_argument;
      _xdr_result = (xdrproc_t)xdr_int;
      local = (char* (*)(char*, struct svc_req*))_add_1;
      break;

    default:
      svcerr_noproc(transp);
      return;
  }
  memset((char*)&argument, 0, sizeof(argument));
  if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
    svcerr_decode(transp);
    return;
  }
  result = (*local)((char*)&argument, rqstp);
  if (result != NULL && !svc_sendreply(transp, (xdrproc_t)_xdr_result, result)) {
    svcerr_systemerr(transp);
  }
  if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
    fprintf(stderr, "%s", "unable to free arguments");
    exit(1);
  }
  return;
}

int main(int argc, char** argv)
{
  register SVCXPRT* transp;

  pmap_unset(ADDPROG, ADDVER);

  transp = svcudp_create(RPC_ANYSOCK);
  if (transp == NULL) {
    fprintf(stderr, "%s", "cannot create udp service.");
    exit(1);
  }
  if (!svc_register(transp, ADDPROG, ADDVER, addprog_1, IPPROTO_UDP)) {
    fprintf(stderr, "%s", "unable to register (ADDPROG, ADDVER, udp).");
    exit(1);
  }

  transp = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (transp == NULL) {
    fprintf(stderr, "%s", "cannot create tcp service.");
    exit(1);
  }
  if (!svc_register(transp, ADDPROG, ADDVER, addprog_1, IPPROTO_TCP)) {
    fprintf(stderr, "%s", "unable to register (ADDPROG, ADDVER, tcp).");
    exit(1);
  }

  svc_run();
  fprintf(stderr, "%s", "svc_run returned");
  exit(1);
  /* NOTREACHED */
}
