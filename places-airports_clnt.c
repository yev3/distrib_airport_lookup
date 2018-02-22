/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/

#include <memory.h> /* for memset */
#include "places-airports.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

places_ret *
places_qry_1(places_req *argp, CLIENT *clnt)
{
	static places_ret clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, PLACES_QRY,
		(xdrproc_t) xdr_places_req, (caddr_t) argp,
		(xdrproc_t) xdr_places_ret, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

airports_ret *
airports_qry_1(location *argp, CLIENT *clnt)
{
	static airports_ret clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, AIRPORTS_QRY,
		(xdrproc_t) xdr_location, (caddr_t) argp,
		(xdrproc_t) xdr_airports_ret, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
