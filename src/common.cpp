/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/

#include "common.h"
#include <iomanip>
#include <iostream>
#include <utility>

// Error handling
/******************************************************************************/

void exitWithMessage(const char *msg) {
  if (errno)
    perror(msg);
  else
    std::cerr << msg << std::endl;
  exit(-1);
}

void exitWithMessage(const std::string &msg) {
  exitWithMessage(msg.c_str());
}

// CityRecord + AirportRecord
/******************************************************************************/

CityRecord::CityRecord() = default;

CityRecord::CityRecord(std::string cityName_,
                       std::string state_,
                       const location &loc_) :
  cityName(std::move(cityName_)), state(std::move(state_)), loc(loc_) {}

AirportRecord::AirportRecord(location location, std::string acode,
                             std::string aname, std::string astate):
  loc(location), code(std::move(acode)),
  name(std::move(aname)), state(std::move(astate)) {}

DistAirport::DistAirport(const AirportRecord &airRef, const double adist):
  airport(&airRef), dist(adist) { }

bool DistAirport::operator<(const DistAirport &other) const {
  return dist < other.dist;
}

// Stream formatting functions
/******************************************************************************/

std::ostream &operator<<(std::ostream &strm, const location &loc) {
  strm << "{ ";
  strm << std::setw(7) << std::fixed << std::setprecision(2) << std::right
       << loc.latitude << std::setw(0) << ", " << std::setw(7)
       << loc.longitude << std::setw(0) << " }";
  return strm;
}

std::ostream &operator<<(std::ostream &strm, const DistAirport &rec) {
  strm << std::setw(5) << std::right << std::fixed << std::setprecision(1) 
       << rec.dist << std::left << std::setw(0) << " " << rec.airport;
  return strm;
}

std::ostream &operator<<(std::ostream &strm, const airport &airp) {
  strm << std::setw(5) << std::right << std::fixed << std::setprecision(1)
    << airp.dist << std::left << std::setw(0) << " "
    << airp.code << "  "
    << std::setw(5) << std::left << airp.state << std::setw(0) << std::left
    << " " << airp.name;
  return strm;
}

std::ostream& listAirports(std::ostream &strm, 
                           const airport *airp, int size) {
  strm << "#   Dist Code State Name" << std::endl
    << "--+-----+----+-----+------------------------------------------"
    << std::endl;

  for (int i = 0; i < NRESULTS; ++i) {
    strm << (i + 1) << ". " << airp[i] << std::endl;
  }
  return strm;
}

std::ostream &operator<<(std::ostream &strm, const airports_ret &airRet) {
  if (airRet.err) {
    strm << "Error: " << airRet.airports_ret_u.error_msg;
  } else {
    listAirports(strm, &airRet.airports_ret_u.results[0], NRESULTS);
  }

  return strm;
}

std::ostream &operator<<(std::ostream &strm, const place &pl) {
  return strm << pl.name << ", " << pl.state << " " << pl.loc;
}


std::ostream &operator<<(std::ostream &strm, const places_ret &plRet) {
  if (plRet.err) {
    strm << "Error: " << plRet.places_ret_u.error_msg;
  } else {
    strm << plRet.places_ret_u.results.request << std::endl;
    listAirports(strm, &plRet.places_ret_u.results.results[0], NRESULTS);
  }

  return strm;
}

std::ostream &operator<<(std::ostream &strm, const CityRecord &rec) {
  strm << std::setw(25) << std::left << rec.cityName;
  strm << std::setw(0) << ", " << rec.state << " " << rec.loc;
  return strm;
}
