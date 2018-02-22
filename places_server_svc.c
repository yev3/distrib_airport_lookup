/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/

#include "places-airports.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
places_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		places_req places_qry_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case PLACES_QRY:
		_xdr_argument = (xdrproc_t) xdr_places_req;
		_xdr_result = (xdrproc_t) xdr_places_ret;
		local = (char *(*)(char *, struct svc_req *)) places_qry_1_svc;
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

extern char* airportsHost;
extern void initTrie(const char* placesPath);

int
main (int argc, char **argv)
{
  char* placesPath = "places2k.txt";

	if (argc < 2 || 3 < argc) {
		printf ("usage: %s <airports-host> [airportsFile]\n", argv[0]);
		exit (1);
	}

  airportsHost = argv[1];

  if (argc == 3) 
    placesPath = argv[2];

  initTrie(placesPath);
  
	register SVCXPRT *transp;

	pmap_unset (PLACES_PROG, PLACES_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PLACES_PROG, PLACES_VERS, places_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PLACES_PROG, PLACES_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PLACES_PROG, PLACES_VERS, places_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PLACES_PROG, PLACES_VERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
