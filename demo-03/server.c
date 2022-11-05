// RPC server for add.x

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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

static int *addproc_1(struct args *p, struct svc_req *rqstp)
{
    static int result;
    result = p->arg1 + p->arg2;
    return &result;
}

static void addprog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{

    // if (rqstp->rq_proc != ADDPROC)
    // {
    //     svcerr_noproc(transp);
    //     return;
    // }

    switch (rqstp->rq_proc)
    {

    case NULLPROC:
        svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
        return;

    case ADDPROC:
        printf("call add proc...\n");
        struct args arg;
        memset((char *)&arg, 0, sizeof(struct args));
        int ret = svc_getargs(transp, (xdrproc_t)xdr_args, (caddr_t)&arg);
        if (!ret)
        {
            svcerr_decode(transp);
            return;
        }
        int result = arg.arg1 + arg.arg2;
        bool_t reply = svc_sendreply(transp, (xdrproc_t)xdr_int, &result);
        if (!reply)
        {
            svcerr_systemerr(transp);
            return;
        }
        svc_freeargs(transp, (xdrproc_t)xdr_args, (caddr_t)&arg);
        break;

    default:
        svcerr_noproc(transp);
        return;
    }

    // if (!svc_freeargs(transp, (xdrproc_t)xdr_args, (caddr_t)&arg))
    // {
    //     fprintf(stderr, "%s", "unable to free arguments");
    //     exit(EXIT_FAILURE);
    // }

    return;
}

int main(int argc, char **argv)
{
    pmap_unset(ADDPROG, ADDVER);

    register SVCXPRT *transptcp = svctcp_create(RPC_ANYSOCK, 0, 0);
    assert(transptcp);
    register SVCXPRT *transpudp = svcudp_create(RPC_ANYSOCK);
    assert(transpudp);

    bool regtcp = svc_register(transptcp, ADDPROG, ADDVER, addprog_1, IPPROTO_TCP);
    assert(regtcp);
    bool regudp = svc_register(transpudp, ADDPROG, ADDVER, addprog_1, IPPROTO_UDP);
    assert(regudp);

    // bool reg = svc_register(transp, ADDPROG, ADDVER, addprog_1, 0); // dont register in portmapper

    svc_run();

    fprintf(stderr, "%s", "svc_run returned");
    return 0;
}
