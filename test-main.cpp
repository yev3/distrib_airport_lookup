#include "common.h"
#include "places_trie.h"
#include "places-airports.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <unistd.h>
#include <cassert>
#include "airports_kd_tree.h"

const char *programUsage[] = {
  "Usage: To search by name with optional state when result is ambiguous",
  "       client <places-host> <city> [state]",
  "",
  "       Use -p flag to search by latitude / longitude:",
  "       client -p <places-host> <latitude> <longitude>",
};

void showUsageAndExit() {
  for (const char *line : programUsage) {
    std::cerr << line << std::endl;
  }
  exit(1);
}

void parseArgs(int argc, char **argv, char **host, places_req &req) {
  bool isLatLongQuery = false;

  int c;
  while ((c = getopt(argc, argv, "p")) != -1) {
    switch (c) {
    case 'p':
      isLatLongQuery = true;
      break;
    case '?':
      if (isprint(optopt))
        std::cerr << "Unknown option '-" << (char)optopt << "'.\n";
      else
        std::cerr << "Unknown option char '" << std::hex << optopt << "'.\n";
      showUsageAndExit();
      break;
    default:
      abort();
    }
  }

  argc -= optind;
  argv += optind;

  if (argc < 2 || 3 < argc ||
      (isLatLongQuery && argc != 3)) {
    showUsageAndExit();
  }

  req.req_type = isLatLongQuery ? REQ_LAT_LONG : REQ_NAMED;

  *host = argv[0];

  if (!isLatLongQuery) {
    req.places_req_u.named.name = argv[1];
    req.places_req_u.named.state = (argc == 3) ? argv[2] : (char*)"";
  } else {
    double latitude;
    double longitude;
    try {
      latitude = std::stod(argv[1]);
      longitude = std::stod(argv[2]);
    }
    catch (...) {
      std::cerr << "Invalid latitude / longitude argument." << std::endl;
      exit(1);
    }
    req.places_req_u.loc.latitude = latitude;
    req.places_req_u.loc.longitude = longitude;
  }
}




void log_args(char * host, const places_req & req) {
#ifndef NDEBUG
  std::cout << "Host: " << host << std::endl;
  if (req.req_type == REQ_NAMED) {
    std::cout << req.places_req_u.named.name << ", "
      << req.places_req_u.named.state << std::endl;
  } else {
    std::cout << "Lat / long: " << std::endl;
    std::cout << req.places_req_u.loc.latitude << ", "
      << req.places_req_u.loc.longitude << std::
      endl;
  }
#endif
}

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

places_ret * places_qry_1_svc(places_req *req, struct svc_req *svcReq) {
  static places_ret result;
  static std::string errorMessage;

  // TODO: free memory, client request only!

  // Note: result is not freed - it doesn't own any alloc memory
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

  // TODO: query the location
  if (result.err == 0) {
    airports_ret * ret = airports_qry_1_svc(&result.places_ret_u.results.request.loc, nullptr);
    if (ret->err) {
      result.err = ret->err;
      result.places_ret_u.error_msg = ret->airports_ret_u.error_msg;
    } else {
      memcpy(&result.places_ret_u.results.results, 
             &ret->airports_ret_u.results,
             sizeof(airports));
    }
  }

  return &result;
}

airports_ret * airports_qry_1_svc(location *argp, struct svc_req *rqstp) {
  static airports_ret result;
  result = {};

  const auto closest = kd5Closest(*argp);
  memcpy(&result.airports_ret_u.results, closest, sizeof(airports));

  return &result;
}

void queryShowResults(location *t) {
  const auto ret = airports_qry_1_svc(t, nullptr);
  std::cout << *ret << std::endl;
}


int main(int argc, char **argv) {
  char *host = nullptr;   // Host of the places server
  places_req req;         // Request to the places server

  // Parse arguments

  parseArgs(argc, argv, &host, req);
  log_args(host, req);

  // Load data

  const char* placesPath = "places2k.txt";
  int placesCountHint = 20000;

  const char* airportsPath = "airport-locations.txt";

  try {
    initTrie(placesPath);
    initKD(airportsPath);
  }
  catch (const std::exception &e) {
    exitWithMessage(e.what());
  }

  // Test args

  places_ret * placesResult = places_qry_1_svc(&req, nullptr);
  std::cout << *placesResult << std::endl;

  // Test KD

  ////Location target{ 47.626353, -122.333144 };
  location target{ 40.704235, -73.917931 };

  queryShowResults(&target);
  target = { 47.626353, -122.333144 };
  queryShowResults(&target);


  // FOR TESTING BELOW

  std::cout << "> ";
  std::string line;
  while (std::getline(std::cin, line)) {
    std::stringstream strm(line);
    std::string city;
    std::string state;
    strm >> city;
    req.req_type = city == "-l" ?  REQ_LAT_LONG : REQ_NAMED;
    if (req.req_type == REQ_NAMED) {
      strm >> state;
      req.places_req_u.named.name = (char*)city.c_str();
      req.places_req_u.named.state = (char*)state.c_str();
    } else {
      double lat, lon;
      strm >> lat >> lon;
      req.places_req_u.loc.latitude = lat;
      req.places_req_u.loc.longitude = lon;
    }

    places_ret * placesRet = places_qry_1_svc(&req, nullptr);
    std::cout << *placesRet << std::endl;

    std::cout << "> ";
  }
}
