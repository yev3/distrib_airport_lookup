#pragma once
#include "places-airports.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 
/******************************************************************************/

#ifndef NDEBUG
#define log_printf(fmt, ...) \
        do { fprintf(stderr, "[%s()]: " fmt "\n", \
                     __func__, __VA_ARGS__); } while (0)
#else
#define log_printf(X)
#endif

// Case-insensitive comparison
/******************************************************************************/

#if defined(_WIN32) || defined(_WIN64) 
  #define strcasecmp _stricmp 
  #define strncasecmp _strnicmp 
#endif

// Error handling
/******************************************************************************/

void exitWithMessage(const char *msg);

void exitWithMessage(const std::string &msg);

// CityRecord + AirportRecord
/******************************************************************************/

/**
 * \brief City record entry
 */
struct CityRecord {
  std::string cityName;
  std::string state;
  location loc{};

  CityRecord();

  CityRecord(std::string cityName_, 
             std::string state_,
             const location &loc_);
};

/**
 * \brief An airport record.
 */
struct AirportRecord {
  location loc;       ///< Location in lat / long
  std::string code;   ///< Airport 3-digit code
  std::string name;   ///< Full airport name
  std::string state;  ///< Airport state

  /**
   * \brief Construct an airport record.
   * \param location Location in lat / long
   * \param acode 3-digit airport code
   * \param aname Full airport name
   * \param astate Airport state it's located in
   */
  explicit AirportRecord(location location, std::string acode,
                         std::string aname, std::string astate);
};

/**
 * \brief Airport record query result
 */
struct DistAirport {
  const AirportRecord *airport;  ///< Airport result (by value since objects are small)
  double dist;            ///< Distance from query target in statute miles

  /**
   * \brief Construct query result
   * \param airRef Result airport
   * \param adist Distance to target in statute miles
   */
  DistAirport(const AirportRecord &airRef, const double adist);


  /**
   * \brief Order the query result by distance
   * \param other Other query result
   * \return True when this object is closer
   */
  bool operator<(const DistAirport &other) const;
};

// Stream formatting functions
/******************************************************************************/

std::ostream &operator<<(std::ostream &strm, const location &loc);

std::ostream &operator<<(std::ostream &strm, const place &pl);;

std::ostream &operator<<(std::ostream &strm, const places_ret &plRet);

std::ostream &operator<<(std::ostream &strm, const CityRecord &rec);

std::ostream &operator<<(std::ostream &strm, const DistAirport &rec);

std::ostream &operator<<(std::ostream &strm, const airport &airp);

std::ostream &operator<<(std::ostream &strm, const airports_ret &airRet);

template <typename TElem>
std::ostream& operator<<(std::ostream &strm, const std::vector<TElem>& elems) {
  for (size_t i = 0; i < elems.size(); ++i) {
    strm << i + 1 << ". " << elems[i] << std::endl;
  }
  return strm;
}

