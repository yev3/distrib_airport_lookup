/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/

#include "places-airports.h"
#include "places_trie.h"
#include <iostream>

char* airportsHost;

void setEmptyError(places_ret &ret) {
  ret.err = 1;
  ret.places_ret_u.error_msg = (char*)"Place not found.";
}

void setAmbiguousError(places_ret & ret, const TrieQueryResult &found) {
  static std::string ambiguousMessage;  

  const auto &fst = found.places.front().get();
  const auto &lst = found.places.back().get();
  ambiguousMessage.assign("Ambiguous result: " +
                          fst.cityName + "," + fst.state + " .. " + lst.
                          cityName + "," + lst.state);
  ret.err = 1;
  ret.places_ret_u.error_msg = (char*)ambiguousMessage.c_str();
}

places_ret * places_qry_1_svc(places_req *req, struct svc_req *rqstp) {
  static places_ret result;
  static std::string errorMessage; 

  // Free previously allocated memory
  // Note: Only freeing the results from the airports that were allocated
  if (result.err == 0)
    xdr_free((xdrproc_t)xdr_airports, (char*)result.places_ret_u.results.results);

  // Clear previous state
  result = { };
  errorMessage.clear();

  if (req->req_type == REQ_NAMED) {
    // Perform a query on the trie and resolve ambiguity if can
    const auto &city = req->places_req_u.named.name;
    const auto &state = req->places_req_u.named.state;
    const auto found = queryPlace(city, state);

    if (found.places.empty()) {
      setEmptyError(result);
    } else if (found.isAmbiguous) {
      setAmbiguousError(result, found);
    } else {
      auto &foundRec = found.places.front().get();
      auto& pl = result.places_ret_u.results.request;

      pl.name = (char*)foundRec.cityName.c_str();
      pl.state = (char*)foundRec.state.c_str();
      pl.loc.latitude = foundRec.loc.latitude;
      pl.loc.longitude = foundRec.loc.longitude;
    }
  } else if (req->req_type == REQ_LAT_LONG) {
    auto& pl = result.places_ret_u.results.request;

    pl.name = (char*)"Latitude / longitude coordinate";
    pl.state = (char*)"NA";
    pl.loc.latitude = req->places_req_u.loc.latitude;
    pl.loc.latitude = req->places_req_u.loc.longitude;
  } else {
    result.err = 1;
    result.places_ret_u.error_msg = (char*)"Unrecognized request type.";
  }

  if (result.err)
    return &result;

  location *loc = &result.places_ret_u.results.request.loc;

  CLIENT *clnt = clnt_create(airportsHost, AIRPORTS_PROG, AIRPORTS_VERS, "udp");
  if (clnt == nullptr) {
    clnt_pcreateerror(airportsHost);
    result.err = 1;
    result.places_ret_u.error_msg =
      (char*)"Unable to connect to airports server.";
    return &result;
  }

	airports_ret *airpResults = airports_qry_1(loc, clnt);
	if (airpResults == nullptr) {
		clnt_perror (clnt, "call failed");
    result.err = 1;
    result.places_ret_u.error_msg =
      (char*)"Remote call to airports server failed.";
    clnt_destroy (clnt);
    return &result;
	}

  if (airpResults->err) {
    result.err = 1;
    errorMessage = airpResults->airports_ret_u.error_msg;
    result.places_ret_u.error_msg = (char*)errorMessage.c_str();
  } else {
    // Take ownership of the resulting memory. It will be freed on next call
    memcpy(&result.places_ret_u.results.results, 
           &airpResults->airports_ret_u.results,
           sizeof(airports));
    memset(&airpResults->airports_ret_u.results, 0, sizeof(airports));
  }

	clnt_freeres(clnt, (xdrproc_t)xdr_airports_ret, (caddr_t)airpResults);
  clnt_destroy(clnt);

  return &result;
}