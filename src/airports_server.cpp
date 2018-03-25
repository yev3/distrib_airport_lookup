/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *
 * Airports server. Uses KD tree to perform KNN lookup from latitude/longitude
 *****************************************************************************/

#include "places_airports.h"
#include "airports_kd_tree.h"
#include <cstdio>
#include <cstdlib>
#include <rpc/pmap_clnt.h>
#include <cstring>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

// Airports server RPC program handle registered with rpcbind (auto-generated)
static void airports_prog_1(struct svc_req *rqstp, register SVCXPRT *transp) {
    union {
        location airports_qry_1_arg;
    } argument;
    char *result;
    xdrproc_t _xdr_argument, _xdr_result;
    char *(*local)(char *, struct svc_req *);

    switch (rqstp->rq_proc) {
    case NULLPROC:
        (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
        return;

    case AIRPORTS_QRY:
        _xdr_argument = (xdrproc_t) xdr_location;
        _xdr_result = (xdrproc_t) xdr_airports_ret;
        local = (char *(*)(char *, struct svc_req *)) airports_qry_1_svc;
        break;

    default:
        svcerr_noproc (transp);
        return;
    }
    memset ((char *)&argument, 0, sizeof (argument));
    if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
        svcerr_decode (transp);
        return;
    }
    result = (*local)((char *)&argument, rqstp);
    if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
        svcerr_systemerr (transp);
    }
    if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
        fprintf (stderr, "%s", "unable to free arguments");
        exit (1);
    }
    return;
}

// Program that services RPC requests to airports server
airports_ret * airports_qry_1_svc(location *argp, struct svc_req *rqstp) {
    static airports_ret  result;

    // Note: no need to free memory from previous query, since all is static
    //       airport_ret contains pointers to airport records in KD tree

    // Zero-out previous query
    result = { };

    // Find 5 closest
    airport *const closest = kd5Closest(*argp);

    // Copy over pointers from results
    memcpy(&result.airports_ret_u.results[0], closest, sizeof(airports));

    return &result;
}

int main (int argc, char **argv) {
  const char* airportsPath = "airport-locations.txt";
  if (argc < 2) {
    printf("Note: airports path not specified, using `airport-locations.txt`\n");
  } else {
    airportsPath = argv[1];
  }

  // Initialize the KD tree
  initKD(airportsPath);

    register SVCXPRT *transp;

    pmap_unset (AIRPORTS_PROG, AIRPORTS_VERS);

    transp = svcudp_create(RPC_ANYSOCK);
    if (transp == NULL) {
        fprintf (stderr, "%s", "cannot create udp service.");
        exit(1);
    }
    if (!svc_register(transp, AIRPORTS_PROG, AIRPORTS_VERS,
                      airports_prog_1, IPPROTO_UDP)) {
        fprintf (stderr, "%s", "unable to register (AIRPORTS_PROG, AIRPORTS_VERS, udp).");
        exit(1);
    }

    transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    if (transp == NULL) {
        fprintf (stderr, "%s", "cannot create tcp service.");
        exit(1);
    }
    if (!svc_register(transp, AIRPORTS_PROG, AIRPORTS_VERS,
                     airports_prog_1, IPPROTO_TCP)) {
        fprintf (stderr, "%s", "unable to register (AIRPORTS_PROG, AIRPORTS_VERS, tcp).");
        exit(1);
    }

    svc_run ();
    fprintf (stderr, "%s", "svc_run returned");
    exit (1);
    /* NOTREACHED */
}

