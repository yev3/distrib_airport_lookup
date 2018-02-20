/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "places-airports.h"


void
places_prog_1(char *host)
{
	CLIENT *clnt;
	places_ret  *result_1;
	client_req_t  places_qry_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, PLACES_PROG, PLACES_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = places_qry_1(&places_qry_1_arg, clnt);
	if (result_1 == (places_ret *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


void
airports_prog_1(char *host)
{
	CLIENT *clnt;
	airports_ret  *result_1;
	location_s  air_qry_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, AIRPORTS_PROG, AIRPORTS_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = air_qry_1(&air_qry_1_arg, clnt);
	if (result_1 == (airports_ret *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	places_prog_1 (host);
	airports_prog_1 (host);
exit (0);
}
