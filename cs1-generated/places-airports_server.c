/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "places-airports.h"

places_ret *
places_qry_1_svc(places_req *argp, struct svc_req *rqstp)
{
	static places_ret  result;

	/*
	 * insert server code here
	 */

	return &result;
}

airports_ret *
airports_qry_1_svc(location *argp, struct svc_req *rqstp)
{
	static airports_ret  result;

	/*
	 * insert server code here
	 */

	return &result;
}
