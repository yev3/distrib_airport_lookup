/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *
 * Places server. Uses Trie to perform prefix matches.
 *****************************************************************************/

#include "places_airports.h"
#include "places_trie.h"
#include <cstdio>
#include <cstdlib>
#include <rpc/pmap_clnt.h>
#include <cstring>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

// Result of the places server call used in this module
static places_ret placesResult = { };

// Host of the airports server provided by the user
static char* airportsHost;

// Places server RPC program handle registered with rpcbind (auto-generated)
static void places_prog_1(struct svc_req *rqstp, register SVCXPRT *transp) {
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

// Program entry point
/******************************************************************************/

int main (int argc, char **argv)
{
	if (argc < 2 || 3 < argc) {
		printf ("usage: %s <airports-host> [airportsFile]\n", argv[0]);
		exit (1);
	}

	airportsHost = argv[1];
	const char* placesPath = "places2k.txt";

  if (argc == 3) {
		placesPath = argv[2];
	} else {
		printf("Note: places file path not specified, using `places2k.txt`\n");
	}

  initTrie(placesPath);

  // Note: code below was auto-generated

	register SVCXPRT *transp;

	pmap_unset (PLACES_PROG, PLACES_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PLACES_PROG, PLACES_VERS,
                    (void (*)())(places_prog_1), IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PLACES_PROG, PLACES_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PLACES_PROG, PLACES_VERS,
                    (void (*)())(places_prog_1), IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PLACES_PROG, PLACES_VERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}

// RPC server program logic and service routine below
/******************************************************************************/

// Helper to return the error result
places_ret *errorResult(const std::string& msg);

// Helper to construct an ambiguous result error message
std::string buildAmbiguousErrorMsg(const TrieQueryResult &found);

// Helper to set the place in result to be from a trie
void setPlaceCityRecord(const CityRecord &cityRec);

// Helper to set the place in result to be a lat/long point user wanted
void setPlaceLatLong(const location &loc);

// Helper to connect and query the airports server. Results forwarded to user.
places_ret *airportsQueryResult(location *ploc);

// Program that services RPC requests to places server
places_ret * places_qry_1_svc(places_req *req, struct svc_req *rqstp) {
  static std::string errorMessage;

  // Free previously allocated memory
  xdr_free((xdrproc_t)xdr_places_ret, (char*)&placesResult);

  // Clear previous state
  placesResult = { };
  errorMessage.clear();

  if (req->req_type == REQ_NAMED) {
    // Perform a query on the trie and resolve ambiguity if can
    const auto found = queryPlace(req->places_req_u.named);

    // Trie could not find any matches
    if (found.places.empty()) {
      return errorResult("Place not found.");
    }

      // Trie search is ambiguous and returned the first and last in range
    else if (found.isAmbiguous) {
      return errorResult(buildAmbiguousErrorMsg(found));
    }

      // Set the places query place to be the result from trie
    else {
      const auto &foundRec = found.places.front().get();
      setPlaceCityRecord(foundRec);
    }
  }

    // Lat / long request from client bypasses trie search
  else if (req->req_type == REQ_LAT_LONG) {
    setPlaceLatLong(req->places_req_u.loc);
  }
  else {
    return errorResult("Unrecognized request type.");
  }

  // places server will return the results of the call to airports server
  location *loc = &placesResult.places_ret_u.results.request.loc;
  return airportsQueryResult(loc);
}

places_ret *errorResult(const std::string& msg) {
  placesResult.err = 1;
  placesResult.places_ret_u.error_msg = strdup(msg.c_str());
  return &placesResult;
}

std::string buildAmbiguousErrorMsg(const TrieQueryResult &found) {
  const auto &fst = found.places.front().get();
  const auto &lst = found.places.back().get();

  std::stringstream strm;
  strm << "Ambiguous result: " << fst.cityName << "," << fst.state << " .. "
       << lst.cityName << "," << lst.state;
  return strm.str();
}

void setPlaceCityRecord(const CityRecord &cityRec) {
  auto& pl = placesResult.places_ret_u.results.request;

  pl.name = strdup(cityRec.cityName.c_str());
  pl.state = strdup(cityRec.state.c_str());
  pl.loc.latitude = cityRec.loc.latitude;
  pl.loc.longitude = cityRec.loc.longitude;
}

void setPlaceLatLong(const location &loc) {
  auto& pl = placesResult.places_ret_u.results.request;

  pl.name = strdup("Latitude / longitude coordinate");
  pl.state = strdup("  ");
  pl.loc.latitude = loc.latitude;
  pl.loc.longitude = loc.longitude;
}

places_ret *airportsQueryResult(location *ploc) {
  CLIENT *clnt = clnt_create(airportsHost, AIRPORTS_PROG, AIRPORTS_VERS, "udp");

  if (clnt == nullptr) {
    clnt_pcreateerror(airportsHost);
    return errorResult("Unable to connect to airports server.");
  }

  airports_ret *airportsResult = airports_qry_1(ploc, clnt);
  if (airportsResult == nullptr) {
    clnt_perror (clnt, "call failed");
    clnt_destroy (clnt);
    return errorResult("Remote call to airports server failed.");
  }

  if (airportsResult->err) {
    errorResult(airportsResult->airports_ret_u.error_msg);
  }
  else {
    // Instead of re-allocating memory for received airports server results,
    // transfer ownership of airports results to places results, the memory
    // will then be freed on the next call to the places server
    memcpy(&placesResult.places_ret_u.results.results[0],
           &airportsResult->airports_ret_u.results[0],
           sizeof(airports));
    memset(&airportsResult->airports_ret_u.results[0], 0, sizeof(airports));
  }

  clnt_freeres(clnt, (xdrproc_t)xdr_airports_ret, (caddr_t)airportsResult);
  clnt_destroy(clnt);

  return &placesResult;
}