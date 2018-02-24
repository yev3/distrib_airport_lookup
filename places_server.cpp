/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/

#include "places-airports.h"
#include "places_trie.h"
#include <iostream>
#include <sstream>

char* airportsHost;

// Result of the places server call used in this module
static places_ret placesResult = { };

// Helper to return the error result
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