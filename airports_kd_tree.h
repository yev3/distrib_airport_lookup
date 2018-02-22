/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/
#pragma once
#include "common.h"
#include <memory>

/** Type of collection of airports loaded from the airports file */
using TAirportRecs = std::unique_ptr<std::vector<AirportRecord>>;

// Public interface methods to init and search
/******************************************************************************/

/**
 * \brief Initializes the kd tree data structure with given data. 
 * Throws on IO/file format error.
 * \param airportsPath Path to the airports file to load
 */
extern "C" void initKD(const char* airportsPath);

/**
 * \brief Performs a k-NN lookup to get 5 closest airports. 
 * Returns pointer to static data structure that does not need to be freed.
 * \param target Latitude / longitude target location to perform search
 * \return Ptr to a static arr of 5 elems that does not need to be freed.
 */
 airport* kd5Closest(const location target);

/**
 * K-D Tree node.
 */
struct KDNode {
  AirportRecord airport;          ///< Airport entry
  std::unique_ptr<KDNode> left;   ///< Left KD subtree
  std::unique_ptr<KDNode> right;  ///< Right KD subtree

  /**
   * \brief Constructs a K-D Tree node.
   * \param pt Airport record
   * \param lft Left subtree
   * \param rgt Right subtree
   */
  KDNode(AirportRecord pt, std::unique_ptr<KDNode> lft,
         std::unique_ptr<KDNode> rgt);
};

/**
 * \brief Airports K-D tree that allows for a closest locations query
 */
class AirportKDTree {
public:
  /**
   * \brief Takes ownership of air records and constructs a K-D tree.
   * \param airRecs Airport records to take ownership of
   */
  explicit AirportKDTree(TAirportRecs airRecs);

  /**
   * \brief Collects k closest locations to the target
   * \param target Target location to collect closest to
   * \param k Number of closest collections to collect
   * \return Closest k locations
   */
  std::vector<DistAirport>
  kClosestLocations(const location target, const size_t k = 5) const;

  /**
   * \brief Get number of airport records loaded into the kd tree.
   * \return Number of airport records in the tree
   */
  size_t size() const;

private:
  TAirportRecs airports;        ///< Airports loaded from file
  std::unique_ptr<KDNode> root; ///< Root Node of the K-D Tree
};
