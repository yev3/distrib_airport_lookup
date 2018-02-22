/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/

#include "places-airports.h"
#include "airports_kd_tree.h"

airports_ret * airports_qry_1_svc(location *argp, struct svc_req *rqstp) {
	static airports_ret  result;

  // Zero-out previous query
  result = { };

  // Note: no need to free memory from previous query, since all is static
  //       airport_ret contains pointers to airport records in KD tree

  // Find 5 closest
  airport *const closest = kd5Closest(*argp);

  // Copy over pointers from results
  memcpy(&result.airports_ret_u.results, closest, sizeof(airports));

  return &result;
}