/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "places-airports.h"

char* airportsHost;

airports_ret* airports_prog_1(char *host, location_s *loc)
{
	CLIENT *clnt;
	airports_ret *airportsFromServer;

#ifndef	DEBUG
	clnt = clnt_create (host, AIRPORTS_PROG, AIRPORTS_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	airportsFromServer = air_qry_1(loc, clnt);
	if (airportsFromServer == (airports_ret *) NULL) {
		clnt_perror (clnt, "call failed");
	}

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */

  return airportsFromServer;
}

places_ret *
places_qry_1_svc(client_req_t *argp, struct svc_req *rqstp)
{
  static places_ret placesRet = { 0 };

  printf("PLACES received city: %s\n", argp->city);
  printf("PLACES received state: %s\n", argp->state);

  placesRet.err = 0;
  placesRet.places_ret_u.result.place = "Dummy result place";

  // TODO: code to lookup the lat + long of the place

  location_s placeLocation = (location_s) {
    .latitude = 0.3, 
    .longitude = -0.2
  };

  airports_ret * airportsRet = airports_prog_1(airportsHost, &placeLocation);

  if(airportsRet->err) {
    printf("Error: %s\n", airportsRet->airports_ret_u.error_msg);
  } else {
    memcpy(&placesRet.places_ret_u.result.airports,
           &airportsRet->airports_ret_u.airports,
           sizeof(airp_dist_recs_t));
  }

  //airports_t sampleAirports = { 0 };
  //sampleAirports.airports_t_len = 5;

	/*
	 * insert server code here
	 */

	return &placesRet;
}